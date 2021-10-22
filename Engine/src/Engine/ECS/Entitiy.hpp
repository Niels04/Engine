#pragma once

#include "Engpch.hpp"//THIS MAY BE CAUSING PROBLEMS WHEN COMPILING

#include "util.hpp"
#include "Engine/core.hpp"//for the const_PtrPtr

namespace Engine
{
	template<typename ... Types>
	class Entity
	{
		template<typename ... Types>//I totally don't get this syntax, but it somehow declares the instanciation of "Registry" with the respective types as a friend to Entity, which is what I wanted
		friend class Registry;
	public:
		inline uint32_t getID() const { return m_id; }
		inline bool hasComponent(const uint8_t type) const { return static_cast<bool>(m_components.count(type)); }
		inline const_PtrPtr<void> getComponent(const uint8_t type) const { return m_components.find(type)->second; }
		template<typename T>
		inline const_PtrPtr<T> getComponent() const { return const_PtrPtr<T>((T**)m_components.find((uint8_t)typeIndexGetter<Types ...>::template getIndex<T>())->second.get()); }
	private:
		Entity(const uint32_t id) : m_id(id) {  }

		template<typename T>
		void addComponent(const_PtrPtr<T> component)
		{
			if (!m_components.count((uint8_t)typeIndexGetter<Types ...>::template getIndex<T>()))
				m_components.insert({ (uint8_t)typeIndexGetter<Types ...>::template getIndex<T>(), const_PtrPtr<void>((void**)component.get()) });
			else
				printf("Error inserting component: There was already a component of this type attached to the entity.\n");
		}

		uint32_t m_id;
		std::unordered_map<uint8_t, const_PtrPtr<void>> m_components;
	};
}