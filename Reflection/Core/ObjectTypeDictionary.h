#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Reflection
{
	class CObjectType;

	class CObjectTypeDictionary
	{
		friend class CReflectionStaticManager;

		static CObjectTypeDictionary* GlobalInstnace;

		std::unordered_map<std::string, size_t> ObjectTypeMap;
		std::vector<CObjectType*> ObjectTypes;

	private:
		CObjectTypeDictionary();

		virtual ~CObjectTypeDictionary();

	public:
		static CObjectTypeDictionary* Instance();

		void CacheCollectibleFields();

		void AddObjectType(CObjectType* InObjectType);

		bool Contains(const std::string& InTypeName) const;

		size_t GetObjectTypeIndex(const std::string& InObjectTypeName) const;

		size_t GetObjectTypeIndex(CObjectType* InObjectType) const;

		CObjectType* GetObjectType(const std::string& InObjectTypeName) const;
	};
}
