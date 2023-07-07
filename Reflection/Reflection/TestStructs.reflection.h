#pragma once

#ifdef ___FILE_NAME___ 
#undef ___FILE_NAME___ 
#endif 
#define ___FILE_NAME___ TestStructs_h

#ifdef ___TEMPLATE_TYPE_0___ 
#undef ___TEMPLATE_TYPE_0___ 
#endif 
#define ___TEMPLATE_TYPE_0___ Reflection::TList<CNode*>


#ifdef ___REFLECTION_TestStructs_h_11___
#undef ___REFLECTION_TestStructs_h_11___
#endif
#define ___REFLECTION_TestStructs_h_11___ \
private: \
DECLARE_STATIC_CONSTRUCTOR() \
public: \
DELETE_STRUCT_NEW_DELETE_OPERATORS() \
DECLARE_THIS_TYPE(STestStruct) \
DECLARE_STRUCT_OBJECT_TYPE() \
START_DECLARE_STATIC_OBJECT_TYPE() \
START_PUSH_FIELD() \
PUSH_TEMPLATE_PARAMETER(false, EObjectType::None, , CNode, 1, false, CNode*, sizeof(CNode*), 0, false) \
SETUP_FIELD_GET_LAMBDA(STestStruct, ___TEMPLATE_TYPE_0___, Nodes) \
SETUP_FIELD_SET_LAMBDA(STestStruct, ___TEMPLATE_TYPE_0___, Nodes) \
END_PUSH_FIELD(false, EObjectType::None, Reflection::, TList, 0, false, ___TEMPLATE_TYPE_0___, sizeof(___TEMPLATE_TYPE_0___), EAccessType::Public, STestStruct, Nodes) \
END_DECLARE_STATIC_OBJECT_TYPE(EObjectType::Struct, STestStruct, , ) \
public:

