#pragma once

#include "ObjectTypes.h"
#include <functional>

namespace Reflection
{
	struct SCollectibleField
	{
		std::vector<CField*> ClassPtrFields;
		std::vector<CField*> StructFields;

		SCollectibleField() {}
		SCollectibleField(const SCollectibleField& Other);
		SCollectibleField(SCollectibleField&& Other) noexcept;

		SCollectibleField& operator = (const SCollectibleField& Other);
		SCollectibleField& operator = (SCollectibleField&& Other) noexcept;

		void Clear();
	};

	class CObjectType : public IToString
	{
		friend class CObjectTypeDictionary;

		EObjectType ObjectType;
		std::string ObjectName;
		std::string SuperName;
		std::string SuperNamespaces;
		std::function<void(void*)> DeleteLambda;
		std::vector<CField*> Fields;
		std::vector<CFunction*> Functions;
		std::vector<std::string> PreProcessors;
		std::vector<const CAttribute*> Attributes;

		bool bCachedCollectibleFields;
		SCollectibleField CachedCollectibleFields;
		SCollectibleField CachedCollectibleFirstContainerFields;
		SCollectibleField CachedCollectibleSecondContainerFields;

	public:
		CObjectType();
		CObjectType(
			EObjectType InObjectType,
			const std::string InObjectName,
			const std::string InSuperName,
			const std::string InSuperNamespaces,
			std::vector<CField*> InFields,
			std::vector<CFunction*> InFunctions,
			std::vector<std::string> InPreProcessors,
			std::vector<const CAttribute*> InAttributes,
			std::function<void(void*)> InDeleteLambda);
		CObjectType(CObjectType&& Other) noexcept;

		CObjectType& operator = (CObjectType&& Other) noexcept;

		virtual ~CObjectType();

		EObjectType GetObjectType() const { return ObjectType; }

		const std::string& GetObjectName() const { return ObjectName; }

		const std::string& GetSuperName() const { return SuperName; }

		const std::vector<CField*>& GetFields() const { return Fields; };
		std::vector<CField*> GetInheritedFields() const;
		std::vector<CField*> GetInheritedSortingFields() const;
		bool IsCachedCollectibleFields() const { return bCachedCollectibleFields; }
		void CacheCollectibleFields();
		const SCollectibleField& GetCachedCollectibleFields() const { return CachedCollectibleFields; }
		const SCollectibleField& GetCachedCollectibleFirstContainerFields() const { return CachedCollectibleFirstContainerFields; }
		const SCollectibleField& GetCachedCollectibleSecondContainerFields() const { return CachedCollectibleSecondContainerFields; }

		const std::vector<CFunction*>& GetFunctions() const { return Functions; };
		std::vector<CFunction*> GetInheritedFunctions() const;
		std::vector<CFunction*> GetInheritedSortingFunctions() const;

		const std::vector<const CAttribute*>& GetAttributes() const { return Attributes; };
		std::vector<const CAttribute*> GetInheritedAttributes() const;
		std::vector<const CAttribute*> GetInheritedSortingAttributes() const;

		template <typename T>
		const T* GetCustomAttribute() const;
		template <typename T>
		std::vector<const T*> GetCustomAttributes() const;
		template <typename T>
		const T* GetInheritedCustomAttribute() const;
		template <typename T>
		std::vector<const T*> GetInheritedCustomAttributes() const;

		bool HasSuperObject() const;
		CObjectType* GetSuperObjectType() const;

		bool CanUpCastingAs(const CObjectType* Other) const;
		bool CanDownCastingAs(const CObjectType* Other) const;
		bool CanCastingAs(const CObjectType* Other) const;

		void DeleteObject(void* InObject) const;

		std::string GetObjectTypeString() const;

		virtual std::string ToString() const override;
	};

	template<typename T>
	inline const T* CObjectType::GetCustomAttribute() const
	{
		for (const CAttribute* Attribute : Attributes)
		{
			const T* ConvertedAttribute = dynamic_cast<const T*>(Attribute);
			if (ConvertedAttribute != nullptr)
			{
				return ConvertedAttribute;
			}
		}
		return nullptr;
	}

	template<typename T>
	inline std::vector<const T*> CObjectType::GetCustomAttributes() const
	{
		std::vector<const T*> Result;
		for (const CAttribute* Attribute : Attributes)
		{
			const T* ConvertedAttribute = dynamic_cast<const T*>(Attribute);
			if (ConvertedAttribute != nullptr)
			{
				Result.push_back(ConvertedAttribute);
			}
		}
		return Result;
	}

	template<typename T>
	inline const T* CObjectType::GetInheritedCustomAttribute() const
	{
		for (const CAttribute* Attribute : GetInheritedAttributes())
		{
			const T* ConvertedAttribute = dynamic_cast<const T*>(Attribute);
			if (ConvertedAttribute != nullptr)
			{
				return ConvertedAttribute;
			}
		}
		return nullptr;
	}

	template<typename T>
	inline std::vector<const T*> CObjectType::GetInheritedCustomAttributes() const
	{
		std::vector<const T*> Result;
		for (const CAttribute* Attribute : GetInheritedAttributes())
		{
			const T* ConvertedAttribute = dynamic_cast<const T*>(Attribute);
			if (ConvertedAttribute != nullptr)
			{
				Result.push_back(ConvertedAttribute);
			}
		}
		return Result;
	}
}

