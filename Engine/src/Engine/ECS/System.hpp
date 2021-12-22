#pragma once

#include "Entity.hpp"

template<size_t ComponentCount, size_t SystemCount>
class Registry;

template<size_t ComponentCount, size_t SystemCount>
class System
{
	friend Registry<ComponentCount, SystemCount>;
public:
	virtual ~System() = default;
protected:
	template<typename ... Types>
	void setRequirements()
	{
		(m_Requirements.set(Types::type), ...);//use a fold expression to set all bits
	}
	const std::vector<Entity>& getManagedEntities() const
	{
		return m_ManagedEntities;
	}
	inline Index getManagedEntityFromEntity(Entity e) const { return (*m_EntityToManagedEntity)[e]; }
	//these are to be implemented by the derived class
	virtual void onManagedEntityAdded([[maybe_unused]] Entity e) {  }
	virtual void onManagedEntityRemoved([[maybe_unused]] Entity e) {  }
private:
	std::bitset<ComponentCount> m_Requirements;//the type requirements for the system
	size_t m_id;
	std::vector<Entity> m_ManagedEntities;
	//std::unordered_map<Entity, Index> m_EntityToManagedEntity;
	std::vector<Index>* m_EntityToManagedEntity = nullptr;

	//clear all data from the system(excluding information about type requirement)
	void clear() { onSystemClear(); m_ManagedEntities.clear(); }
	virtual void onSystemClear() {  }
	void setUp(size_t id, std::vector<Index>* entityToManagedEntity) { m_id = id; m_EntityToManagedEntity = entityToManagedEntity; }//called by Registry to assign an id to the system
	void onEntityUpdated(Entity e, const std::bitset<ComponentCount>& components/*the entity's component bitset*/)
	{
		bool satisfied = (m_Requirements & components) == m_Requirements;
		//bool managed = m_EntityToManagedEntity.find(e) != std::end(m_EntityToManagedEntity);
		bool managed = (*m_EntityToManagedEntity)[e] != invalidIndex;
		if (satisfied && !managed)
			addEntity(e);
		else if (managed && !satisfied)
			removeEntity(e);
		//else the entity doesn't satisfy the conditions and isn't managed eigther, so don't do anything
		//or it satisfies the conditions and is already managed, which doesn't require for any action eighter
	}
	void onEntityRemoved(Entity e)
	{
		bool managed = (*m_EntityToManagedEntity)[e] != invalidIndex;
		if (managed)
			removeEntity(e);
	}
	void addEntity(Entity e)
	{
		//m_EntityToManagedEntity[e] = static_cast<Index>(m_ManagedEntities.size());
		(*m_EntityToManagedEntity)[e] = static_cast<Index>(m_ManagedEntities.size());
		m_ManagedEntities.push_back(e);
		onManagedEntityAdded(e);
	}
	void removeEntity(Entity e)
	{
		onManagedEntityRemoved(e);//why do we call this before actually removing the entity(maybe to perform some action on the entity before actually removing it???)?
		//Index index = m_EntityToManagedEntity[e];//get the index to the entity that is to be removed into m_ManagedEntities
		//update m_EntityToMangedEntity
		//m_EntityToManagedEntity[m_ManagedEntities.back()] = index;//because m_ManagedEntities.back() will be the new entity stored at "index", we set up the unordered_map of that entity to point to the new location of it
		//m_EntityToManagedEntity.erase(e);//erase the old entity
		Index index = (*m_EntityToManagedEntity)[e];
		(*m_EntityToManagedEntity)[m_ManagedEntities.back()] = index;
		(*m_EntityToManagedEntity)[e] = invalidIndex;
		//update m_ManagedEntities
		m_ManagedEntities[index] = m_ManagedEntities.back();//move the entity at the back to the entity at "index"
		m_ManagedEntities.pop_back();//erase the now obsolete last entity
	}
};