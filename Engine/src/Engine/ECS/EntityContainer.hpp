#pragma once

#include <bitset>
#include <vector>
#include <numeric>
#include <array>

#include "Entity.hpp"

template<size_t ComponentCount, size_t SystemCount>
class EntityContainer
{
public:
	void reserve(std::size_t count);
	std::vector<std::bitset<ComponentCount>>& getEntityToBitset();
	const std::bitset<ComponentCount>& getBitset(Entity e) const;
	Entity create();
	void remove(Entity e);
	std::vector<Index>& getEntityToComponent(size_t type);
	std::vector<Index>& getEntityToManagedEntity(size_t systemID);
	void clear();
private:
	std::vector<std::bitset<ComponentCount>> m_EntityToBitset;
	std::array<std::vector<Index>, ComponentCount> m_EntityToComponents;
	std::array<std::vector<Index>, SystemCount> m_EntityToManagedEntities;
	std::vector<Entity> m_FreeEntities;
};

template<size_t ComponentCount, size_t SystemCount>
void EntityContainer<ComponentCount, SystemCount>::clear()
{
	m_EntityToBitset.clear();
	m_FreeEntities.clear();
	for (auto& entityToComponent : m_EntityToComponents)
		entityToComponent.clear();
	for (auto& entityToManagedEntity : m_EntityToManagedEntities)
		entityToManagedEntity.clear();
}

template<size_t ComponentCount, size_t SystemCount>
std::vector<Index>& EntityContainer<ComponentCount, SystemCount>::getEntityToComponent(size_t type)
{
	return m_EntityToComponents[type];
}

template<size_t ComponentCount, size_t SystemCount>
std::vector<Index>& EntityContainer<ComponentCount, SystemCount>::getEntityToManagedEntity(size_t systemID)
{
	return m_EntityToManagedEntities[systemID];
}

template<size_t ComponentCount, size_t SystemCount>
std::vector<std::bitset<ComponentCount>>& EntityContainer<ComponentCount, SystemCount>::getEntityToBitset()
{
	return m_EntityToBitset;
}

template<size_t ComponentCount, size_t SystemCount>
const std::bitset<ComponentCount>& EntityContainer<ComponentCount, SystemCount>::getBitset(Entity e) const
{
	return m_EntityToBitset[e];
}

template<size_t ComponentCount, size_t SystemCount>
Entity EntityContainer<ComponentCount, SystemCount>::create()
{
	Entity e = Entity();
	if (m_FreeEntities.empty())
	{
		e = static_cast<Entity>(m_EntityToBitset.size());//assign the next id
		m_EntityToBitset.emplace_back();//at the index of the new entity(last element) construct a new empty bitset(it is important to explicitly call the constructor)
		for (auto& entityToComponent : m_EntityToComponents)
			entityToComponent.emplace_back(0);//for every type of component construct a new entity in entityToComponent and assign component index 0
		for (auto& entityToManagedEntity : m_EntityToManagedEntities)
			entityToManagedEntity.emplace_back(invalidIndex);//for every system create a new entry in entityToManagedEntity but assign invalid index to indicate that the entity doesn't have a corresponding
		//managed entity because it isn't managed
	}
	else
	{
		e = m_FreeEntities.back();
		m_FreeEntities.pop_back();
		m_EntityToBitset[e].reset();//reset the bitset associated with the entity
		for (auto& entityToManagedEntity : m_EntityToManagedEntities)
			entityToManagedEntity[e] = invalidIndex;
	}
	return e;
}

template<size_t ComponentCount, size_t SystemCount>
void EntityContainer<ComponentCount, SystemCount>::remove(Entity e)
{
	m_FreeEntities.push_back(e);
}

template<size_t ComponentCount, size_t SystemCount>
void EntityContainer<ComponentCount, SystemCount>::reserve(size_t count)
{
	m_FreeEntities.resize(count);
	std::iota(std::begin(m_FreeEntities), std::end(m_FreeEntities), 0);
	m_EntityToBitset.reserve(count);//in the original implementation this is vector::resize. I think one should use vector::reserve here, but maybe that's not true
	for (auto& entityToComponent : m_EntityToComponents)
		entityToComponent.resize(count);
	for (auto& entityToManagedEntity : m_EntityToManagedEntities)
		entityToManagedEntity.resize(count);
}