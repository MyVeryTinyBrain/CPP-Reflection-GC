#include "ObjectType.h"
#include "ObjectTypeDictionary.h"
#include <stack>

using namespace Reflection;
using namespace std;

Reflection::SCollectibleField::SCollectibleField(const SCollectibleField& Other)
{
	*this = Other;
}

Reflection::SCollectibleField::SCollectibleField(SCollectibleField&& Other) noexcept
{
	*this = std::move(Other);
}

SCollectibleField& Reflection::SCollectibleField::operator=(const SCollectibleField& Other)
{
	ClassPtrFields = Other.ClassPtrFields;
	StructFields = Other.StructFields;

	return *this;
}

SCollectibleField& Reflection::SCollectibleField::operator=(SCollectibleField&& Other) noexcept
{
	ClassPtrFields = std::move(Other.ClassPtrFields);
	StructFields = std::move(Other.StructFields);

	Other.ClassPtrFields.clear();
	Other.StructFields.clear();

	return *this;
}

void Reflection::SCollectibleField::Clear()
{
	ClassPtrFields.clear();
	StructFields.clear();
}

Reflection::CObjectType::CObjectType() :
	CObjectType(EObjectType::None, "", "", "", {}, {}, {}, {}, {})
{
}

Reflection::CObjectType::CObjectType(
	EObjectType InObjectType, 
	const std::string InObjectName, 
	const std::string InSuperName, 
	const std::string InSuperNamespaces, 
	std::vector<CField*> InFields, 
	std::vector<CFunction*> InFunctions, 
	std::vector<std::string> InPreProcessors, 
	std::vector<const CAttribute*> InAttributes, 
	std::function<void(void*)> InDeleteLambda) :
	ObjectType(InObjectType),
	ObjectName(InObjectName),
	SuperName(InSuperName),
	SuperNamespaces(InSuperNamespaces),
	Fields(InFields),
	Functions(InFunctions),
	PreProcessors(InPreProcessors),
	Attributes(InAttributes),
	DeleteLambda(InDeleteLambda),
	bCachedCollectibleFields(false)
{
}

Reflection::CObjectType::CObjectType(CObjectType&& Other) noexcept
{
	*this = std::move(Other);
}

CObjectType& Reflection::CObjectType::operator=(CObjectType&& Other) noexcept
{
	ObjectType = std::move(Other.ObjectType);
	ObjectName = std::move(Other.ObjectName);
	SuperName = std::move(Other.SuperName);
	SuperNamespaces = std::move(Other.SuperNamespaces);
	Fields = std::move(Other.Fields);
	Functions = std::move(Other.Functions);
	PreProcessors = std::move(Other.PreProcessors);
	Attributes = std::move(Other.Attributes);
	bCachedCollectibleFields = std::move(Other.bCachedCollectibleFields);
	CachedCollectibleFields = std::move(Other.CachedCollectibleFields);
	CachedCollectibleFirstContainerFields = std::move(Other.CachedCollectibleFirstContainerFields);
	CachedCollectibleSecondContainerFields = std::move(Other.CachedCollectibleSecondContainerFields);

	Other.Fields.clear();
	Other.Functions.clear();
	Other.PreProcessors.clear();
	Other.Attributes.clear();
	Other.CachedCollectibleFields.Clear();
	Other.CachedCollectibleFirstContainerFields.Clear();
	Other.CachedCollectibleSecondContainerFields.Clear();

	return *this;
}

Reflection::CObjectType::~CObjectType()
{
	for (auto& Field : Fields)
	{
		delete Field;
		Field = nullptr;
	}
	Fields.clear();

	for (auto& Function : Functions)
	{
		delete Function;
		Function = nullptr;
	}
	Functions.clear();

	for (auto& Attribute : Attributes)
	{
		delete Attribute;
		Attribute = nullptr;
	}
	Attributes.clear();
}

std::vector<CField*> Reflection::CObjectType::GetInheritedFields() const
{
	std::vector<CField*> Vector;
	const CObjectType* Type = this;
	while (Type != nullptr)
	{
		for (CField* Field : Type->GetFields())
		{
			Vector.push_back(Field);
		}
		Type = Type->GetSuperObjectType();
	}
	return Vector;
}

std::vector<CField*> Reflection::CObjectType::GetInheritedSortingFields() const
{
	std::stack<const CObjectType*> Types;
	const CObjectType* Type = this;
	while (Type != nullptr)
	{
		Types.push(Type);
		Type = Type->GetSuperObjectType();
	}

	std::vector<CField*> Vector;
	while (!Types.empty())
	{
		Type = Types.top();
		Types.pop();
		for (CField* Field : Type->GetFields())
		{
			Vector.push_back(Field);
		}
	}
	return Vector;
}

void Reflection::CObjectType::CacheCollectibleFields()
{
	if (bCachedCollectibleFields)
	{
		return;
	}
	bCachedCollectibleFields = true;

	CachedCollectibleFields.Clear();
	CachedCollectibleFirstContainerFields.Clear();
	CachedCollectibleSecondContainerFields.Clear();

	for (auto Field : GetInheritedFields())
	{
		ECollectibleType CollectibleType = Field->IsCollectible();
		ECollectibleContainerType CollectibleContainerType = Field->IsCollectibleContainerType();
		if (CollectibleType == ECollectibleType::ClassPtr)
		{
			CachedCollectibleFields.ClassPtrFields.push_back(Field);
		}
		else if (CollectibleType == ECollectibleType::Struct)
		{
			CachedCollectibleFields.StructFields.push_back(Field);
		}
		else if (CollectibleContainerType != ECollectibleContainerType::None)
		{
			switch (CollectibleContainerType)
			{
				case ECollectibleContainerType::FirstClassPtr:
					CachedCollectibleFirstContainerFields.ClassPtrFields.push_back(Field);
					break;

				case ECollectibleContainerType::FirstStruct:
					CachedCollectibleFirstContainerFields.StructFields.push_back(Field);
					break;

				case ECollectibleContainerType::SecondClassPtr:
					CachedCollectibleSecondContainerFields.ClassPtrFields.push_back(Field);
					break;

				case ECollectibleContainerType::SecondStruct:
					CachedCollectibleSecondContainerFields.StructFields.push_back(Field);
					break;

				case ECollectibleContainerType::FirstClassPtrSecondClassPtr:
					CachedCollectibleFirstContainerFields.ClassPtrFields.push_back(Field);
					CachedCollectibleSecondContainerFields.ClassPtrFields.push_back(Field);
					break;

				case ECollectibleContainerType::FirstClassPtrSecondStruct:
					CachedCollectibleFirstContainerFields.ClassPtrFields.push_back(Field);
					CachedCollectibleSecondContainerFields.StructFields.push_back(Field);
					break;

				case ECollectibleContainerType::FirstValueSecondClassPtr:
					CachedCollectibleFirstContainerFields.StructFields.push_back(Field);
					CachedCollectibleSecondContainerFields.ClassPtrFields.push_back(Field);
					break;

				case ECollectibleContainerType::FirstValueSecondStruct:
					CachedCollectibleFirstContainerFields.StructFields.push_back(Field);
					CachedCollectibleSecondContainerFields.StructFields.push_back(Field);
					break;
			}
		}
	}
}

std::vector<CFunction*> Reflection::CObjectType::GetInheritedFunctions() const
{
	std::vector<CFunction*> Vector;
	const CObjectType* Type = this;
	while (Type != nullptr)
	{
		for (CFunction* Function : Type->GetFunctions())
		{
			Vector.push_back(Function);
		}
		Type = Type->GetSuperObjectType();
	}
	return Vector;
}

std::vector<CFunction*> Reflection::CObjectType::GetInheritedSortingFunctions() const
{
	std::stack<const CObjectType*> Types;
	const CObjectType* Type = this;
	while (Type != nullptr)
	{
		Types.push(Type);
		Type = Type->GetSuperObjectType();
	}

	std::vector<CFunction*> Vector;
	while (!Types.empty())
	{
		Type = Types.top();
		Types.pop();
		for (CFunction* Function : Type->GetFunctions())
		{
			Vector.push_back(Function);
		}
	}
	return Vector;
}

std::vector<const CAttribute*> Reflection::CObjectType::GetInheritedAttributes() const
{
	std::vector<const CAttribute*> Vector;
	// 이 타입의 객체에 부착된 어트리뷰트를 탐색합니다.
	const CObjectType* Type = this;
	while (Type != nullptr)
	{
		// 이 타입에 부착된 모든 어트리뷰트를 저장합니다.
		for (const CAttribute* Attribute : Type->GetAttributes())
		{
			Vector.push_back(Attribute);
		}
		// 이 타입의 부모 타입으로 설정해, 부모의 어트리뷰트를 탐색할 것입니다.
		Type = Type->GetSuperObjectType();
	}
	return Vector;
}

std::vector<const CAttribute*> Reflection::CObjectType::GetInheritedSortingAttributes() const
{
	std::stack<const CObjectType*> Types;
	const CObjectType* Type = this;
	while (Type != nullptr)
	{
		Types.push(Type);
		Type = Type->GetSuperObjectType();
	}

	std::vector<const CAttribute*> Vector;
	while (!Types.empty())
	{
		Type = Types.top();
		Types.pop();
		for (const CAttribute* Attribute : Type->GetAttributes())
		{
			Vector.push_back(Attribute);
		}
	}
	return Vector;
}

bool Reflection::CObjectType::HasSuperObject() const
{
	return SuperName != "";
}

CObjectType* Reflection::CObjectType::GetSuperObjectType() const
{
	return CObjectTypeDictionary::Instance()->GetObjectType(SuperName);
}

bool Reflection::CObjectType::CanUpCastingAs(const CObjectType* Other) const
{
	const CObjectType* Type = Other;
	// 이 타입의 모든 부모를 탐색합니다.
	// 부모 타입 중 하나가 Other 타입이면 참을 반환합니다.
	while (Type != nullptr)
	{
		if (Type == Other)
		{
			return true;
		}
		Type = Type->GetSuperObjectType();
	}
	return false;
}

bool Reflection::CObjectType::CanDownCastingAs(const CObjectType* Other) const
{
	// 다운캐스팅 대상을 뒤바꾸면 업캐스팅과 같은 효과입니다.
	return Other->CanUpCastingAs(this);
}

bool Reflection::CObjectType::CanCastingAs(const CObjectType* Other) const
{
	// 업캐스팅 또는 다운캐스팅이 가능하면 참을 반환합니다.
	return CanUpCastingAs(Other) || CanDownCastingAs(Other);
}

void Reflection::CObjectType::DeleteObject(void* InObject) const
{
	if (DeleteLambda == 0)
	{
		return;
	}

	if (InObject == nullptr)
	{
		return;
	}

	DeleteLambda(InObject);
}

std::string Reflection::CObjectType::GetObjectTypeString() const
{
	string ObjectTypeEnumString;
	switch (ObjectType)
	{
		default:
		case EObjectType::Class:
			ObjectTypeEnumString = "class ";
			break;
		case EObjectType::Struct:
			ObjectTypeEnumString = "struct ";
			break;
	}

	string SuperObjectTypeString;
	if (HasSuperObject())
	{
		SuperObjectTypeString = " : " + SuperName;
	}
	
	return ObjectTypeEnumString + ObjectName + SuperObjectTypeString;
}

std::string Reflection::CObjectType::ToString() const
{
	return GetObjectTypeString();
}
