#pragma once

#include <string>
#include <vector>

namespace Reflection
{
	class CObjectType;

	enum class EObjectType
	{
		None,
		Class,
		Struct,
	};

	enum class EAccessType
	{
		Public,
		Protected,
		Private,
	};

	enum class ECollectibleType
	{
		None,
		ClassPtr,
		Struct,
	};

	enum class ECollectibleContainerType
	{
		None,
		FirstClassPtr,
		FirstStruct,
		SecondClassPtr,
		SecondStruct,
		FirstClassPtrSecondClassPtr,
		FirstClassPtrSecondStruct,
		FirstValueSecondClassPtr,
		FirstValueSecondStruct,
	};

	class IToString
	{
	public:
		virtual ~IToString() {}
		virtual std::string ToString() const { return ""; }
	};

	class CAttribute
	{
	public:
		CAttribute() {}
		virtual ~CAttribute() {}
	};

	class CNativeTypeInfo : IToString
	{
		bool bIsConst;
		EObjectType Forward;
		std::string Namespaces;
		std::string TypeName;
		int NumPointers;
		bool bIsReference;
		size_t TypeSize;

	public:
		CNativeTypeInfo(bool InbIsConst, EObjectType InForward, std::string InNamespaces, std::string InTypeName, int InNumPointers, bool InbIsReference, size_t InTypeSize);
		CNativeTypeInfo(CNativeTypeInfo&& Other) noexcept;

		CNativeTypeInfo& operator = (CNativeTypeInfo&& Other) noexcept;

		bool IsConst() const { return bIsConst; }

		EObjectType GetForward() const { return Forward; }

		const std::string& GetNamespaces() const { return Namespaces; }

		const std::string& GetTypeName() const { return TypeName; }

		std::string GetTypeString() const;

		int GetNumPointers() const { return NumPointers; }

		bool IsReference() const { return bIsReference; }

		size_t GetTypeSize() const { return TypeSize; }

		ECollectibleType IsCollectible() const;

		virtual std::string ToString() const override;
	};

	class CNativeType : public IToString
	{
	protected:
		CNativeTypeInfo* TypeInfo;

	public:
		CNativeType(CNativeTypeInfo* InTypeInfo);
		CNativeType(CNativeType&& Other) noexcept;

		CNativeType& operator = (CNativeType&& Other) noexcept;

		virtual ~CNativeType();

		const CNativeTypeInfo* GetTypeInfo() const { return TypeInfo; }

		const std::string& GetTypeName() const { return TypeInfo->GetTypeName(); }

		size_t GetTypeSize() const { return TypeInfo->GetTypeSize(); }

		ECollectibleType IsCollectible() const { return TypeInfo->IsCollectible(); }

		int GetNumPointers() const { return TypeInfo->GetNumPointers(); }

		virtual std::string ToString() const override;
	};

	class CTemplateParameter : public CNativeType
	{
		int Index;
		bool bIsUnknownType;

	public:
		CTemplateParameter(CNativeTypeInfo* InTypeInfo, int InIndex, bool InbIsUnknownType);
		CTemplateParameter(CTemplateParameter&& Other) noexcept;

		CTemplateParameter& operator = (CTemplateParameter&& Other) noexcept;

		virtual ~CTemplateParameter();

		int GetIndex() const { return Index; }

		bool IsUnknownType() const { return bIsUnknownType; }

		virtual std::string ToString() const override;
	};

	class CType : public CNativeType
	{
		std::vector<CTemplateParameter*> TemplateParameters;

	public:
		CType(CNativeTypeInfo* InTypeInfo, std::vector<CTemplateParameter*> InTemplateParameters);
		CType(CType&& Other) noexcept;

		CType& operator = (CType&& Other) noexcept;

		virtual ~CType();

		const std::vector<CTemplateParameter*>& GetTemplateParameters() const { return TemplateParameters; }

		bool IsUnknownType() const;

		bool IsTemplate() const;

		ECollectibleContainerType IsCollectibleContainerType() const;

		std::string GetTypeString() const;

		virtual std::string ToString() const override;
	};

	class CVariableInfo : public IToString
	{
		CType* Type;
		std::string Name;

	public:
		CVariableInfo(CType* InType, std::string InName);
		CVariableInfo(CVariableInfo&& Other) noexcept;

		CVariableInfo& operator = (CVariableInfo&& Other) noexcept;

		virtual ~CVariableInfo();

		const CType* GetType() const { return Type; }

		const std::string& GetName() const { return Name; }

		const std::string& GetTypeName() const { return Type->GetTypeName(); }

		size_t GetTypeSize() const { return Type->GetTypeSize(); }

		ECollectibleType IsCollectible() const { return Type->IsCollectible(); }

		ECollectibleContainerType IsCollectibleContainerType() const { return Type->IsCollectibleContainerType(); }

		bool IsTemplate() const { Type->IsTemplate(); }

		const std::vector<CTemplateParameter*>& GetTemplateParameters() const { return Type->GetTemplateParameters(); }

		virtual std::string ToString() const override;
	};

	class CMemberInfo : public IToString
	{
		std::vector<std::string> PreProcessors;
		std::vector<const CAttribute*> Attributes;
		EAccessType AccessType;
		std::string DeclaringType;

	public:
		CMemberInfo(std::vector<std::string> InPreProcessors, std::vector<const CAttribute*> InAttributes, EAccessType InAccessType, std::string InDeclaringType);
		CMemberInfo(CMemberInfo&& Other) noexcept;

		CMemberInfo& operator = (CMemberInfo&& Other) noexcept;

		virtual ~CMemberInfo();

		const std::vector<std::string>& GetPreProcessors() const { return PreProcessors; }

		const std::vector<const CAttribute*>& GetAttributes() const { return Attributes; }

		EAccessType GetAccessType() const { return AccessType; }

		std::string GetAccessTypeString() const;

		const std::string& GetDeclaringType() const { return DeclaringType; }

		virtual std::string ToString() const override;
	};

	class CVariable : public IToString
	{
		CVariableInfo* VariableInfo;

	public:
		CVariable(CVariableInfo* InVariableInfo);
		CVariable(CVariable&& Other) noexcept;

		CVariable& operator = (CVariable&& Other) noexcept;

		virtual ~CVariable();

		const CVariableInfo* GetVariableInfo() const { return VariableInfo; }

		std::string GetVariableString() const;

		const std::string& GetName() const { return VariableInfo->GetName(); }

		const std::string& GetTypeName() const { return VariableInfo->GetTypeName(); }

		size_t GetTypeSize() const { return VariableInfo->GetTypeSize(); }

		ECollectibleType IsCollectible() const { return VariableInfo->IsCollectible(); }

		ECollectibleContainerType IsCollectibleContainerType() const { return VariableInfo->IsCollectibleContainerType(); }

		bool IsTemplate() const { VariableInfo->IsTemplate(); }

		const std::vector<CTemplateParameter*>& GetTemplateParameters() const { return VariableInfo->GetTemplateParameters(); }

		CObjectType* GetObjectType() const;

		virtual std::string ToString() const override;
	};

	class CMember : public CVariable
	{
		CMemberInfo* MemberInfo;

	public:
		CMember(CVariableInfo* InVariableInfo, CMemberInfo* InMemberInfo);
		CMember(CMember&& Other) noexcept;

		CMember& operator = (CMember&& Other) noexcept;

		virtual ~CMember();

		CObjectType* GetDeclaringObjectType() const;

		const CMemberInfo* GetMemberInfo() const { return MemberInfo; }

		std::string GetMemberString() const;

		const std::vector<const CAttribute*>& GetAttributes() const { return MemberInfo->GetAttributes(); }

		template <typename T>
		const T* GetCustomAttribute() const;

		template <typename T>
		std::vector<const T*> GetCustomAttributes() const;

		virtual std::string ToString() const override;
	};

	template<typename T>
	inline const T* CMember::GetCustomAttribute() const
	{
		for (const CAttribute* Attribute : MemberInfo->GetAttributes())
		{
			T* ConvertedAttribute = dynamic_cast<T*>(Attribute);
			if (ConvertedAttribute != nullptr)
			{
				return ConvertedAttribute;
			}
		}
		return nullptr;
	}

	template<typename T>
	inline std::vector<const T*> CMember::GetCustomAttributes() const
	{
		std::vector<const T*> Result;
		for (const CAttribute* Attribute : MemberInfo->GetAttributes())
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

