#pragma once
#include <Engpch.hpp>

#include "Entitiy.hpp"
#include "util.hpp"

#define ALLOCATES(x) x.capacity() == x.size()//checks whether a vector is full and thus will allocate the next time an element is pushed back

namespace Engine
{
	template<typename ... Types>
	class Registry
	{
	public:
		Registry()
		{
			//allocate the storage for all the types required
			vecAllocator<Registry<Types...>::n, 0, Types...>()(m_componentVecs);
			setAllocator<Registry<Types...>::n, 0, Types...>()(m_ptrSets);

			//construct a jumpTable for efficient "branching" when dealing with componentTypes
			forAllTypes<0, Types...>::fillDestructJumpTable(m_destructComponentJmpTable);
		}

		[[nodiscard]] Entity<Types ...>* create()
		{
			m_entities.insert(std::make_pair<uint32_t, Entity<Types ...>>(m_lastID + 1, m_lastID + 1));
			m_lastID++;
			return &m_entities.at(m_lastID);
		}
		void destruct(Entity<Types ...>** e)
		{
			//destroy all of the entitie's components and the entity itself
			for (auto& component : (*e)->m_components)
			{
				std::invoke(m_destructComponentJmpTable[component.first], *this, component.second);//I wonder if "*this" copies the whole registry(I don't think so, but maybe investigate that)
			}
			m_entities.erase((*e)->m_id);
			*e = nullptr;
		}
		template<typename T>
		void addComponent(Entity<Types ...>* e, const T& component)
		{
			const_PtrPtr<T> ptr = push<T>(component);
			e->addComponent(ptr);
		}
		template<typename T>
		void reserve(const uint32_t n)
		{
			auto& vec = getVecByType<T>();
			vec.reserve(n);
			updatePtrs(vec, getSetByType<T>());
		}

		static constexpr size_t n = sizeof ...(Types);//make this public, so that I don't have to declare every template instantiation friends to each other
	private:
		template<typename T>
		const_PtrPtr<T> push(const T& component)
		{
			auto& vec = getVecByType<T>();//get the vec of the corresponding type
			auto& set = getSetByType<T>();//get the set of the corresponding type
			bool allocates = ALLOCATES(vec);//if the vec allocated when the new element got pushed back, update all pointers
			vec.push_back(component);
			//set.insert(&vec.back());
			if (allocates)
				updatePtrs(vec, set);
			set.insert(&vec.back());
			return const_PtrPtr<T>(&(*set.rbegin()));//it is safe to do this, because the last element in the set is always the new element, that just got added
		}

		template<typename T>
		void updatePtrs(std::vector<T>& vec, std::set<T*>& ptrSet)
		{
			std::set<T*> newSet;
			uint32_t i = 0;
			typename std::set<T*>::const_iterator delIt;
			for (auto it = ptrSet.begin(); it != ptrSet.end();)
			{
				delIt = it; it++;
				auto nodeHandler = ptrSet.extract(*delIt);
				nodeHandler.value() = &vec[i];
				newSet.insert(std::move(nodeHandler));
				i++;
			}
			ptrSet = std::move(newSet);
		}

		template<typename T>
		void destructComponent(const const_PtrPtr<void>& component)
		{
			auto& vec = getVecByType<T>();//get the vector of the corresponding type
			auto& set = getSetByType<T>();//get the set of the corresponding type
			T* ptr = (T*)*component.get();
			size_t index = vecIndexFromPtr(vec, ptr);
			if (index == (vec.size() - 1))
			{
				//the optimized deletion will probably just delete the last element from the set as well as the vector( -> look into how we can convert reverse_iterator to iterator or if we should use "--set.end()")
				vec.pop_back();
				set.erase(ptr);//remove the element, which is also the last element //since there is no function to remove the last element of a set, I assume this is the fastest way to do it
			}
			else
			{
				vec[index] = vec.back();//replace the element that is to be deleted with the last element
				vec.pop_back();//pop the last element -> this is the key improvement of this algorithm, because no reallocation is required whatsoever
				set.erase(ptr);//erase the element from the set
				auto nodeHandler = set.extract(*set.rbegin());//extract the last element of the set(-> this is the element that points to the last element of the vector)
				nodeHandler.value() = ptr;
				set.insert(std::move(nodeHandler));//after this procedure the element in the set should still be at the same address but it's value should have changed to point to the new location of the proviously last element
			}
		}

		template<typename T>
		constexpr std::vector<T>& getVecByType()
		{
			return *(std::vector<T>*)m_componentVecs[getTypeIndexByType<T>()];
		}

		template<typename T>
		constexpr std::set<T*>& getSetByType()
		{
			return *(std::set<T*>*)m_ptrSets[getTypeIndexByType<T>()];
		}

		template<typename T>
		constexpr size_t getTypeIndexByType()
		{
			//return typeIndexGetter<n, 0, Types...>::template getIndex<T>();//in solchen Fällen muss hier noch "template" mit eingesetzt werden, da man sonst einen Compilerfehler bekommt
			return typeIndexGetter<Types ...>::template getIndex<T>();
		}

		std::unordered_map<uint32_t, Entity<Types ...>> m_entities;

		typedef void (Registry::* destructComponentFn)(const const_PtrPtr<void>&);
		destructComponentFn m_destructComponentJmpTable[Registry<Types ...>::n] = { nullptr };

		void* m_componentVecs[Registry<Types... >::n];
		void* m_ptrSets[Registry<Types ...>::n];

		uint32_t m_lastID = 0;//this means, that there can never be an Entity with ID 0 -> could use that for some kind of assertion(ASSERT(entity.id))

		template<size_t i, typename ... Types>
		struct forAllTypes
		{
			static constexpr void fillDestructJumpTable(destructComponentFn* jumpTable)
			{
				if constexpr (i < sizeof ...(Types))
				{
					jumpTable[i] = &Registry<Types ...>::template destructComponent<NthTypeOf<i, Types...>>;
					forAllTypes<i + 1, Types...>::fillDestructJumpTable(jumpTable);
				}
			}
		};
	};
}