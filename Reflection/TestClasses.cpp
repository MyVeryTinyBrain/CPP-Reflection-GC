#include "TestClasses.h"
#include <iostream>

using namespace std;

int CBaseObject::NumObjects = 0;

CBaseObject::CBaseObject()
{
	++NumObjects;
}

void CBaseObject::GCDestructor()
{
	--NumObjects;

	cout << "Objects: " << NumObjects << "\t|\t" << "Remove " << ObjectType()->GetObjectName() << endl;
}

CRootObject::CRootObject()
{
	Reflection::CGarbageCollector::Instance()->RegistRootObject(this);
}
