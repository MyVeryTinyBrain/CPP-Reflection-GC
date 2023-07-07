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
	UnregistObject(InObject);
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
				if (false == ThreadSafePopFromCandidates(&ObjectWrapper))
				{
					size_t CurrentWaitingThreads;
					NumWaitingThreadsMutex.lock();
					{
						++NumWaitingThreads;
						CurrentWaitingThreads = NumWaitingThreads;
					}
					NumWaitingThreadsMutex.unlock();

					if (CurrentWaitingThreads == std::thread::hardware_concurrency())
					{
						CollectThreadsWakeEvent.notify_all();
						return;
					}

					std::unique_lock<std::mutex> Sleep(SleepMutex);
					CollectThreadsWakeEvent.wait(Sleep);

					NumWaitingThreadsMutex.lock();
					{
						CurrentWaitingThreads = NumWaitingThreads;
					}
					NumWaitingThreadsMutex.unlock();

					if (CurrentWaitingThreads == std::thread::hardware_concurrency())
					{
						return;
					}

					NumWaitingThreadsMutex.lock();
					{
						--NumWaitingThreads;
					}
					NumWaitingThreadsMutex.unlock();

					continue;
				}
				else
				{
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

	for (CObject* Object : Objects)
	{
		if (Object)
		{
			Object->GCDestructor();
			delete Object;
		}
	}
	Objects.swap(ReferencedObjects);

	//CacheCollectibleFields();
	//InitializeCollect();	

	//while (false == Candidates.empty())
	//{
	//	SObjectWrapper ObjectWrapper = Candidates.top();
	//	Candidates.pop();

	//	HandleCandidate(ObjectWrapper);
	//}

	//for (CObject* Object : Objects)
	//{
	//	if (Object)
	//	{
	//		Object->GCDestructor();
	//		delete Object;
	//	}
	//}
	//Objects.swap(ReferencedObjects);
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

	CObject* Object = (CObject*)InObjectWrapper.Object;
	bool IsNewReference = ReferencedObjects.insert(Object).second;
	if (IsNewReference == false)
	{
		return;
	}

	Objects.erase(Object);

	Lock.unlock();

	HandleObject(InObjectWrapper);
}

void Reflection::CGarbageCollector::HandleStruct(SObjectWrapper InObjectWrapper)
{
	HandleObject(InObjectWrapper);
}

void Reflection::CGarbageCollector::HandleObject(SObjectWrapper InObjectWrapper)
{
	CandidatesMutex.lock();

	const SCollectibleField& CollectibleFields = InObjectWrapper.ObjectType->GetCachedCollectibleFields();

	for (CField* Field : CollectibleFields.ClassPtrFields)
	{
		CObject* FieldObject = *(CObject**)Field->GetPointedValue(InObjectWrapper);
		CObjectType* FieldObjectType = Field->GetObjectType();
		PushToCandidates(SObjectWrapper(FieldObject, FieldObjectType));
	}

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
	
	for (CField* Field : CollectibleFirstContainerFields.ClassPtrFields)
	{
		const std::vector<CTemplateParameter*>& TemplateParameters = Field->GetTemplateParameters();
		CObjectType* ConatinerKeyObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[0]->GetTypeName());

		IContainerObjectExtracter* Extracter = (IContainerObjectExtracter*)Field->GetPointedValue(InObjectWrapper);
		FirstObjects.clear();
		Extracter->Append(FirstObjects, SecondObjects);
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

	CandidatesMutex.unlock();
}
void Reflection::CGarbageCollector::PushToCandidates(SObjectWrapper InObjectWrapper)
{
	if (InObjectWrapper.Object == nullptr)
	{
		return;
	}

	Candidates.push(InObjectWrapper);
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
