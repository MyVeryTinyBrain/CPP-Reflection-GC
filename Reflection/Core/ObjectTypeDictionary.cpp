#include "ObjectTypeDictionary.h"
#include "ObjectType.h"
#include "GarbageCollector.h"
#include <thread>
#include <vector>

using namespace Reflection;

CObjectTypeDictionary* CObjectTypeDictionary::GlobalInstnace = nullptr;

Reflection::CObjectTypeDictionary::CObjectTypeDictionary()
{
}

Reflection::CObjectTypeDictionary::~CObjectTypeDictionary()
{
}

CObjectTypeDictionary* Reflection::CObjectTypeDictionary::Instance()
{
	if (GlobalInstnace == nullptr)
	{
		GlobalInstnace = new CObjectTypeDictionary();
	}
	return GlobalInstnace;
}

void Reflection::CObjectTypeDictionary::CacheCollectibleFields()
{
	std::mutex TypeIndexMutex;
	size_t TypeIndex = 0;
	std::vector<std::thread> Threads;
	Threads.reserve(std::thread::hardware_concurrency());

	for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		Threads.push_back(std::thread([&]() 
		{ 
			while (true)
			{
				size_t ObjectTypeIndex;
				TypeIndexMutex.lock();
				{
					ObjectTypeIndex = TypeIndex++;
				}
				TypeIndexMutex.unlock();

				if (ObjectTypeIndex >= ObjectTypes.size())
				{
					return;
				}

				CObjectType* ObjectType = ObjectTypes[ObjectTypeIndex];
				ObjectType->CacheCollectibleFields();
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
}

void Reflection::CObjectTypeDictionary::AddObjectType(CObjectType* InObjectType)
{
	if (ObjectTypeMap.find(InObjectType->GetObjectName()) != ObjectTypeMap.end())
	{
		return;
	}

	size_t Index = ObjectTypes.size();
	ObjectTypes.push_back(InObjectType);
	ObjectTypeMap.insert(make_pair(InObjectType->GetObjectName(), Index));
}

bool Reflection::CObjectTypeDictionary::Contains(const std::string& InTypeName) const
{
	auto FindIterator = ObjectTypeMap.find(InTypeName);
	if (FindIterator == ObjectTypeMap.end())
	{
		return false;
	}
	return true;
}

size_t Reflection::CObjectTypeDictionary::GetObjectTypeIndex(const std::string& InObjectTypeName) const
{
	auto FindIterator = ObjectTypeMap.find(InObjectTypeName);
	if (FindIterator == ObjectTypeMap.end())
	{
		return -1;
	}

	size_t Index = FindIterator->second;
	return Index;
}

size_t Reflection::CObjectTypeDictionary::GetObjectTypeIndex(CObjectType* InObjectType) const
{
	return GetObjectTypeIndex(InObjectType->GetObjectName());
}

CObjectType* Reflection::CObjectTypeDictionary::GetObjectType(const std::string& InObjectTypeName) const
{
	size_t Index = GetObjectTypeIndex(InObjectTypeName);
	if (Index == -1)
	{
		return nullptr;
	}

	return ObjectTypes[Index];
}
