#include "Object.h"
#include "GarbageCollector.h"

Reflection::CObject::SStaticConstructor Reflection::CObject::StaticConstructor;

Reflection::CObject::CObject()
{
}

void* Reflection::CObject::operator new(size_t InSize)
{
    void* MemoryBlock = new char[InSize];
    CGarbageCollector::Instance()->RegistObject((CObject*)MemoryBlock);
    return MemoryBlock;
}

Reflection::CObject::~CObject()
{
}

std::string Reflection::CObject::ToString() const
{
    return ObjectType()->ToString();
}
