#pragma once

#include "TestStructs.h"
#include "Core/Reflection.h"
#include <math.h>
#include "Reflection/TestClasses.reflection.h"

class CNode; 

OBJECT()
class CBaseObject : public Reflection::CObject
{
	REFLECTION()

public:
	static int NumObjects;

	CBaseObject();

	virtual void GCDestructor() override;
};

OBJECT()
class CRootObject : public CBaseObject
{
	REFLECTION()

public:
	CRootObject();

	FIELD()
	CNode* NodeA = nullptr;

	FIELD()
	CNode* NodeB = nullptr;

	FIELD()
	CNode* NodeC = nullptr;

	FIELD()
	Reflection::TList<CNode*> Nodes;

	FIELD()
	Reflection::TSet<CNode*> NodeSet;

	FIELD()
	Reflection::TMap<int, CNode*> NodeMap;

	FIELD()
	STestStruct TestStruct;

	FUNCTION()
	CNode* GetNodeA() const { return NodeA; }

	FUNCTION()
	int AddReturn(int A, int B) const { return A + B; }

	FUNCTION()
	void SqrtReference(float& A) const { A = sqrt(A); }
};

OBJECT()
class CNode : public CBaseObject
{
	REFLECTION()

public:
	FIELD()
	CNode* Next = nullptr;
};
