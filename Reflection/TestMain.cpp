#include <iostream>
#include <time.h>
#include "Core/Reflection.h"
#include "TestClasses.h"

using namespace Reflection;
using namespace std;

void ReflectionTest();
void GCTest();

int main()
{
    ReflectionTest();
    
    cout << endl;

    GCTest();

 	return 0;  
}

template <class T, typename... TParameter>
T* LogNew(TParameter&& ...InParameters)
{
    T* Object = new T(std::forward<TParameter>(InParameters)...);
    cout << "Objects: " << T::NumObjects << "\t|\t" << "Create " << Object->ObjectType()->GetObjectName() << endl;
    return Object;
}

void ReflectionTest()
{
    cout << "== Reflection Test =====================================================================================" << endl;

    CRootObject* RootObject = LogNew<CRootObject>();

    cout << endl;

    cout << "-- Information of CRootObject --------------------------------------------------------------------------" << endl;

    CObjectType* Type = RootObject->ObjectType();
    cout << Type->ToString() << endl;

    for (CField* Field : Type->GetInheritedFields())
    {
        cout << Field->ToString() << endl;
    }

    for (CFunction* Function : Type->GetInheritedFunctions())
    {
        cout << Function->ToString() << endl;
    }

    const CTestAttribute* Attribute = Type->GetInheritedCustomAttribute<CTestAttribute>();
    cout << Attribute->Context << endl;

    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "-- Function Call Test ----------------------------------------------------------------------------------" << endl;

    for (CFunction* Function : Type->GetInheritedFunctions()) {
        if (Function->GetName() == "TestFunction") {
            cout << "Call CRootObject::TestFunction" << endl;
            Function->Invoke(RootObject);
        }
        else if (Function->GetName() == "SqrtReference") {
            float f = 4;
            cout << "Call CRootObject::SqrtReference" << endl;
            Function->Invoke(RootObject, 1, MakeVoidWrapper(f));
            cout << "sqrt(4) = " << f << endl;
        }
        else if (Function->GetName() == "AddReturn") {
            int a = 10, b = 100, result;
            cout << "Call CRootObject::AddReturn" << endl;
            Function->Invoke(MakeVoidWrapper(result), RootObject, 2, MakeVoidWrapper(a), MakeVoidWrapper(b));
            cout << "10 + 100 = " << result << endl;
        }
    }

    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "-- Field Read/Write Test ----------------------------------------------------------------------------------" << endl;

    for (CField* Field : Type->GetInheritedFields()) {
        if (Field->GetName() == "BaseObjectTestVariable") {
            cout << "CRootObject::BaseObjectTestVariable = " << *(int*)Field->GetPointedValue(RootObject) << endl;
            int NewValue = 456;
            cout << "Change CRootObject::BaseObjectTestVariable to 456" << endl;
            Field->SetPointedValue(RootObject, MakeVoidWrapper(NewValue));
            cout << "CRootObject::BaseObjectTestVariable = " << *(int*)Field->GetPointedValue(RootObject) << endl;
        }
    }

    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    CGarbageCollector::Instance()->DeleteAllRootObjects();

    cout << "========================================================================================================" << endl;
}

void GCTest()
{
    cout << "== Garbage Collection Test =============================================================================" << endl;

    CRootObject* RootObject = LogNew<CRootObject>();

    cout << endl;
    
    cout << "-- Test case 1 -----------------------------------------------------------------------------------------" << endl;
    {
        cout << "* Create NodeA, NodeB, NodeC" << endl;
        RootObject->NodeA = LogNew<CNode>();
        RootObject->NodeB = LogNew<CNode>();
        RootObject->NodeC = LogNew<CNode>();

        cout << "* CNodeA = nullptr" << endl;
        RootObject->NodeA = nullptr;

        cout << "* GC" << endl;
        CGarbageCollector::Instance()->Collect();

        cout << "* CNodeB, CNodeC = nullptr" << endl;
        RootObject->NodeB = nullptr;
        RootObject->NodeC = nullptr;

        cout << "* GC" << endl;
        CGarbageCollector::Instance()->Collect();
    }
    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "-- Test case 2 -----------------------------------------------------------------------------------------" << endl;
    {
        clock_t start = clock();

        int NumNodes = 10;
        cout << "* Create " << NumNodes << "Nodes" << endl;
        for (int i = 0; i < NumNodes; ++i)
        {
            RootObject->Nodes.PushBack(LogNew<CNode>());
        }

        cout << "* CNode[0 ~ 9] = nullptr" << endl;
        for (int i = 0; i < NumNodes; ++i)
        {
            RootObject->Nodes[i] = nullptr;
        }

        cout << "* GC" << endl;
        CGarbageCollector::Instance()->Collect();

        cout << "* Create " << NumNodes << "Nodes" << endl;
        for (int i = 0; i < NumNodes; ++i)
        {
            RootObject->Nodes.PushBack(LogNew<CNode>());
        }

        cout << "* Clear Node List" << endl;
        RootObject->Nodes.Clear();

        cout << "* GC" << endl;
        CGarbageCollector::Instance()->Collect();
        
        cout << "* Time taken to process: " << (double)(clock() - start) / (double)CLOCKS_PER_SEC << endl;
    }
    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "-- Test case 3 -----------------------------------------------------------------------------------------" << endl;
    {
        int NumNodes = 10;
        cout << "* Create " << NumNodes << "Nodes to List of RootObject->Struct" << endl;
        for (int i = 0; i < NumNodes; ++i)
        {
            RootObject->TestStruct.Nodes.PushBack(LogNew<CNode>());
        }

        cout << "* Reset RootObject->Struct" << endl;
        RootObject->TestStruct = {};

        cout << "* GC" << endl;
        CGarbageCollector::Instance()->Collect();
    }
    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "-- Test case 4 -----------------------------------------------------------------------------------------" << endl;
    {
        cout << "* Create NodeA And Set to head of Linked list" << endl;
        RootObject->NodeA = LogNew<CNode>();

        int NumNodes = 10;
        cout << "* Make Linked list with " << NumNodes << "Nodes" << endl;
        CNode* CurrentNode = RootObject->NodeA;
        for (int i = 0; i < NumNodes; ++i)
        {
            CurrentNode->Next = LogNew<CNode>();;
            CurrentNode = CurrentNode->Next;
        }

        cout << "* Set head to nullptr" << endl;
        RootObject->NodeA = nullptr;

        cout << "* GC" << endl;
        CGarbageCollector::Instance()->Collect();
    }
    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "-- Test case 5 -----------------------------------------------------------------------------------------" << endl;
    {
        int NumNodes = 10;
        cout << "* Create " << NumNodes << "Nodes To Set" << endl;
        for (int i = 0; i < NumNodes; ++i)
        {
            RootObject->NodeSet.Insert(LogNew<CNode>());
        }

        cout << "* Clear Set" << endl;
        RootObject->NodeSet.Clear();
        
        cout << "* GC" << endl;
        CGarbageCollector::Instance()->Collect();
    }
    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "-- Test case 6 -----------------------------------------------------------------------------------------" << endl;
    {
        int NumNodes = 10;
        cout << "* Create " << NumNodes << "Nodes To Map" << endl;
        for (int i = 0; i < NumNodes; ++i)
        {
            RootObject->NodeMap.Emplace(i, LogNew<CNode>());
        }

        cout << "* Clear Map" << endl;
        RootObject->NodeMap.Clear();

        cout << "* GC" << endl;
        CGarbageCollector::Instance()->Collect();
    }
    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "-- Test case 7 -----------------------------------------------------------------------------------------" << endl;
    {
        int NumNodes = 10;
        cout << "* Create " << NumNodes << "Nodes" << endl;
        for (int i = 0; i < NumNodes; ++i)
        {
            RootObject->Nodes.PushBack(LogNew<CNode>());
        }

        cout << "* There is no collection after creations." << endl;
    }
    cout << "--------------------------------------------------------------------------------------------------------" << endl;

    cout << endl;

    cout << "* Clear And CG" << endl;
    CGarbageCollector::Instance()->DeleteAllRootObjects();

    cout << "========================================================================================================" << endl;
}