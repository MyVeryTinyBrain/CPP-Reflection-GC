#pragma once

#define OBJECT(...)

#define FIELD(...)

#define FUNCTION(...)

#define REFLECTION_COMBINE_INTERNAL(S0,S1,S2,S3,S4,S5) S0##S1##S2##S3##S4##S5

#define REFLECTION_COMBINE(S0,S1,S2,S3,S4,S5) REFLECTION_COMBINE_INTERNAL(S0,S1,S2,S3,S4,S5)

#define REFLECTION(...) REFLECTION_COMBINE(___,REFLECTION_,___FILE_NAME___,_,__LINE__,___)

#define TEMPLATE_PARAMETERS_NAME TemplateParameters

#define VARIABLE_INFO_NAME VariableInfo

#define PRE_PROCESSORS_NAME PreProcessors

#define ATTRIBUTES_NAME Attributes

#define PARAMETERS_NAME Parameters

#define FIELDS_NAME Fields

#define FUNCTIONS_NAME Functions

#define DELETE_LABMDA_NAME DeleteLambda

#define FIELD_LAMBDA_NAME FieldLambda

#define GET_LAMBDA_NAME GetLambda

#define SET_LAMBDA_NAME SetLambda

#define FUNCTION_LAMBDA_NAME FunctionLambda

#define CALL_LAMBDA_NAME CallLambda

// 이 매크로가 작성된 타입을 표현합니다.
#define DECLARE_THIS_TYPE(TYPE_NAME) typedef TYPE_NAME This;

// 이 매크로가 작성된 타입의 부모 타입을 표현합니다.
#define DECLARE_SUPER_TYPE(TYPE_NAME) typedef TYPE_NAME Super;

#define DECLARE_OBJECT_TYPE(VIRTUAL, OVERRIDE) VIRTUAL Reflection::CObjectType* ObjectType() const OVERRIDE { return StaicObjectType(); }

#define DECLARE_CLASS_OBJECT_TYPE() DECLARE_OBJECT_TYPE(virtual, override)

// 이 매크로가 작성된 타입의 정보를 반환하는 멤버 함수를 표현합니다.
#define DECLARE_STRUCT_OBJECT_TYPE() DECLARE_OBJECT_TYPE(,)

// 이 매크로가 작성된 타입이 프로그램 실행 시 정적 함수를 호출하도록 합니다.
#define DECLARE_STATIC_CONSTRUCTOR() \
struct SStaticConstructor \
{ \
	SStaticConstructor() \
	{ \
        using namespace Reflection; \
		CObjectTypeDictionary::Instance()->AddObjectType(StaicObjectType()); \
	} \
}; \
static SStaticConstructor StaticConstructor;

#define PUSH_PRE_PROCESSOR(CONTEXT) PRE_PROCESSORS_NAME.push_back(#CONTEXT);

#define PUSH_ATTRIBUTE(CONTEXT) ATTRIBUTES_NAME.push_back(new CONTEXT);

#define SETUP_DELETE_LAMBDA(OBJECT_TYPE_NAME) \
DELETE_LABMDA_NAME = [](void* InObject) \
{ \
    typedef OBJECT_TYPE_NAME ObjectType; \
    ObjectType* Ptr = (ObjectType*)InObject; \
    delete Ptr; \
};

#define DELETE_STRUCT_NEW_DELETE_OPERATORS() \
void* operator new (size_t InSize) = delete; \
void * operator new[] (size_t InSize) = delete; \
void operator delete (void* InAddress) = delete; \
void operator delete[] (void * InAddress) = delete;

// 타입 정보를 표현하는 객체를 반환하기 위한 함수 작성을 시작합니다.
// 이 매크로 하단에 추가적인 내용을 포함해 타입 정보를 완성합니다.
// 모든 내용을 추가한 이후, END_DECLARE_STATIC_OBJECT_TYPE를 추가해야 합니다.
#define START_DECLARE_STATIC_OBJECT_TYPE() \
static Reflection::CObjectType* StaicObjectType() \
{ \
    using namespace Reflection; \
	static bool Generated = false; \
	static CObjectType ObjectType; \
	if (Generated == true) \
	{ \
		return &ObjectType; \
	} \
	std::vector<CField*> FIELDS_NAME; \
	std::vector<CFunction*> FUNCTIONS_NAME; \
    std::vector<std::string> PRE_PROCESSORS_NAME; \
	std::vector<const CAttribute*> ATTRIBUTES_NAME; \
    std::function<void(void*)> DELETE_LABMDA_NAME = 0;

// 타입 정보를 표현하는 객체를 반환하기 위한 함수 작성을 끝냅니다.
#define END_DECLARE_STATIC_OBJECT_TYPE(OBJECT_TYPE, TYPE_NAME, SUPER_NAME, SUPER_NAMESPACES) \
	ObjectType = std::move(CObjectType( \
		OBJECT_TYPE, \
		#TYPE_NAME, \
		#SUPER_NAME, \
        #SUPER_NAMESPACES, \
		FIELDS_NAME, \
		FUNCTIONS_NAME, \
        PRE_PROCESSORS_NAME, \
		ATTRIBUTES_NAME, \
        DELETE_LABMDA_NAME \
	)); \
	Generated = true; \
	return &ObjectType; \
}

#define PUSH_TEMPLATE_PARAMETER(IS_CONST, FORWARD, NAMESPACES, TYPE_NAME, NUM_POINTERS, IS_REFERENCE, FULL_TYPE_NAME, SIZE, INDEX, IS_UNKNOWN_TYPE) \
{ \
    CNativeTypeInfo* TemplateParameterNativeTypeInfo = new CNativeTypeInfo(IS_CONST, FORWARD, #NAMESPACES, #TYPE_NAME, NUM_POINTERS, IS_REFERENCE, SIZE); \
    CTemplateParameter* TemplateParameter = new CTemplateParameter(TemplateParameterNativeTypeInfo, INDEX, IS_UNKNOWN_TYPE); \
    TEMPLATE_PARAMETERS_NAME.push_back(TemplateParameter); \
}

#define SETUP_FIELD_GET_LAMBDA(DECLARING_OBJECT_TYPE, FULL_TYPE_NAME, FIELD_NAME) \
FIELD_LAMBDA_NAME.GET_LAMBDA_NAME = [](void* InObject) -> void* \
{ \
    typedef DECLARING_OBJECT_TYPE ObjectType; \
    ObjectType* Object = (ObjectType*)(InObject); \
    void* Ptr = (void*)&(Object->FIELD_NAME); \
    return Ptr; \
};

#define SETUP_FIELD_SET_LAMBDA(DECLARING_OBJECT_TYPE, FULL_TYPE_NAME, FIELD_NAME) \
FIELD_LAMBDA_NAME.SET_LAMBDA_NAME = [](void* InObject, void* InValue) -> void \
{ \
    typedef DECLARING_OBJECT_TYPE ObjectType; \
    typedef FULL_TYPE_NAME ValueType; \
    ObjectType* Object = (ObjectType*)(InObject); \
    ValueType* Ptr = (ValueType*)InValue; \
    Object->FIELD_NAME = (*Ptr); \
};

#define START_PUSH_FIELD() \
{ \
    std::vector<CTemplateParameter*> TEMPLATE_PARAMETERS_NAME; \
    std::vector<std::string> PRE_PROCESSORS_NAME; \
    std::vector<const CAttribute*> ATTRIBUTES_NAME; \
    SFieldLambda FIELD_LAMBDA_NAME;

#define END_PUSH_FIELD(IS_CONST, FORWARD, NAMESPACES, TYPE_NAME, NUM_POINTERS, IS_REFERENCE, FULL_TYPE_NAME, SIZE, ACCESS_TYPE, DECLARING_OBJECT_TYPE, FIELD_NAME) \
    CNativeTypeInfo* FieldNativeTypeInfo = new CNativeTypeInfo(IS_CONST, FORWARD, #NAMESPACES, #TYPE_NAME, NUM_POINTERS, IS_REFERENCE, SIZE); \
    CType* FieldType = new CType(FieldNativeTypeInfo, TEMPLATE_PARAMETERS_NAME); \
    CVariableInfo* FieldVariableInfo = new CVariableInfo(FieldType, #FIELD_NAME); \
    CMemberInfo* FieldMemberInfo = new CMemberInfo(PRE_PROCESSORS_NAME, ATTRIBUTES_NAME, ACCESS_TYPE, #DECLARING_OBJECT_TYPE); \
    CField* Field = new CField(FieldVariableInfo, FieldMemberInfo, FIELD_LAMBDA_NAME); \
    FIELDS_NAME.push_back(Field); \
}

#define START_PUSH_PARAMETER() \
{ \
    std::vector<CTemplateParameter*> TEMPLATE_PARAMETERS_NAME;

#define END_PUSH_PARAMETER(IS_CONST, FORWARD, NAMESPACES, TYPE_NAME, NUM_POINTERS, IS_REFERENCE, FULL_TYPE_NAME, SIZE, PARAMETER_NAME, INDEX) \
    CNativeTypeInfo* ParameterNativeTypeInfo = new CNativeTypeInfo(IS_CONST, FORWARD, #NAMESPACES, #TYPE_NAME, NUM_POINTERS, IS_REFERENCE, SIZE); \
    CType* ParameterType = new CType(ParameterNativeTypeInfo, TEMPLATE_PARAMETERS_NAME); \
    CVariableInfo* ParameterVariableInfo = new CVariableInfo(ParameterType, #PARAMETER_NAME); \
    CParameterInfo* ParameterInfo = new CParameterInfo(INDEX); \
    CParameter* Parameter = new CParameter(ParameterVariableInfo, ParameterInfo); \
    PARAMETERS_NAME.push_back(Parameter); \
}

namespace Reflection
{
    struct SFunctionConverter
    {
        template<class TFunction>
        static void* Convert(TFunction InFunction)
        {
            return (void*&)InFunction;
        }
    };
}

#define PLACE_FUNCTION_CALL_LAMBDA_PARAMETER(FULL_PARAMETER_TYPE_WITHOUT_REF_NAME, INDEX) *((FULL_PARAMETER_TYPE_WITHOUT_REF_NAME*)InArguments[INDEX].Object)

#define START_SETUP_FUNCTION_CALL_RETURN_LAMBDA(FULL_RETURN_TYPE_WITHOUT_CONST_AND_REF_NAME, FUNCTION_NAME, DECLARING_OBJECT_TYPE) \
FUNCTION_LAMBDA_NAME.CALL_LAMBDA_NAME = [](SVoidWrapper OutReturn, void* InObject, std::vector<SVoidWrapper> InArguments) \
{ \
    typedef FULL_RETURN_TYPE_WITHOUT_CONST_AND_REF_NAME ReturnType; \
    typedef DECLARING_OBJECT_TYPE ObjectType; \
    ObjectType* Object = (ObjectType*)(InObject); \
    if(Object == nullptr) \
    { \
        return; \
    } \
    ReturnType* ReturnPtr = (ReturnType*)OutReturn.Object; \
    *ReturnPtr = Object->FUNCTION_NAME \
    ( 

#define END_SETUP_FUNCTION_CALL_RETURN_LAMBDA() \
    ); \
};

#define START_SETUP_FUNCTION_CALL_NORETURN_LAMBDA(FULL_RETURN_TYPE_WITHOUT_CONST_AND_REF_NAME, FUNCTION_NAME, DECLARING_OBJECT_TYPE) \
FUNCTION_LAMBDA_NAME.CALL_LAMBDA_NAME = [](SVoidWrapper OutReturn, void* InObject, std::vector<SVoidWrapper> InArguments) \
{ \
    typedef DECLARING_OBJECT_TYPE ObjectType; \
    ObjectType* Object = (ObjectType*)(InObject); \
    if(Object == nullptr) \
    { \
        return; \
    } \
    Object->FUNCTION_NAME \
    (

#define END_SETUP_FUNCTION_CALL_NORETURN_LAMBDA() \
    ); \
};

#define START_PUSH_FUNCTION() \
{ \
    std::vector<CTemplateParameter*> TEMPLATE_PARAMETERS_NAME; \
    std::vector<std::string> PRE_PROCESSORS_NAME; \
    std::vector<const CAttribute*> ATTRIBUTES_NAME; \
    std::vector<CParameter*> PARAMETERS_NAME; \
    SFunctionLambda FUNCTION_LAMBDA_NAME;

#define END_PUSH_FUNCTION(IS_CONST, FORWARD, NAMESPACES, TYPE_NAME, NUM_POINTERS, IS_REFERENCE, FULL_TYPE_NAME, SIZE, ACCESS_TYPE, DECLARING_OBJECT_TYPE, FUNCTION_NAME, IS_READONLY_FUNCTION) \
    CNativeTypeInfo* FunctionNativeTypeInfo = new CNativeTypeInfo(IS_CONST, FORWARD, #NAMESPACES, #TYPE_NAME, NUM_POINTERS, IS_REFERENCE, SIZE); \
    CType* FunctionType = new CType(FunctionNativeTypeInfo, TEMPLATE_PARAMETERS_NAME); \
    CVariableInfo* FunctionVariableInfo = new CVariableInfo(FunctionType, #FUNCTION_NAME); \
    CMemberInfo* FunctionMemberInfo = new CMemberInfo(PRE_PROCESSORS_NAME, ATTRIBUTES_NAME, ACCESS_TYPE, #DECLARING_OBJECT_TYPE); \
    CFunctionInfo* FunctionInfo = new CFunctionInfo(IS_READONLY_FUNCTION, SFunctionConverter::Convert(&DECLARING_OBJECT_TYPE::FUNCTION_NAME), PARAMETERS_NAME); \
    CFunction* Function = new CFunction(FunctionVariableInfo, FunctionMemberInfo, FunctionInfo, FUNCTION_LAMBDA_NAME); \
    FUNCTIONS_NAME.push_back(Function); \
}
