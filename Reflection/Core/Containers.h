#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>

#define TCONTAINER_TYPENAMEOF(TYPE) #TYPE

namespace Reflection
{
	template <typename T, typename TContainer>
	class TConstIterator
	{
	protected:
		using TNativeIterator = typename TContainer::const_iterator;

		TNativeIterator Iterator;

	public:
		TConstIterator() : Iterator()
		{
		}
		TConstIterator(const TNativeIterator& InIterator) : Iterator(InIterator)
		{
		}
		TConstIterator(TNativeIterator&& InIterator) noexcept : Iterator(std::move(InIterator))
		{
		}
		TConstIterator(const TConstIterator& InIterator) : Iterator(InIterator.Iterator)
		{
		}
		TConstIterator(TConstIterator&& InIterator) noexcept : Iterator(std::move(InIterator.Iterator))
		{
		}

		TConstIterator& operator ++()
		{
			++Iterator;
			return *this;
		}
		TConstIterator operator++(int)
		{
			TNativeIterator PrevIterator = Iterator;
			++Iterator;
			return TConstIterator(PrevIterator);
		}
		TConstIterator& operator --()
		{
			--Iterator;
			return *this;
		}
		TConstIterator operator--(int)
		{
			TNativeIterator PrevIterator = Iterator;
			--Iterator;
			return TConstIterator(PrevIterator);
		}

		operator TNativeIterator() const
		{
			return Iterator;
		}

		bool operator==(const TConstIterator& Other) const
		{
			return Iterator == Other.Iterator;
		}
		bool operator!=(const TConstIterator& Other) const
		{
			return Iterator != Other.Iterator;
		}

		const T& operator * () const
		{
			return *Iterator;
		}

		const T& operator -> () const
		{
			return *Iterator;
		}

		TConstIterator& Next() const
		{
			++Iterator;
			return *this;
		}
		TConstIterator& Prev() const
		{
			--Iterator;
			return *this;
		}

		const T& Get() const
		{
			return *Iterator;
		}

		TNativeIterator GetNativeIterator() const
		{
			return Iterator;
		}
	};

	template <typename T, typename TContainer>
	class TIterator
	{	
		using TNativeIterator = typename TContainer::iterator;

		TNativeIterator Iterator;

	public:
		TIterator() : Iterator()
		{
		}
		TIterator(const TNativeIterator& InIterator) : Iterator(InIterator)
		{
		}
		TIterator(TNativeIterator&& InIterator) noexcept : Iterator(std::move(InIterator))
		{
		}
		TIterator(const TIterator& InIterator) : Iterator(InIterator.Iterator)
		{
		}
		TIterator(TIterator&& InIterator) noexcept : Iterator(std::move(InIterator.Iterator))
		{
		}

		TIterator& operator ++()
		{
			++Iterator;
			return *this;
		}
		TIterator operator++(int)
		{
			TNativeIterator PrevIterator = Iterator;
			++Iterator;
			return TIterator(PrevIterator);
		}
		TIterator& operator --()
		{
			--Iterator;
			return *this;
		}
		TIterator operator--(int)
		{
			TNativeIterator PrevIterator = Iterator;
			--Iterator;
			return TIterator(PrevIterator);
		}

		operator TConstIterator<T, TContainer>()
		{
			return TConstIterator<T, TContainer>(Iterator);
		}

		operator TNativeIterator()
		{
			return Iterator;
		}

		bool operator==(const TIterator& Other) const
		{
			return Iterator == Other.Iterator;
		}
		bool operator!=(const TIterator& Other) const
		{
			return Iterator != Other.Iterator;
		}

		const T& operator * () const
		{
			return *Iterator;
		}

		const T& operator -> () const
		{
			return *Iterator;
		}

		TIterator& Next() const
		{
			++Iterator;
			return *this;
		}
		TIterator& Prev() const
		{
			--Iterator;
			return *this;
		}

		T& Get()
		{
			return *Iterator;
		}
		const T& Get() const
		{
			return *Iterator;
		}

		TConstIterator<T, TContainer> ToConstIterator() const
		{
			return TConstIterator<T, TContainer>(Iterator);
		}

		TNativeIterator GetNativeIterator() const
		{
			return Iterator;
		}
	};

	class IContainerObjectExtracter
	{
	public:
		virtual ~IContainerObjectExtracter() {}
		virtual void Append(std::vector<void**>& OutFirstObjects, std::vector<void**>& OutSecondObjects) = 0;
	};

#define TLIST_TYPENAME TCONTAINER_TYPENAMEOF(TList)
	template <typename T>
	class TList : public IContainerObjectExtracter
	{
		using TNativeVector = std::vector<T>;

		TNativeVector Vector;

	public:
		using TIterator = TIterator<T, TNativeVector>;
		using TConstIterator = TConstIterator<T, TNativeVector>;

		TList()
		{
		}
		TList(const TNativeVector& InVector) : Vector(InVector)
		{
			
		}
		TList(TNativeVector&& InVector) noexcept : Vector(std::move(InVector))
		{
		}
		TList(const TList& Other) : TList(Other.Vector)
		{
		}
		TList(TList&& Other) noexcept : TList(std::move(Other.Vector))
		{
		}
		TList(std::initializer_list<T> InInitalizerList) : TList(TNativeVector(InInitalizerList))
		{
		}
		TList(size_t InCount) : Vector(InCount)
		{
		}
		TList(size_t InCount, const T& Struct) : Vector(InCount, Struct)
		{
		}
		template <typename TIteratorType>
		TList(TIteratorType InBegin, TIteratorType InEnd) : Vector(InBegin, InEnd)
		{
		}

		TIterator Begin()
		{
			return TIterator(Vector.begin());
		}
		TConstIterator Begin() const
		{
			return TConstIterator(Vector.begin());
		}
		TIterator End()
		{
			return TIterator(Vector.end());
		}
		TConstIterator End() const
		{
			return TConstIterator(Vector.end());
		}
		TIterator begin()
		{
			return Begin();
		}
		TConstIterator begin() const
		{
			return Begin();
		}
		TIterator end()
		{
			return End();
		}
		TConstIterator end() const
		{
			return End();
		}

		TList& operator = (const TList& Other)
		{
			Vector = Other.Vector;
			return *this;
		}
		TList& operator = (TList&& Other) noexcept
		{
			Vector = std::move(Other.Vector);
			return *this;
		}
		TList& operator = (std::initializer_list<T> InInitalizerList)
		{
			Vector = InInitalizerList;
			return *this;
		}

		bool operator == (const TList& Other) const
		{
			return Vector == Other.Vector;
		}
		bool operator != (const TList& Other) const
		{
			return Vector != Other.Vector;
		}

		T& operator[](size_t InPosition)
		{
			return Vector[InPosition];
		}
		const T& operator[](size_t InPosition) const
		{
			return Vector[InPosition];
		}

		T& At(size_t InPosition)
		{
			return Vector[InPosition];
		}
		const T& At(size_t InPosition) const
		{
			return Vector[InPosition];
		}

		T* Data()
		{
			return Vector.data();
		}
		const T* Data() const
		{
			return Vector.data();
		}

		bool Empty() const
		{
			return Vector.empty();
		}

		size_t Size() const
		{
			return Vector.size();
		}

		void Clear()
		{
			return Vector.clear();
		}

		void PushBack(T&& InValue) noexcept
		{
			Vector.push_back(std::move(InValue));
		}
		void PushBack(const T& InValue)
		{
			Vector.push_back(InValue);
		}

		void Insert(TConstIterator InWhere, std::initializer_list<T> InInitializeList)
		{
			Vector.insert(InWhere, InInitializeList);
		}
		void Insert(TConstIterator InWhere, size_t InCount, const T& InValue)
		{
			Vector.insert(InWhere, InCount, InValue);
		}
		void Insert(TConstIterator InWhere, const T& InValue)
		{
			Vector.insert(InWhere, InValue);
		}
		template <typename TIteratorType>
		void Insert(TConstIterator InWhere, TIteratorType InBegin, TIteratorType InEnd)
		{
			Vector.insert(InWhere, InBegin, InEnd);
		}

		template <typename... Param>
		void EmplaceBack(Param&&... InParameters) noexcept
		{
			Vector.emplace_back(std::forward<Param>(InParameters));
		}

		template <typename... Param>
		void Emplace(TConstIterator InWhere, Param&&... InParameters) noexcept
		{
			Vector.emplace(InWhere, std::forward<Param>(InParameters));
		}

		void PopBack()
		{
			Vector.pop_back();
		}

		TIterator Erase(TConstIterator InWhere)
		{
			return TIterator(Vector.erase(InWhere));
		}

		TIterator Erase(TConstIterator InBegin, TConstIterator InEnd)
		{
			return TIterator(Vector.erase(InBegin, InEnd));
		}

		void Reserve(size_t InCapacity)
		{
			Vector.reserve(InCapacity);
		}

		void Resize(size_t InSize, const T& InValue)
		{
			Vector.resize(InSize, InValue);
		}

		void ShrinkToFit()
		{
			Vector.shrink_to_fit();
		}

		void Swap(TList& Other)
		{
			Vector.swap(Other.Vector);
		}

	private:
		virtual void Append(std::vector<void**>& OutFirstObjects, std::vector<void**>& OutSecondObjects) override
		{
			for (T& Struct : Vector)
			{
				void** Ptr = (void**)(&Struct);
				OutFirstObjects.push_back(Ptr);
			}
		}
	};

#define TSET_TYPENAME TCONTAINER_TYPENAMEOF(TSet)
	template <typename T, typename THasher = std::hash<T>>
	class TSet : public IContainerObjectExtracter
	{
		using TNativeSet = std::unordered_set<T, THasher>;

		TNativeSet Set;

	public:
		using TIterator = TIterator<T, TNativeSet>;
		using TConstIterator = TConstIterator<T, TNativeSet>;

		TSet() : Set()
		{
		}
		TSet(const TNativeSet& InSet) : Set(InSet)
		{
		}
		TSet(TNativeSet&& InSet) noexcept : Set(std::move(InSet))
		{
		}
		TSet(const TSet& Other) : Set(Other.Set)
		{
		}
		TSet(TSet&& Other) noexcept : Set(std::move(Other.Set))
		{
		}
		template <typename TIteratorType>
		TSet(TIteratorType InBegin, TIteratorType InEnd) : Set(InBegin, InEnd)
		{
		}
		TSet(std::initializer_list<T> InInitializeList) : Set(InInitializeList)
		{
		}

		TIterator Begin()
		{
			return TIterator(Set.begin());
		}
		TConstIterator Begin() const
		{
			return TConstIterator(Set.begin());
		}
		TIterator End()
		{
			return TIterator(Set.end());
		}
		TConstIterator End() const
		{
			return TConstIterator(Set.end());
		}
		TIterator begin()
		{
			return Begin();
		}
		TConstIterator begin() const
		{
			return Begin();
		}
		TIterator end()
		{
			return End();
		}
		TConstIterator end() const
		{
			return End();
		}

		TSet& operator = (const TSet& Other)
		{
			Set = Other.Set;
			return *this;
		}
		TSet& operator = (TSet&& Other) noexcept
		{
			Set = std::move(Other.Set);
			return *this;
		}
		TSet& operator = (std::initializer_list<T> InInitalizerList)
		{
			Set = InInitalizerList;
			return *this;
		}

		bool operator == (const TSet& Other) const
		{
			return Set == Other.Set;
		}
		bool operator != (const TSet& Other) const
		{
			return Set != Other.Set;
		}

		bool Empty() const
		{
			return Set.empty();
		}

		size_t Size() const
		{
			return Set.size();
		}

		void Clear()
		{
			return Set.clear();
		}

		template <typename TIteratorType>
		void Insert(TIteratorType InBegin, TIteratorType InEnd)
		{
			Set.insert(InBegin, InEnd);
		}
		void Insert(std::initializer_list<T> InInitializeList)
		{
			Set.insert(InInitializeList);
		}
		std::pair<TIterator, bool> Insert(const T& InValue)
		{
			std::pair<std::unordered_set<T>::iterator, bool> ResultPair = Set.insert(InValue);
			return std::make_pair(TIterator(ResultPair.first), ResultPair.second);
		}
		std::pair<TIterator, bool> Insert(T&& InValue) noexcept
		{
			std::pair<std::unordered_set<T>::iterator, bool> ResultPair = Set.insert(std::move(InValue));
			return std::make_pair(TIterator(ResultPair.first), ResultPair.second);
		}

		template <typename... Param>
		std::pair<TIterator, bool> Emplace(Param&&... InParameters) noexcept
		{
			std::pair<TNativeSet::iterator, bool> ResultPair = Set.emplace(std::forward<Param>(InParameters));
			return std::make_pair(TIterator(ResultPair.first), ResultPair.second);
		}

		size_t Erase(const T& InValue)
		{
			return Set.erase(InValue);
		}
		TIterator Erase(TConstIterator InWhere)
		{
			return TIterator(Set.erase(InWhere));
		}
		TIterator Erase(TConstIterator InBegin, TConstIterator InEnd)
		{
			return TIterator(Set.erase(InBegin, InEnd));
		}

		TIterator Find(const T& InValue)
		{
			return TIterator(Set.find(InValue));
		}

		void Reserve(size_t InCapacity)
		{
			Set.reserve(InCapacity);
		}

		void Swap(TSet& Other)
		{
			Set.swap(Other.Vector);
		}

	private:
		virtual void Append(std::vector<void**>& OutFirstObjects, std::vector<void**>& OutSecondObjects) override
		{
			for (const T& Struct : Set)
			{
				void** Ptr = (void**)(&Struct);
				OutFirstObjects.push_back(Ptr);
			}
		}
	};

#define TMAP_TYPENAME TCONTAINER_TYPENAMEOF(TMap)
	template <typename TKey, typename TValue, typename THasher = std::hash<TKey>>
	class TMap : public IContainerObjectExtracter
	{
		using TNativeMap = std::unordered_map<TKey, TValue, THasher>;

		TNativeMap Map;

	public:
		using TPair = std::pair<const TKey, TValue>;
		using TIterator = TIterator<TPair, TNativeMap>;
		using TConstIterator = TConstIterator<const TPair, TNativeMap>;

		TMap() : Map()
		{
		}
		TMap(const TNativeMap& InMap) : Map(InMap)
		{
		}
		TMap(TNativeMap&& InMap) noexcept : Map(std::move(InMap))
		{
		}
		TMap(const TMap& Other) : Map(Other.Map)
		{
		}
		TMap(TMap&& Other) noexcept : Map(std::move(Other.Map))
		{
		}
		template <typename TIteratorType>
		TMap(TIteratorType InBegin, TIteratorType InEnd) : Map(InBegin, InEnd)
		{
		}
		TMap(std::initializer_list<TPair> InInitializeList) : Map(InInitializeList)
		{
		}

		TIterator Begin()
		{
			return TIterator(Map.begin());
		}

		TConstIterator Begin() const
		{
			return TConstIterator(Map.begin());
		}

		TIterator End()
		{
			return TIterator(Map.end());
		}

		TConstIterator End() const
		{
			return TConstIterator(Map.end());
		}

		TIterator begin()
		{
			return Begin();
		}

		TConstIterator begin() const
		{
			return Begin();
		}

		TIterator end()
		{
			return End();
		}

		TConstIterator end() const
		{
			return End();
		}

		TMap& operator = (const TMap& Other)
		{
			Map = Other.Map;
			return *this;
		}
		TMap& operator = (TMap&& Other) noexcept
		{
			Map = std::move(Other.Map);
			return *this;
		}
		TMap& operator = (std::initializer_list<TPair> InInitalizerList)
		{
			Map = InInitalizerList;
			return *this;
		}

		bool operator == (const TMap& Other) const
		{
			return Map == Other.Map;
		}
		bool operator != (const TMap& Other) const
		{
			return Map != Other.Map;
		}

		TValue& operator[](const TKey& InKey)
		{
			return Map[InKey];
		}
		TValue& operator[](TKey&& InKey) noexcept
		{
			return Map[std::move(InKey)];
		}

		bool Empty() const
		{
			return Map.empty();
		}

		size_t Size() const
		{
			return Map.size();
		}

		void Clear()
		{
			return Map.clear();
		}

		std::pair<TIterator, bool> Insert(const TPair& InPair)
		{
			std::pair<TNativeMap::iterator, bool> ResultPair = Map.insert(InPair);
			return std::make_pair(TIterator(ResultPair.first), ResultPair.second);
		}
		std::pair<TIterator, bool> Insert(TPair&& InPair) noexcept
		{
			std::pair<TNativeMap::iterator, bool> ResultPair = Map.insert(std::move(InPair));
			return std::make_pair(TIterator(ResultPair.first), ResultPair.second);
		}
		void Insert(std::initializer_list<TPair> InInitalizeList)
		{
			Map.insert(InInitalizeList);
		}
		template <typename TIteratorType>
		void Insert(TIteratorType InBegin, TIteratorType InEnd)
		{
			Map.insert(InBegin, InEnd);
		}

		template <typename... Param>
		std::pair<TIterator, bool> Emplace(Param&&... InParameters) noexcept
		{
			std::pair<TNativeMap::iterator, bool> ResultPair = Map.emplace(std::forward<Param>(InParameters));
			return std::make_pair(TIterator(ResultPair.first), ResultPair.second);
		}
		template <typename TKeyType, typename TValueType>
		std::pair<TIterator, bool> Emplace(TKeyType&& InKey, TValueType&& InValue) noexcept
		{
			std::pair<TNativeMap::iterator, bool> ResultPair = Map.emplace(std::forward<TKeyType>(InKey), std::forward<TValueType>(InValue));
			return std::make_pair(TIterator(ResultPair.first), ResultPair.second);
		}

		TIterator Find(const TKey& InKey)
		{
			return TIterator(Map.find(InKey));
		}
		TConstIterator Find(const TKey& InKey) const
		{
			return TConstIterator(Map.find(InKey));
		}

		void Reserve(size_t InCapacity)
		{
			Map.reserve(InCapacity);
		}

		void Swap(TMap& Other)
		{
			Map.swap(Other.Map);
		}

	private:
		virtual void Append(std::vector<void**>& OutFirstObjects, std::vector<void**>& OutSecondObjects) override
		{
			for (TPair& Pair : Map)
			{
				void** FirstPtr = (void**)(&Pair.first);
				void** SecondPtr = (void**)(&Pair.second);
				OutFirstObjects.push_back(FirstPtr);
				OutSecondObjects.push_back(SecondPtr);
			}
		}
	};
}
