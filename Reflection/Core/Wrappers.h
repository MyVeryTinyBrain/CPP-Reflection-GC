#pragma once

#include <functional>

namespace Reflection
{
	class CObjectType;

	struct SVoidWrapper
	{
		void* Object;
		size_t ObjectSize;

		SVoidWrapper(void* InObject, size_t InObjectSize);

		bool operator == (const SVoidWrapper& Other) const;
		bool operator != (const SVoidWrapper& Other) const;
	};

	struct SObjectWrapper
	{
		void* Object;
		CObjectType* ObjectType;

		SObjectWrapper();
		SObjectWrapper(void* InObject, CObjectType* InObjectType);

		bool IsValid() const;

		bool operator == (const SObjectWrapper& Other) const;
		bool operator != (const SObjectWrapper& Other) const;
	};

	struct SVoidWrapperHasher
	{
		size_t operator()(const SVoidWrapper& Target) const
		{
			size_t ObjectPtrHash = std::hash<void*>()(Target.Object);
			size_t ObjectSizeHash = std::hash<size_t>()(Target.ObjectSize);
			return ObjectPtrHash ^ ObjectSizeHash;
		}
	};

	struct SObjectWrapperHasher
	{
		size_t operator()(const SObjectWrapper& Target) const
		{
			size_t ObjectPtrHash = std::hash<void*>()(Target.Object);
			size_t ObjectTypeHash = std::hash<const CObjectType*>()(Target.ObjectType);
			return ObjectPtrHash ^ ObjectTypeHash;
		}
	};

	template<typename T>
	Reflection::SVoidWrapper MakeVoidWrapper(T& InObject)
	{
		return Reflection::SVoidWrapper(&InObject, sizeof(InObject));
	}

	template<typename T>
	Reflection::SObjectWrapper MakeObjectWrapper(T& InObject)
	{
		return Reflection::SObjectWrapper(&InObject, InObject.ObjectType());
	}

	template<typename T>
	Reflection::SObjectWrapper MakeObjectWrapper(const T& InObject)
	{
		return Reflection::SObjectWrapper(&InObject, InObject.ObjectType());
	}

	template<typename T>
	Reflection::SObjectWrapper MakeObjectWrapper(T* InObject)
	{
		return Reflection::SObjectWrapper(InObject, InObject->ObjectType());
	}
}

