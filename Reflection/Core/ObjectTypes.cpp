#include "ObjectTypes.h"
#include "Object.h"

Reflection::SFieldLambda::SFieldLambda() :
	GetLambda(0),
	SetLambda(0)
{
}

Reflection::CParameterInfo::CParameterInfo(int InIndex) :
	Index(InIndex)
{
}

Reflection::CParameterInfo::CParameterInfo(CParameterInfo&& Other) noexcept
{
	*this = std::move(Other);
}

Reflection::CParameterInfo& Reflection::CParameterInfo::operator=(CParameterInfo&& Other) noexcept
{
	Index = std::move(Other.Index);

	return *this;
}

Reflection::CFunctionInfo::CFunctionInfo(bool InbIsReadonlyFunction, void* InVoidFunctionPtr, std::vector<CParameter*> InParameters) :
	bIsReadonlyFunction(InbIsReadonlyFunction),
	VoidFunctionPtr(InVoidFunctionPtr),
	Parameters(InParameters)
{
}

Reflection::CFunctionInfo::CFunctionInfo(CFunctionInfo&& Other) noexcept
{
	*this = std::move(Other);
}

Reflection::CFunctionInfo& Reflection::CFunctionInfo::operator=(CFunctionInfo&& Other) noexcept
{
	bIsReadonlyFunction = std::move(Other.bIsReadonlyFunction);
	VoidFunctionPtr = std::move(Other.VoidFunctionPtr);
	Parameters = std::move(Other.Parameters);

	Other.VoidFunctionPtr = nullptr;
	Other.Parameters.clear();

	return *this;
}

Reflection::SFunctionLambda::SFunctionLambda() :
	CallLambda(0)
{
}

Reflection::CField::CField(CVariableInfo* InVariableInfo, CMemberInfo* InMemberInfo, SFieldLambda InFieldLambda) :
	CMember(InVariableInfo, InMemberInfo),
	FieldLambda(InFieldLambda)
{
}

Reflection::CField::CField(CField&& Other) noexcept :
	CMember(std::move(Other))
{
	FieldLambda = std::move(Other.FieldLambda);

	Other.FieldLambda = SFieldLambda();
}

Reflection::CField& Reflection::CField::operator=(CField&& Other) noexcept
{
	CMember::operator=(std::move(Other));

	FieldLambda = std::move(Other.FieldLambda);

	Other.FieldLambda = SFieldLambda();

	return *this;
}

void* Reflection::CField::GetPointedValue(CObject* InObject) const
{
	return GetPointedValue(MakeObjectWrapper(InObject));
}

bool Reflection::CField::SetPointedValue(CObject* InObject, SVoidWrapper InParameterWrapper)
{
	return SetPointedValue(MakeObjectWrapper(InObject), InParameterWrapper);
}

void* Reflection::CField::GetPointedValue(SObjectWrapper InObjectWrapper) const
{
	if (FieldLambda.GetLambda == 0)
	{
		return nullptr;
	}
	if (false == InObjectWrapper.ObjectType->CanCastingAs(GetDeclaringObjectType()))
	{
		return nullptr;
	}
	return FieldLambda.GetLambda(InObjectWrapper.Object);
}

bool Reflection::CField::SetPointedValue(SObjectWrapper InObjectWrapper, SVoidWrapper InParameterWrapper)
{
	if (FieldLambda.SetLambda == 0)
	{
		return false;
	}
	size_t ThisFieldTypeSize = GetVariableInfo()->GetType()->GetTypeInfo()->GetTypeSize();
	if (ThisFieldTypeSize != InParameterWrapper.ObjectSize)
	{
		return false;
	}
	if (false == InObjectWrapper.ObjectType->CanCastingAs(GetDeclaringObjectType()))
	{
		return false;
	}
	FieldLambda.SetLambda(InObjectWrapper.Object, InParameterWrapper.Object);
	return true;
}

Reflection::CParameter::CParameter(CVariableInfo* InVariableInfo, CParameterInfo* InParameterInfo) :
	CVariable(InVariableInfo),
	ParameterInfo(InParameterInfo)
{
}

Reflection::CParameter::CParameter(CParameter&& Other) noexcept :
	CVariable(std::move(Other))
{
	ParameterInfo = std::move(Other.ParameterInfo);

	Other.ParameterInfo = nullptr;
}

Reflection::CParameter& Reflection::CParameter::operator=(CParameter&& Other) noexcept
{
	CVariable::operator=(std::move(Other));

	ParameterInfo = std::move(Other.ParameterInfo);

	Other.ParameterInfo = nullptr;

	return *this;
}

Reflection::CFunction::CFunction(CVariableInfo* InVariableInfo, CMemberInfo* InMemberInfo, CFunctionInfo* InFunctionInfo, SFunctionLambda InFunctionLambda) :
	CMember(InVariableInfo, InMemberInfo),
	FunctionInfo(InFunctionInfo),
	FunctionLambda(InFunctionLambda)
{
}

Reflection::CFunction::CFunction(CFunction&& Other) noexcept :
	CMember(std::move(Other))
{
	FunctionInfo = std::move(Other.FunctionInfo);
	FunctionLambda = std::move(Other.FunctionLambda);

	Other.FunctionInfo = nullptr;
	Other.FunctionLambda = SFunctionLambda();
}

Reflection::CFunction& Reflection::CFunction::operator=(CFunction&& Other) noexcept
{
	CMember::operator=(std::move(Other));

	FunctionInfo = std::move(Other.FunctionInfo);
	FunctionLambda = std::move(Other.FunctionLambda);

	Other.FunctionInfo = nullptr;
	Other.FunctionLambda = SFunctionLambda();

	return *this;
}

bool Reflection::CFunction::Invoke(CObject* InObject) 
{
	SVoidWrapper TempReturn(nullptr, 0);

	va_list VAList = 0;
	bool Succeeded = Internal_Invoke(TempReturn, MakeObjectWrapper(InObject), 0, VAList);

	return Succeeded;
}

bool Reflection::CFunction::Invoke(CObject* InObject, size_t InArgumentsCount, SVoidWrapper InArgumentPointers ...)
{
	SVoidWrapper TempReturn(nullptr, 0);

	va_list VAList;
	va_start(VAList, InArgumentsCount);
	bool Succeeded = Internal_Invoke(TempReturn, MakeObjectWrapper(InObject), InArgumentsCount, VAList);
	va_end(VAList);

	return Succeeded;
}

bool Reflection::CFunction::Invoke(SObjectWrapper InObjectWrapper) 
{
	SVoidWrapper TempReturn(nullptr, 0);

	va_list VAList = 0;
	bool Succeeded = Internal_Invoke(TempReturn, InObjectWrapper, 0, VAList);

	return Succeeded;
}

bool Reflection::CFunction::Invoke(SObjectWrapper InObjectWrapper, size_t InArgumentsCount, SVoidWrapper InArgumentPointers ...)
{
	SVoidWrapper TempReturn(nullptr, 0);

	va_list VAList;
	va_start(VAList, InArgumentsCount);
	bool Succeeded = Internal_Invoke(TempReturn, InObjectWrapper, InArgumentsCount, VAList);
	va_end(VAList);

	return Succeeded;
}

bool Reflection::CFunction::Invoke(SVoidWrapper OutReturn, CObject* InObject) 
{
	va_list VAList = 0;
	bool Succeeded = Internal_Invoke(OutReturn, MakeObjectWrapper(InObject), 0, VAList);

	return Succeeded;
}

bool Reflection::CFunction::Invoke(SVoidWrapper OutReturn, CObject* InObject, size_t InArgumentsCount, SVoidWrapper InArgumentPointers...)
{
	va_list VAList;
	va_start(VAList, InArgumentsCount);
	bool Succeeded = Internal_Invoke(OutReturn, MakeObjectWrapper(InObject), InArgumentsCount, VAList);
	va_end(VAList);

	return Succeeded;
}

bool Reflection::CFunction::Invoke(SVoidWrapper OutReturn, SObjectWrapper InObjectWrapper) 
{
	va_list VAList = 0;
	bool Succeeded = Internal_Invoke(OutReturn, InObjectWrapper, 0, VAList);

	return Succeeded;
}

bool Reflection::CFunction::Invoke(SVoidWrapper OutReturn, SObjectWrapper InObjectWrapper, size_t InArgumentsCount, SVoidWrapper InArgumentPointers...)
{
	va_list VAList;
	va_start(VAList, InArgumentsCount);
	bool Succeeded = Internal_Invoke(OutReturn, InObjectWrapper, InArgumentsCount, VAList);
	va_end(VAList);

	return Succeeded;
}

bool Reflection::CFunction::Internal_Invoke(SVoidWrapper OutReturn, SObjectWrapper InObjectWrapper, size_t InArgumentsCount, std::va_list InArgumentPointers)
{
	const std::vector<CParameter*> Parameters = FunctionInfo->GetParameters();

	if (FunctionLambda.CallLambda == 0)
	{
		return false;
	}

	if (InArgumentsCount != Parameters.size())
	{
		return false;
	}

	if (OutReturn.ObjectSize != GetTypeSize())
	{
		return false;
	}

	if (!GetDeclaringObjectType()->CanCastingAs(InObjectWrapper.ObjectType))
	{
		return false;
	}

	std::vector<SVoidWrapper> Arguments;
	for (size_t i = 0; i < InArgumentsCount; ++i)
	{
		SVoidWrapper Argument = va_arg(InArgumentPointers, SVoidWrapper);
		if (Argument.ObjectSize != Parameters[i]->GetTypeSize())
		{
			return false;
		}
		Arguments.push_back(Argument);
	}
	if (Arguments.size() != Parameters.size())
	{
		return false;
	}
	FunctionLambda.CallLambda(OutReturn, InObjectWrapper.Object, Arguments);
	return true;
}

std::string Reflection::CFunction::ToString() const
{
	std::vector<CParameter*> Parameters = FunctionInfo->GetParameters();
	std::string ParameterString;
	for (size_t i = 0; i < Parameters.size(); ++i)
	{
		ParameterString += Parameters[i]->ToString();
		if (i < Parameters.size() - 1)
		{
			ParameterString += ", ";
		}
	}
	std::string ReadonlyFunctionString;
	if (FunctionInfo->IsReadonlyFunction())
	{
		ReadonlyFunctionString = " const";
	}
	return GetMemberString() + "(" + ParameterString + ")" + ReadonlyFunctionString;
}
