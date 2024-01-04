#pragma once

#ifdef ___FILE_NAME___ 
#undef ___FILE_NAME___ 
#endif 
#define ___FILE_NAME___ TestClasses_h

#ifdef ___TEMPLATE_TYPE_0___ 
#undef ___TEMPLATE_TYPE_0___ 
#endif 
#define ___TEMPLATE_TYPE_0___ Reflection::TList<CNode*>

#ifdef ___TEMPLATE_TYPE_1___ 
#undef ___TEMPLATE_TYPE_1___ 
#endif 
#define ___TEMPLATE_TYPE_1___ Reflection::TSet<CNode*>

#ifdef ___TEMPLATE_TYPE_2___ 
#undef ___TEMPLATE_TYPE_2___ 
#endif 
#define ___TEMPLATE_TYPE_2___ Reflection::TMap<int, CNode*>


#ifdef ___REFLECTION_TestClasses_h_21___
#undef ___REFLECTION_TestClasses_h_21___
#endif
#define ___REFLECTION_TestClasses_h_21___ \
private: \
DECLARE_STATIC_CONSTRUCTOR() \
public: \
DECLARE_THIS_TYPE(CBaseObject) \
DECLARE_SUPER_TYPE(CObject) \
DECLARE_CLASS_OBJECT_TYPE() \
START_DECLARE_STATIC_OBJECT_TYPE() \
START_PUSH_FIELD() \
SETUP_FIELD_GET_LAMBDA(CBaseObject, int, BaseObjectTestVariable) \
SETUP_FIELD_SET_LAMBDA(CBaseObject, int, BaseObjectTestVariable) \
END_PUSH_FIELD(false, EObjectType::None, , int, 0, false, int, sizeof(int), EAccessType::Public, CBaseObject, BaseObjectTestVariable) \
PUSH_ATTRIBUTE(CTestAttribute("This_Is_Base_Object")) \
SETUP_DELETE_LAMBDA(CBaseObject) \
END_DECLARE_STATIC_OBJECT_TYPE(EObjectType::Class, CBaseObject, CObject, Reflection::) \
private:

#ifdef ___REFLECTION_TestClasses_h_37___
#undef ___REFLECTION_TestClasses_h_37___
#endif
#define ___REFLECTION_TestClasses_h_37___ \
private: \
DECLARE_STATIC_CONSTRUCTOR() \
public: \
DECLARE_THIS_TYPE(CRootObject) \
DECLARE_SUPER_TYPE(CBaseObject) \
DECLARE_CLASS_OBJECT_TYPE() \
START_DECLARE_STATIC_OBJECT_TYPE() \
START_PUSH_FIELD() \
SETUP_FIELD_GET_LAMBDA(CRootObject, CNode*, NodeA) \
SETUP_FIELD_SET_LAMBDA(CRootObject, CNode*, NodeA) \
END_PUSH_FIELD(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), EAccessType::Public, CRootObject, NodeA) \
START_PUSH_FIELD() \
SETUP_FIELD_GET_LAMBDA(CRootObject, CNode*, NodeB) \
SETUP_FIELD_SET_LAMBDA(CRootObject, CNode*, NodeB) \
END_PUSH_FIELD(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), EAccessType::Public, CRootObject, NodeB) \
START_PUSH_FIELD() \
SETUP_FIELD_GET_LAMBDA(CRootObject, CNode*, NodeC) \
SETUP_FIELD_SET_LAMBDA(CRootObject, CNode*, NodeC) \
END_PUSH_FIELD(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), EAccessType::Public, CRootObject, NodeC) \
START_PUSH_FIELD() \
PUSH_TEMPLATE_PARAMETER(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), 0, false) \
SETUP_FIELD_GET_LAMBDA(CRootObject, ___TEMPLATE_TYPE_0___, Nodes) \
SETUP_FIELD_SET_LAMBDA(CRootObject, ___TEMPLATE_TYPE_0___, Nodes) \
END_PUSH_FIELD(false, EObjectType::None, Reflection::, TList, 0, false, ___TEMPLATE_TYPE_0___, sizeof(___TEMPLATE_TYPE_0___), EAccessType::Public, CRootObject, Nodes) \
START_PUSH_FIELD() \
PUSH_TEMPLATE_PARAMETER(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), 0, false) \
SETUP_FIELD_GET_LAMBDA(CRootObject, ___TEMPLATE_TYPE_1___, NodeSet) \
SETUP_FIELD_SET_LAMBDA(CRootObject, ___TEMPLATE_TYPE_1___, NodeSet) \
END_PUSH_FIELD(false, EObjectType::None, Reflection::, TSet, 0, false, ___TEMPLATE_TYPE_1___, sizeof(___TEMPLATE_TYPE_1___), EAccessType::Public, CRootObject, NodeSet) \
START_PUSH_FIELD() \
PUSH_TEMPLATE_PARAMETER(false, EObjectType::None, , int, 0, false, int, sizeof(int), 0, false) \
PUSH_TEMPLATE_PARAMETER(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), 1, false) \
SETUP_FIELD_GET_LAMBDA(CRootObject, ___TEMPLATE_TYPE_2___, NodeMap) \
SETUP_FIELD_SET_LAMBDA(CRootObject, ___TEMPLATE_TYPE_2___, NodeMap) \
END_PUSH_FIELD(false, EObjectType::None, Reflection::, TMap, 0, false, ___TEMPLATE_TYPE_2___, sizeof(___TEMPLATE_TYPE_2___), EAccessType::Public, CRootObject, NodeMap) \
START_PUSH_FIELD() \
SETUP_FIELD_GET_LAMBDA(CRootObject, STestStruct, TestStruct) \
SETUP_FIELD_SET_LAMBDA(CRootObject, STestStruct, TestStruct) \
END_PUSH_FIELD(false, EObjectType::None, , STestStruct, 0, false, STestStruct, sizeof(STestStruct), EAccessType::Public, CRootObject, TestStruct) \
START_PUSH_FUNCTION() \
START_SETUP_FUNCTION_CALL_RETURN_LAMBDA(CNode*, GetNodeA, CRootObject) \
END_SETUP_FUNCTION_CALL_RETURN_LAMBDA() \
END_PUSH_FUNCTION(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), EAccessType::Public, CRootObject, GetNodeA, true) \
START_PUSH_FUNCTION() \
START_PUSH_PARAMETER() \
END_PUSH_PARAMETER(false, EObjectType::None, , int, 0, false, int, sizeof(int), A, 0) \
START_PUSH_PARAMETER() \
END_PUSH_PARAMETER(false, EObjectType::None, , int, 0, false, int, sizeof(int), B, 1) \
START_SETUP_FUNCTION_CALL_RETURN_LAMBDA(int, AddReturn, CRootObject) \
PLACE_FUNCTION_CALL_LAMBDA_PARAMETER(int, 0), \
PLACE_FUNCTION_CALL_LAMBDA_PARAMETER(int, 1) \
END_SETUP_FUNCTION_CALL_RETURN_LAMBDA() \
END_PUSH_FUNCTION(false, EObjectType::None, , int, 0, false, int, sizeof(int), EAccessType::Public, CRootObject, AddReturn, true) \
START_PUSH_FUNCTION() \
START_PUSH_PARAMETER() \
END_PUSH_PARAMETER(false, EObjectType::None, , float, 0, true, float&, sizeof(float&), A, 0) \
START_SETUP_FUNCTION_CALL_NORETURN_LAMBDA(void, SqrtReference, CRootObject) \
PLACE_FUNCTION_CALL_LAMBDA_PARAMETER(float, 0) \
END_SETUP_FUNCTION_CALL_NORETURN_LAMBDA() \
END_PUSH_FUNCTION(false, EObjectType::None, , void, 0, false, void, 0, EAccessType::Public, CRootObject, SqrtReference, false) \
START_PUSH_FUNCTION() \
START_SETUP_FUNCTION_CALL_NORETURN_LAMBDA(void, TestFunction, CRootObject) \
END_SETUP_FUNCTION_CALL_NORETURN_LAMBDA() \
END_PUSH_FUNCTION(false, EObjectType::None, , void, 0, false, void, 0, EAccessType::Public, CRootObject, TestFunction, true) \
SETUP_DELETE_LAMBDA(CRootObject) \
END_DECLARE_STATIC_OBJECT_TYPE(EObjectType::Class, CRootObject, CBaseObject, ) \
private:

#ifdef ___REFLECTION_TestClasses_h_79___
#undef ___REFLECTION_TestClasses_h_79___
#endif
#define ___REFLECTION_TestClasses_h_79___ \
private: \
DECLARE_STATIC_CONSTRUCTOR() \
public: \
DECLARE_THIS_TYPE(CNode) \
DECLARE_SUPER_TYPE(CBaseObject) \
DECLARE_CLASS_OBJECT_TYPE() \
START_DECLARE_STATIC_OBJECT_TYPE() \
START_PUSH_FIELD() \
SETUP_FIELD_GET_LAMBDA(CNode, CNode*, Next) \
SETUP_FIELD_SET_LAMBDA(CNode, CNode*, Next) \
END_PUSH_FIELD(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), EAccessType::Public, CNode, Next) \
SETUP_DELETE_LAMBDA(CNode) \
END_DECLARE_STATIC_OBJECT_TYPE(EObjectType::Class, CNode, CBaseObject, ) \
private:

