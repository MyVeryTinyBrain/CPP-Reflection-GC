#include "Wrappers.h"

Reflection::SVoidWrapper::SVoidWrapper(void* InObject, size_t InObjectSize) :
	Object(InObject),
	ObjectSize(InObjectSize)
{
}

bool Reflection::SVoidWrapper::operator==(const SVoidWrapper& Other) const
{
	return (Object == Other.Object) && (ObjectSize == Other.ObjectSize);
}

bool Reflection::SVoidWrapper::operator!=(const SVoidWrapper& Other) const
{
	return !SVoidWrapper::operator==(Other);
}

Reflection::SObjectWrapper::SObjectWrapper() :
	Object(nullptr),
	ObjectType(nullptr)
{
}

Reflection::SObjectWrapper::SObjectWrapper(void* InObject, CObjectType* InObjectType) :
	Object(InObject),
	ObjectType(InObjectType)
{
}

bool Reflection::SObjectWrapper::IsValid() const
{
	return (ObjectType != nullptr);
}

bool Reflection::SObjectWrapper::operator==(const SObjectWrapper& Other) const
{
	return (Object == Other.Object) && (ObjectType == Other.ObjectType);
}

bool Reflection::SObjectWrapper::operator!=(const SObjectWrapper& Other) const
{
	return !SObjectWrapper::operator==(Other);
}
