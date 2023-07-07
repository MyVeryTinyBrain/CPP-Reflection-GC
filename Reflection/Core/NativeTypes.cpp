#include "NativeTypes.h"
#include "ObjectTypeDictionary.h"
#include "ObjectType.h"
#include "Containers.h"

using namespace Reflection;
using namespace std;

Reflection::CNativeTypeInfo::CNativeTypeInfo(bool InbIsConst, EObjectType InForward, std::string InNamespaces, std::string InTypeName, int InNumPointers, bool InbIsReference, size_t InTypeSize) :
	bIsConst(InbIsConst),
	Forward(InForward),
	Namespaces(InNamespaces),
	TypeName(InTypeName),
	NumPointers(InNumPointers),
	bIsReference(InbIsReference),
	TypeSize(InTypeSize)
{
}

Reflection::CNativeTypeInfo::CNativeTypeInfo(CNativeTypeInfo&& Other) noexcept
{
	*this = std::move(Other);
}

CNativeTypeInfo& Reflection::CNativeTypeInfo::operator=(CNativeTypeInfo&& Other) noexcept
{
	bIsConst = std::move(Other.bIsConst);
	Forward = std::move(Other.Forward);
	Namespaces = std::move(Other.Namespaces);
	TypeName = std::move(Other.TypeName);
	NumPointers = std::move(Other.NumPointers);
	bIsReference = std::move(Other.bIsReference);
	TypeSize = std::move(Other.TypeSize);

	return *this;
}

std::string Reflection::CNativeTypeInfo::GetTypeString() const
{
	string IsConstString = (bIsConst) ? "const " : "";
	string PointerString;
	for (int i = 0; i < NumPointers; ++i)
	{
		PointerString += "*";
	}
	string ReferenceString;
	if (bIsReference)
	{
		ReferenceString = "&";
	}

	return IsConstString + TypeName + PointerString + ReferenceString;
}

ECollectibleType Reflection::CNativeTypeInfo::IsCollectible() const
{
	if (bIsReference)
	{
		return ECollectibleType::None;
	}

	CObjectType* ObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TypeName);
	if (ObjectType == nullptr)
	{
		return ECollectibleType::None;
	}
	else
	{
		EObjectType Type = ObjectType->GetObjectType();
		if (Type == EObjectType::Class && NumPointers == 1)
		{
			return ECollectibleType::ClassPtr;
		}
		else if (Type == EObjectType::Struct && NumPointers == 0)
		{
			return ECollectibleType::Struct;
		}
	}
	return ECollectibleType::None;
}

std::string Reflection::CNativeTypeInfo::ToString() const
{
	return GetTypeString();
}

CNativeType::CNativeType(CNativeTypeInfo* InTypeInfo) :
	TypeInfo(InTypeInfo)
{
}

Reflection::CNativeType::CNativeType(CNativeType&& Other) noexcept
{
	*this = std::move(Other);
}

CNativeType& Reflection::CNativeType::operator=(CNativeType&& Other) noexcept
{
	TypeInfo = std::move(Other.TypeInfo);

	Other.TypeInfo = nullptr;

	return *this;
}

CNativeType::~CNativeType()
{
	if (TypeInfo)
	{
		delete TypeInfo;
		TypeInfo = nullptr;
	}
}

std::string Reflection::CNativeType::ToString() const
{
	return TypeInfo->ToString();
}

CTemplateParameter::CTemplateParameter(CNativeTypeInfo* InTypeInfo, int InIndex, bool InbIsUnknownType) :
	CNativeType(InTypeInfo),
	Index(InIndex),
	bIsUnknownType(InbIsUnknownType)
{
}

Reflection::CTemplateParameter::CTemplateParameter(CTemplateParameter&& Other) noexcept :
	CNativeType(std::move(Other))
{
	Index = std::move(Other.Index);
	bIsUnknownType = std::move(Other.bIsUnknownType);
}

CTemplateParameter& Reflection::CTemplateParameter::operator=(CTemplateParameter&& Other) noexcept
{
	CNativeType::operator=(std::move(Other));

	Index = std::move(Other.Index);
	bIsUnknownType = std::move(Other.bIsUnknownType);

	return *this;
}

CTemplateParameter::~CTemplateParameter()
{
}

std::string Reflection::CTemplateParameter::ToString() const
{
	if (bIsUnknownType)
	{
		return TypeInfo->GetTypeName();
	}
	else
	{
		return CNativeType::ToString();
	}
}

Reflection::CType::CType(CNativeTypeInfo* InTypeInfo, std::vector<CTemplateParameter*> InTemplateParameters) :
	CNativeType(InTypeInfo),
	TemplateParameters(InTemplateParameters)
{
}

Reflection::CType::CType(CType&& Other) noexcept :
	CNativeType(std::move(Other))
{
	TemplateParameters = std::move(Other.TemplateParameters);

	Other.TemplateParameters.clear();
}

CType& Reflection::CType::operator=(CType&& Other) noexcept
{
	CNativeType::operator=(std::move(Other));

	TemplateParameters = std::move(Other.TemplateParameters);

	Other.TemplateParameters.clear();

	return *this;
}

Reflection::CType::~CType()
{
	for (auto& TemplateParameter : TemplateParameters)
	{
		if (TemplateParameter)
		{
			delete TemplateParameter;
			TemplateParameter = nullptr;
		}
	}
	TemplateParameters.clear();
}

bool Reflection::CType::IsUnknownType() const
{
	for (const auto& TemplateParameter : TemplateParameters)
	{
		if (TemplateParameter->IsUnknownType())
		{
			return true;
		}
	}
	return false;
}

bool Reflection::CType::IsTemplate() const
{
	return TemplateParameters.size() > 0;
}

ECollectibleContainerType Reflection::CType::IsCollectibleContainerType() const
{
	if (TypeInfo->IsReference())
	{
		return ECollectibleContainerType::None;
	}

	if (false == IsTemplate())
	{
		return ECollectibleContainerType::None;
	}

	if (GetTypeName() == TLIST_TYPENAME && TemplateParameters.size() >= 1)
	{
		CObjectType* ObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[0]->GetTypeName());
		if (ObjectType)
		{
			EObjectType Type =  ObjectType->GetObjectType();
			if (Type == EObjectType::Class && TemplateParameters[0]->GetNumPointers() == 1)
			{
				return ECollectibleContainerType::FirstClassPtr;
			}
			if (Type == EObjectType::Struct && TemplateParameters[0]->GetNumPointers() == 0)
			{
				return ECollectibleContainerType::FirstStruct;
			}
		}
	}
	else if (GetTypeName() == TSET_TYPENAME && TemplateParameters.size() >= 1)
	{
		CObjectType* ObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[0]->GetTypeName());
		if (ObjectType)
		{
			EObjectType Type = ObjectType->GetObjectType();
			if (Type == EObjectType::Class && TemplateParameters[0]->GetNumPointers() == 1)
			{
				return ECollectibleContainerType::FirstClassPtr;
			}
			if (Type == EObjectType::Struct && TemplateParameters[0]->GetNumPointers() == 0)
			{
				return ECollectibleContainerType::FirstStruct;
			}
		}
	}
	else if (GetTypeName() == TMAP_TYPENAME && TemplateParameters.size() >= 2)
	{
		CObjectType* FirstObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[0]->GetTypeName());
		CObjectType* SecondObjectType = CObjectTypeDictionary::Instance()->GetObjectType(TemplateParameters[1]->GetTypeName());
		EObjectType FirstType = EObjectType::None;
		EObjectType SecondType = EObjectType::None;
		if (FirstObjectType)
		{
			FirstType = FirstObjectType->GetObjectType();
		}
		if (SecondObjectType)
		{
			SecondType = SecondObjectType->GetObjectType();
		}

		if (FirstType != EObjectType::None && SecondType == EObjectType::None)
		{
			if (FirstType == EObjectType::Class && TemplateParameters[0]->GetNumPointers() == 1)
			{
				return ECollectibleContainerType::FirstClassPtr;
			}
			if (FirstType == EObjectType::Struct && TemplateParameters[0]->GetNumPointers() == 0)
			{
				return ECollectibleContainerType::FirstStruct;
			}
		}
		else if (FirstType == EObjectType::None && SecondType != EObjectType::None)
		{
			if (SecondType == EObjectType::Class && TemplateParameters[1]->GetNumPointers() == 1)
			{
				return ECollectibleContainerType::SecondClassPtr;
			}
			if (SecondType == EObjectType::Struct && TemplateParameters[1]->GetNumPointers() == 0)
			{
				return ECollectibleContainerType::SecondStruct;
			}
		}
		else if (FirstType != EObjectType::None && SecondType != EObjectType::None)
		{
			if (FirstType == EObjectType::Class && TemplateParameters[0]->GetNumPointers() == 1 && SecondType == EObjectType::Class && TemplateParameters[1]->GetNumPointers() == 1)
			{
				return ECollectibleContainerType::FirstClassPtrSecondClassPtr;
			}
			else if (FirstType == EObjectType::Class && TemplateParameters[0]->GetNumPointers() == 1 && SecondType == EObjectType::Struct && TemplateParameters[1]->GetNumPointers() == 0)
			{
				return ECollectibleContainerType::FirstClassPtrSecondStruct;
			}
			else if (FirstType == EObjectType::Struct && TemplateParameters[0]->GetNumPointers() == 0 && SecondType == EObjectType::Class && TemplateParameters[1]->GetNumPointers() == 1)
			{
				return ECollectibleContainerType::FirstValueSecondClassPtr;
			}
			else if (FirstType == EObjectType::Struct && TemplateParameters[0]->GetNumPointers() == 0 && SecondType == EObjectType::Struct  && TemplateParameters[1]->GetNumPointers() == 0)
			{
				return ECollectibleContainerType::FirstValueSecondStruct;
			}
		}
	}

	return ECollectibleContainerType::None;
}

std::string Reflection::CType::GetTypeString() const
{
	if (IsTemplate())
	{
		string IsConstString = (TypeInfo->IsConst()) ? "const " : "";
		string PointerString;
		for (int i = 0; i < TypeInfo->GetNumPointers(); ++i)
		{
			PointerString += "*";
		}

		string TemplatesString;
		for (size_t i = 0; i < TemplateParameters.size(); ++i)
		{
			TemplatesString += TemplateParameters[i]->ToString();
			if (i < TemplateParameters.size() - 1)
			{
				TemplatesString += ',';
			}
		}

		return IsConstString + TypeInfo->GetTypeName() + '<' + TemplatesString + '>' + PointerString;
	}
	else
	{
		return CNativeType::ToString();
	}
}

std::string Reflection::CType::ToString() const
{
	return GetTypeString();
}

Reflection::CVariableInfo::CVariableInfo(CType* InType, std::string InName) :
	Type(InType),
	Name(InName)
{
}

Reflection::CVariableInfo::CVariableInfo(CVariableInfo&& Other) noexcept
{
	*this = std::move(Other);
}

CVariableInfo& Reflection::CVariableInfo::operator=(CVariableInfo&& Other) noexcept
{
	Type = std::move(Other.Type);
	Name = std::move(Other.Name);

	Other.Type = nullptr;

	return *this;
}

Reflection::CVariableInfo::~CVariableInfo()
{
	if (Type)
	{
		delete Type;
		Type = nullptr;
	}
}

std::string Reflection::CVariableInfo::ToString() const
{
	return Type->GetTypeString() + " " + Name;
}

Reflection::CVariable::CVariable(CVariableInfo* InVariableInfo) :
	VariableInfo(InVariableInfo)
{
}

Reflection::CVariable::CVariable(CVariable&& Other) noexcept
{
	*this = std::move(Other);
}

CVariable& Reflection::CVariable::operator=(CVariable&& Other) noexcept
{
	VariableInfo = std::move(Other.VariableInfo);

	Other.VariableInfo = nullptr;

	return *this;
}

Reflection::CVariable::~CVariable()
{
	if (VariableInfo)
	{
		delete VariableInfo;
		VariableInfo = nullptr;
	}
}

std::string Reflection::CVariable::GetVariableString() const
{
	return VariableInfo->ToString();
}

CObjectType* Reflection::CVariable::GetObjectType() const
{
	return CObjectTypeDictionary::Instance()->GetObjectType(GetTypeName());
}

std::string Reflection::CVariable::ToString() const
{
	return GetVariableString();
}

Reflection::CMember::CMember(CVariableInfo* InVariableInfo, CMemberInfo* InMemberInfo) :
	CVariable(InVariableInfo),
	MemberInfo(InMemberInfo)
{
}

Reflection::CMember::CMember(CMember&& Other) noexcept :
	CVariable(std::move(Other))
{
	*this = std::move(Other);
}

CMember& Reflection::CMember::operator=(CMember&& Other) noexcept
{
	CVariable::operator=(std::move(Other));

	MemberInfo = std::move(Other.MemberInfo);

	Other.MemberInfo = nullptr;

	return *this;
}

Reflection::CMember::~CMember()
{
	if (MemberInfo)
	{
		delete MemberInfo;
		MemberInfo = nullptr;
	}
}

CObjectType* Reflection::CMember::GetDeclaringObjectType() const
{
	return CObjectTypeDictionary::Instance()->GetObjectType(MemberInfo->GetDeclaringType());
}

std::string Reflection::CMember::GetMemberString() const
{
	return MemberInfo->GetAccessTypeString() + " " + GetVariableString();
}

std::string Reflection::CMember::ToString() const
{
	return GetMemberString();
}

Reflection::CMemberInfo::CMemberInfo(std::vector<std::string> InPreProcessors, std::vector<const CAttribute*> InAttributes, EAccessType InAccessType, std::string InDeclaringType) :
	PreProcessors(InPreProcessors),
	Attributes(InAttributes),
	AccessType(InAccessType),
	DeclaringType(InDeclaringType)
{
}

Reflection::CMemberInfo::CMemberInfo(CMemberInfo&& Other) noexcept
{
	*this = std::move(Other);
}

CMemberInfo& Reflection::CMemberInfo::operator=(CMemberInfo&& Other) noexcept
{
	PreProcessors = std::move(PreProcessors);
	Attributes = std::move(Attributes);
	AccessType = std::move(AccessType);
	DeclaringType = std::move(DeclaringType);

	Other.PreProcessors.clear();
	Other.Attributes.clear();

	return *this;
}

Reflection::CMemberInfo::~CMemberInfo()
{
	for (auto& Arribute : Attributes)
	{
		if (Arribute)
		{
			delete Arribute;
			Arribute = nullptr;
		}
	}
	Attributes.clear();
}

std::string Reflection::CMemberInfo::GetAccessTypeString() const
{
	switch (AccessType)
	{
		default:
		case EAccessType::Public:
			return "public";

		case EAccessType::Protected:
			return "protected";

		case EAccessType::Private:
			return "private";
	}
}

std::string Reflection::CMemberInfo::ToString() const
{
	return GetAccessTypeString();
}
