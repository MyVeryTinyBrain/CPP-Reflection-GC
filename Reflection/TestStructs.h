#pragma once

#include "Core/Reflection.h"
#include "Reflection/TestStructs.reflection.h"

class CNode; 

OBJECT()
struct STestStruct
{
	REFLECTION()

	FIELD()
	Reflection::TList<CNode*> Nodes;
};

