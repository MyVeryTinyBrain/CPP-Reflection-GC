#pragma once

#ifdef ___FILE_NAME___ 
#undef ___FILE_NAME___ 
#endif 
#define ___FILE_NAME___ Test_h

#ifdef ___REFLECTION_Test_h_9___
#undef ___REFLECTION_Test_h_9___
#endif
#define ___REFLECTION_Test_h_9___ \
private: \
DECLARE_STATIC_CONSTRUCTOR() \
public: \
DECLARE_THIS_TYPE(CTest) \
DECLARE_SUPER_TYPE(CObject) \
DECLARE_CLASS_OBJECT_TYPE() \
START_DECLARE_STATIC_OBJECT_TYPE() \
SETUP_DELETE_LAMBDA(CTest) \
END_DECLARE_STATIC_OBJECT_TYPE(EObjectType::Class, CTest, CObject, Reflection::) \
private:

