#pragma once

#include "Engpch.hpp"//MAYBE THIS CAUSES PROBLEMS WHEN COMPILING

#define vecItIndex(vec, it) it - vec.begin()

namespace Engine
{
	template<typename T>
	inline typename std::vector<T>::const_iterator vecIteratorFromPtr(std::vector<T>& vec, T* ptr)
	{
		typename std::vector<T>::iterator it = vec.begin();//first associate the iterator with the container(that's why we need the vector here)
		size_t diff = ptr - &vec[0];
		//diff /= sizeof(T);//since we are dealing with T* here, we can just subtract to get the difference in elements of type T rather than in bytes
		//size_t diff = (ptr - &vec[0]) / sizeof(T);//calculate the difference
		it += diff;//increment the iterator by the difference
		return it;
	}

	template<typename T>
	inline typename std::list<T>::iterator listIteratorFromPtr(std::list<T>& list, T* ptr)
	{
		typename std::list<T>::iterator it = list.begin();//we must first set the iterator to the list's begin in order to associate it with the list
		it._Ptr = (std::_List_node<T, void*>*)((char*)ptr - sizeof(std::_List_node<T, void*>) + sizeof(T));//then the pointer is set to the pointer to the (element - sizeof(node)) + sizeof(element)
		return it;
	}

	template<typename T>
	inline const size_t vecIndexFromPtr(std::vector<T>& vec, T* ptr)
	{
		size_t diff = ptr - &vec[0];
		_ASSERT(diff < vec.size());//otherwise the index would be out of bounds
		return diff;
	}

	template<size_t N, typename ... Types> using NthTypeOf =
		typename std::tuple_element<N, std::tuple<Types...>>::type;

	template<typename T, typename V>
	struct isSame
	{
		enum { value = 0 };
	};
	template<typename T>
	struct isSame<T, T>
	{
		enum { value = 1 };
	};

	template<size_t n, size_t i, typename ... Types>
	struct vecAllocator
	{
		void operator()(void** data) const
		{
			static_assert(sizeof ...(Types), "Yout can't create a vecAllocator without specifying at least one type!");//you cannot create a Registry with no data in it
			if constexpr (n > i)
			{
				data[i] = new std::vector<NthTypeOf<i, Types...>>;
				vecAllocator<n, i + 1, Types...>()(data);
			}
		}
	};

	template<size_t n, size_t i, typename ... Types>
	struct listAllocator
	{
		void operator()(void** data) const
		{
			static_assert(sizeof ...(Types), "Yout can't create a listAllocator without specifying at least one type!");//you cannot create a Registry with no data in it
			if constexpr (n > i)
			{
				data[i] = new std::list<NthTypeOf<i, Types...>>;
				listAllocator<n, i + 1, Types...>()(data);
			}
		}
	};

	template<size_t n, size_t i, typename ... Types>
	struct mapAllocator
	{
		void operator()(void** data) const
		{
			static_assert(sizeof ...(Types), "Yout can't create a mapAllocator without specifying at least one type!");//you cannot create a Registry with no data in it
			if constexpr (n > i)
			{
				data[i] = new std::map<NthTypeOf<i, Types...>*, NthTypeOf<i, Types...>**>;
				mapAllocator<n, i + 1, Types...>()(data);
			}
		}
	};

	template<size_t n, size_t i, typename ... Types>
	struct setAllocator
	{
		void operator()(void** data) const
		{
			static_assert(sizeof ...(Types), "Yout can't create a setAllocator without specifying at least one type!");//you cannot create a Registry with no data in it
			if constexpr (n > i)
			{
				data[i] = new std::set<NthTypeOf<i, Types...>*>;
				setAllocator<n, i + 1, Types...>()(data);
			}
		}
	};

	template<typename ... Types>
	struct typeIndexGetter
	{
		template<typename T>
		static constexpr size_t getIndex()//beware that this will only return the first occurence of "T" in "Types"!!!
		{
			return forTypes<0, T, Types ...>();
		}
	private:
		template<size_t i, typename Search, typename ... Types>
		static constexpr size_t forTypes()
		{
			if constexpr (i < sizeof ...(Types))
			{
				if constexpr (isSame<NthTypeOf<i, Types...>, Search>::value)
					return i;
				else
					return forTypes<i + 1, Search, Types ...>();
			}
		}
	};
}