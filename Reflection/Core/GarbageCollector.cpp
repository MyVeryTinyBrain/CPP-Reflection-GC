#include "GarbageCollector.h"
#include "ObjectTypeDictionary.h"
#include "Object.h"
#include "Containers.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace Reflection;

CGarbageCollector* CGarbageCollector::GlobalInstnace = nullptr;

Reflection::CGarbageCollector::CGarbageCollector()
{
	bCachedCollectibleFields = false;
}

Reflection::CGarbageCollector::~CGarbageCollector()
{
}

CGarbageCollector* Reflection::CGarbageCollector::Instance()
{
	if (GlobalInstnace == nullptr)
	{
		GlobalInstnace = new CGarbageCollector();
	}
	return GlobalInstnace;
}

void Reflection::CGarbageCollector::RegistObject(CObject* InObject)
{
	Objects.insert(InObject);
}

void Reflection::CGarbageCollector::RegistRootObject(CObject* InObject)
{
	RegistObject(InObject);
	RootObjects.insert(InObject);
}

void Reflection::CGarbageCollector::UnregistObject(CObject* InObject)
{
	Objects.erase(InObject);
}

void Reflection::CGarbageCollector::UnregistRootObject(CObject* InObject)
{
	RootObjects.erase(InObject);
}

bool Reflection::CGarbageCollector::IsRootObject(CObject* InObject) const
{
	return (RootObjects.find(InObject) != RootObjects.end());
}

void Reflection::CGarbageCollector::DeleteAllRootObjects()
{
	for (CObject* RootObject : RootObjects)
	{
		if (RootObject)
		{
			RootObject->GCDestructor();
			delete RootObject;
		}
		Objects.erase(RootObject);
	}
	RootObjects.clear();

	Collect();
}

void Reflection::CGarbageCollector::Collect()
{
	CacheCollectibleFields();
	InitializeCollect();

	size_t NumWaitingThreads = 0;
	std::mutex NumWaitingThreadsMutex;
	std::vector<std::thread> Threads;
	Threads.reserve(std::thread::hardware_concurrency());

	for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		Threads.push_back(std::thread([&]()
		{
			std::mutex SleepMutex;
			while (true)
			{
				SObjectWrapper ObjectWrapper;
				// 스택에서 오브젝트를 Pop합니다.
				if (false == ThreadSafePopFromCandidates(&ObjectWrapper))
				{
					// 스택이 비어 있어, Pop에 실패한 경우입니다.

					// 대기중인 스레드 카운트를 증가시킵니다.
					size_t CurrentWaitingThreads;
					NumWaitingThreadsMutex.lock();
					{
						++NumWaitingThreads;
						CurrentWaitingThreads = NumWaitingThreads;
					}
					NumWaitingThreadsMutex.unlock();
					// 모든 스레드가 대기중이면 알림을 발생시킵니다.
					if (CurrentWaitingThreads == std::thread::hardware_concurrency())
					{
						CollectThreadsWakeEvent.notify_all();
						return;
					}
					// 오브젝트가 스택에 Push되거나, 모든 스레드가 대기중일 때 까지 대기합니다.
					std::unique_lock<std::mutex> Sleep(SleepMutex);
					CollectThreadsWakeEvent.wait(Sleep);
					// 대기중인 스레드 카운트를 갱신합니다.
					NumWaitingThreadsMutex.lock();
					{
						CurrentWaitingThreads = NumWaitingThreads;
					}
					NumWaitingThreadsMutex.unlock();
					// 모든 스레드가 대기중이면 종료합니다.
					if (CurrentWaitingThreads == std::thread::hardware_concurrency())
					{
						return;
					}
					// 대기중인 스레드 카운트를 감소시킵니다.
					NumWaitingThreadsMutex.lock();
					{
						--NumWaitingThreads;
					}
					NumWaitingThreadsMutex.unlock();
					continue;
				}
				else
				{
					// Pop한 오브젝트를 마킹하고, 멤버 변수들을 스택에 Push합니다.
					HandleCandidate(ObjectWrapper);
				}
			}
		}));
	}

	for (std::thread& Thread : Threads)
	{
		if (Thread.joinable())
		{
			Thread.join();
		}
	}

	// 삭제 대상인 오브젝트들을 삭제합니다.
	for (CObject* Object : Objects)
	{
		if (Object)
		{
			Object->GCDestructor();
			delete Object;
		}
	}
	// 참조중인 오브젝트 셋과 교체합니다.
	Objects.swap(ReferencedObjects);
}

void Reflection::CGarbageCollector::CacheCollectibleFields()
{
	static bool Cached = false;
	if (false == Cached)
	{
		Cached = true;
		CObjectTypeDictionary::Instance()->CacheCollectibleFields();
	}
}

void Reflection::CGarbageCollector::InitializeCollect()
{
	std::stack<SObjectWrapper> EmptyCandidates;
	Candidates.swap(EmptyCandidates);

	std::unordered_set<CObject*> EmptyReferencedObjects;
	ReferencedObjects.swap(EmptyReferencedObjects);

	for (CObject* RootObject : RootObjects)
	{
		PushToCandidates(MakeObjectWrapper(RootObject));
	}
}

void Reflection::CGarbageCollector::HandleCandidate(SObjectWrapper InObjectWrapper)
{
	switch (InObjectWrapper.ObjectType->GetObjectType())
	{
		case EObjectType::Class:
			HandleClass(InObjectWrapper);
			break;

		case EObjectType::Struct:
			HandleStruct(InObjectWrapper);
			break;
	}
}

void Reflection::CGarbageCollector::HandleClass(SObjectWrapper InObjectWrapper)
{
	std::unique_lock<std::mutex> Lock(ObjectsMutex);
	{
		CObject* Object = (CObject*)InObjectWrapper.Object;
		// 이 오브젝트를 마킹합니다.
		bool IsNewReference = ReferencedObjects.insert(Object).second;
		// 이 오브젝트가 이미 마킹되었으면 종료합니다.
		if (IsNewReference == false) 
		{
			return;
		}
		// 이 오브젝트를 삭제 대상 셋에서 제거합니다.
		Objects.erase(Object);
	}
	Lock.unlock();
	// 이 오브젝트의 멤버 변수들을 스택에 Push합니다.
	HandleObject(InObjectWrapper);
}

void Reflection::CGarbageCollector::HandleStruct(SObjectWrapper InObjectWrapper)
{
	HandleObject(InObjectWrapper);
}

void Reflection::CGarbageCollector::HandleObject(SObjectWrapper InObjectWrapper)
{
	const SCollectibleField& CollectibleFields = InObjectWrapper.ObjectType->GetCachedCollectibleFields();
	
	// 이 오브젝트의 모든 클래스 포인터 멤버 변수들을 스택에 Push합니다.
	for (CField* Field : CollectibleFields.ClassPtrFields)
	{
		CObject* FieldObject = *(CObject**)Field->GetPointedValue(InObjectWrapper);
		CObjectType* FieldObjectType = Field->GetObjectType();
		PushToCandidates(SObjectWrapper(FieldObject, FieldObjectType));
	}
	// 이 오브젝트의 모든 구조체 멤버 변수들을 스택에 Push합니다.
	for (CField* Field : CollectibleFields.StructFields)
	{
		void* FieldObject = (void*)Field->GetPointedValue(InObjectWrapper);
		CObjectType* FieldObjectType = Field->GetObjectType();
		PushToCandidates(SObjectWrapper(FieldObject, FieldObjectType));
	}

	std::vector<void**> FirstObjects;
	std::vector<void**> SecondObjects;

	const SCollectibleField& CollectibleFirstContainerFields = InObjectWrapper.ObjectType->GetCachedCollectibleFirstContainerFields();
	const SCollectibleField& CollectibleSecondContainerFields = InObjectWrapper.ObjectType->GetCachedCollectibleSecondContainerFields();	
	
	// 이 오브젝트의 템플릿 컨테이너 중에서 클래스 포인터를 저장하는 컨테이너들을 순회합니다.
	for (CField* Field : CollectibleFirstContainerFields.ClassPtrFields)
	{
		// 이 컨테이너가 저장하는 타입을 찾습니다.
		const std::vector<CTemplateParameter*>& TemplateParameters = Field->GetTemplateParameters();
		CObjectType* ConatinerKeyObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[0]->GetTypeName());
		// 이 컨테이너에 저장된 값들을 추출합니다.
		IContainerObjectExtracter* Extracter = (IContainerObjectExtracter*)Field->GetPointedValue(InObjectWrapper);
		FirstObjects.clear();
		Extracter->Append(FirstObjects, SecondObjects);
		// 이 컨테이너에 저장된 값들을 스택에 Push합니다.
		for (void** Ptr : FirstObjects)
		{
			CObject* ContainerKeyObject = *(CObject**)Ptr;
			PushToCandidates(SObjectWrapper(ContainerKeyObject, ConatinerKeyObjectType));
		}
	}

	for (CField* Field : CollectibleFirstContainerFields.StructFields)
	{
		const std::vector<CTemplateParameter*>& TemplateParameters = Field->GetTemplateParameters();
		CObjectType* ConatinerKeyObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[0]->GetTypeName());

		IContainerObjectExtracter* Extracter = (IContainerObjectExtracter*)Field->GetPointedValue(InObjectWrapper);
		FirstObjects.clear();
		Extracter->Append(FirstObjects, SecondObjects);
		for (void** Ptr : FirstObjects)
		{
			void* ContainerKeyObject = (void*)Ptr;
			PushToCandidates(SObjectWrapper(ContainerKeyObject, ConatinerKeyObjectType));
		}
	}

	for (CField* Field : CollectibleSecondContainerFields.ClassPtrFields)
	{
		const std::vector<CTemplateParameter*>& TemplateParameters = Field->GetTemplateParameters();
		CObjectType* ConatinerKeyObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[0]->GetTypeName());

		IContainerObjectExtracter* Extracter = (IContainerObjectExtracter*)Field->GetPointedValue(InObjectWrapper);
		SecondObjects.clear();
		Extracter->Append(FirstObjects, SecondObjects);
		for (void** Ptr : SecondObjects)
		{
			CObject* ContainerKeyObject = *(CObject**)Ptr;
			PushToCandidates(SObjectWrapper(ContainerKeyObject, ConatinerKeyObjectType));
		}
	}

	for (CField* Field : CollectibleSecondContainerFields.StructFields)
	{
		const std::vector<CTemplateParameter*>& TemplateParameters = Field->GetTemplateParameters();
		CObjectType* ConatinerKeyObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[0]->GetTypeName());

		IContainerObjectExtracter* Extracter = (IContainerObjectExtracter*)Field->GetPointedValue(InObjectWrapper);
		SecondObjects.clear();
		Extracter->Append(FirstObjects, SecondObjects);
		for (void** Ptr : SecondObjects)
		{
			void* ContainerKeyObject = (void*)Ptr;
			PushToCandidates(SObjectWrapper(ContainerKeyObject, ConatinerKeyObjectType));
		}
	}
}
void Reflection::CGarbageCollector::PushToCandidates(SObjectWrapper InObjectWrapper)
{
	if (InObjectWrapper.Object == nullptr)
	{
		return;
	}

	CandidatesMutex.lock();
	{
		Candidates.push(InObjectWrapper);
	}
	CandidatesMutex.unlock();
	
	CollectThreadsWakeEvent.notify_one();
}

bool Reflection::CGarbageCollector::ThreadSafePopFromCandidates(SObjectWrapper* OutObjectWrapper)
{
	std::unique_lock<std::mutex> Lock(CandidatesMutex);
	if (Candidates.empty())
	{
		return false;
	}
	*OutObjectWrapper = Candidates.top();
	Candidates.pop();
	return true;
}
