#include "ReflectionStaticManager.h"
#include "GarbageCollector.h"
#include "ObjectTypeDictionary.h"

using namespace Reflection;

CReflectionStaticManager::SStaticConstructor CReflectionStaticManager::StaticConstructor;

Reflection::CReflectionStaticManager::SStaticConstructor::SStaticConstructor()
{
}

Reflection::CReflectionStaticManager::SStaticConstructor::~SStaticConstructor()
{
	if (CGarbageCollector::Instance())
	{
		CGarbageCollector::Instance()->DeleteAllRootObjects();

		delete CGarbageCollector::Instance();
	}

	if (CObjectTypeDictionary::Instance())
	{
		delete CObjectTypeDictionary::Instance();
	}
}