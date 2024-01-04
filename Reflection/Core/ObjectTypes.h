#pragma once

#include "NativeTypes.h"
#include "Wrappers.h"
#include <functional>
#include <cstdarg>

namespace Reflection
{
	class CParameter;
	class CObject;
	class CObjectType;

	struct SFieldLambda
	{
		std::function<void* (void*)> GetLambda;
		std::function<void(void*, void*)> SetLambda;

		SFieldLambda();
	};

	class CParameterInfo
	{
		int Index;

	public:
		CParameterInfo(int InIndex);
		CParameterInfo(CParameterInfo&& Other) noexcept;

		CParameterInfo& operator = (CParameterInfo&& Other) noexcept;

		int GetIndex() const { return Index; }
	};

	class CFunctionInfo
	{
		bool bIsReadonlyFunction;
		void* VoidFunctionPtr;
		std::vector<CParameter*> Parameters;

	public:
		CFunctionInfo(bool InbIsReadonlyFunction, void* InVoidFunctionPtr, std::vector<CParameter*> InParameters);
		CFunctionInfo(CFunctionInfo&& Other) noexcept;

		CFunctionInfo& operator = (CFunctionInfo&& Other) noexcept;

		bool IsReadonlyFunction() const { return bIsReadonlyFunction; }

		void* GetVoidFunctionPtr() const { return VoidFunctionPtr; }

		const std::vector<CParameter*>& GetParameters() const { return Parameters; }
	};

	struct SFunctionLambda
	{
		std::function<void(SVoidWrapper, void*, std::vector<SVoidWrapper>)> CallLambda;

		SFunctionLambda();
	};

	class CField : public CMember
	{
		SFieldLambda FieldLambda;

	public:
		CField(CVariableInfo* InVariableInfo, CMemberInfo* InMemberInfo, SFieldLambda InFieldLambda);
		CField(CField&& Other) noexcept;

		CField& operator = (CField&& Other) noexcept;

		void* GetPointedValue(CObject* InObject) const;
		bool SetPointedValue(CObject* InObject, SVoidWrapper InParameterWrapper);
		void* GetPointedValue(SObjectWrapper InObjectWrapper) const;
		bool SetPointedValue(SObjectWrapper InObjectWrapper, SVoidWrapper InParameterWrapper);
	};

	class CParameter : public CVariable
	{
		CParameterInfo* ParameterInfo;

	public:
		CParameter(CVariableInfo* InVariableInfo, CParameterInfo* InParameterInfo);
		CParameter(CParameter&& Other) noexcept;

		CParameter& operator = (CParameter&& Other) noexcept;

		const CParameterInfo* GetParameterInfo() const { return ParameterInfo; }
	};

	class CFunction : public CMember
	{
		CFunctionInfo* FunctionInfo;
		SFunctionLambda FunctionLambda;

	public:
		CFunction(CVariableInfo* InVariableInfo, CMemberInfo* InMemberInfo, CFunctionInfo* InFunctionInfo, SFunctionLambda InFunctionLambda);
		CFunction(CFunction&& Other) noexcept;

		CFunction& operator = (CFunction&& Other) noexcept;

		const CFunctionInfo* GetFunctionInfo() const { return FunctionInfo; }

		// SObjectWrapper InObjectWrapper: Struct Object
		// SVoidWrapper OutReturn: Return value pointer wrapper

		bool Invoke(CObject* InObject);
		bool Invoke(CObject* InObject, size_t InArgumentsCount, SVoidWrapper InArgumentPointers...);		
		bool Invoke(SObjectWrapper InObjectWrapper);
		bool Invoke(SObjectWrapper InObjectWrapper, size_t InArgumentsCount, SVoidWrapper InArgumentPointers...);
		bool Invoke(SVoidWrapper OutReturn, CObject* InObject);
		bool Invoke(SVoidWrapper OutReturn, CObject* InObject, size_t InArgumentsCount, SVoidWrapper InArgumentPointers...);
		bool Invoke(SVoidWrapper OutReturn, SObjectWrapper InObjectWrapper);
		bool Invoke(SVoidWrapper OutReturn, SObjectWrapper InObjectWrapper, size_t InArgumentsCount, SVoidWrapper InArgumentPointers...);

	private:
		bool Internal_Invoke(SVoidWrapper OutReturn, SObjectWrapper InObjectWrapper, size_t InArgumentsCount, std::va_list InArgumentPointers);

	public:

		virtual std::string ToString() const override;
	};
}