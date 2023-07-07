#pragma once

#include "Wrappers.h"
#include <unordered_set>
#include <stack>
#include <mutex>
#include <condition_variable>

namespace Reflection
{
	class CObject;

	class CGarbageCollector
	{
		friend class CReflectionStaticManager;

		static CGarbageCollector* GlobalInstnace;

		std::unordered_set<CObject*> Objects;
		std::unordered_set<CObject*> RootObjects;

		bool bCachedCollectibleFields;

		std::stack<SObjectWrapper> Candidates;
		std::unordered_set<CObject*> ReferencedObjects;

		std::mutex ObjectsMutex;
		std::mutex CandidatesMutex;
		std::condition_variable CollectThreadsWakeEvent;

	private:
		CGarbageCollector();

		virtual ~CGarbageCollector();

	public:
		static CGarbageCollector* Instance();

		void RegistObject(CObject* InObject);

		void UnregistObject(CObject* InObject);

		void RegistRootObject(CObject* InObject);

		void UnregistRootObject(CObject* InObject);

		bool IsRootObject(CObject* InObject) const;

		void DeleteAllRootObjects();

		void Collect();

	private:
		void CacheCollectibleFields();
		void InitializeCollect();

		void HandleCandidate(SObjectWrapper InObjectWrapper);
		void HandleClass(SObjectWrapper InObjectWrapper);
		void HandleStruct(SObjectWrapper InObjectWrapper);
		void HandleObject(SObjectWrapper InObjectWrapper);

		void PushToCandidates(SObjectWrapper InObjectWrapper);
		bool ThreadSafePopFromCandidates(SObjectWrapper* OutObjectWrapper);
	};
}

