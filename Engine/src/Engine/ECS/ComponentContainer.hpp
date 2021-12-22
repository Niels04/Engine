#pragma once

#include <vector>
#include <unordered_map>
#include <bitset>

#include "Component.hpp"
#include "Entity.hpp"

class BaseComponentContainer
{
public:
	virtual ~BaseComponentContainer() = default;

	virtual void clear() = 0;
	virtual void reserve(size_t count) = 0;
	virtual bool tryRemove(Entity e) = 0;
};

template<typename T, size_t ComponentCount, size_t SystemCount>
class ComponentContainer : public BaseComponentContainer
{
public:
	/*ComponentContainer(std::vector<std::bitset<ComponentCount>>& entityToBitset)
		: m_EntityToBitset(entityToBitset)
	{
	
	}*/
	ComponentContainer(std::vector<Index>& entityToComponent, std::vector<std::bitset<ComponentCount>>& entityToBitset)
		: m_EntityToComponent(entityToComponent), m_EntityToBitset(entityToBitset)
	{

	}
	
	virtual void clear() override
	{
		m_Components.clear();
		m_ComponentToEntity.clear();
	}

	virtual void reserve(size_t count) override
	{
		m_Components.reserve(count);
		m_ComponentToEntity.reserve(count);
		//m_EntityToComponent.reserve(count);
	}

	T& get(Entity e)
	{
		return m_Components[m_EntityToComponent[e]];
	}
	const T& get(Entity e) const
	{
		return m_Components[m_EntityToComponent[e]];
	}
	template<typename ... Args>
	void add(Entity e, Args&&... args)
	{
		Index index = static_cast<Index>(m_Components.size());//get index for the new component
		m_Components.push_back(T(std::forward<Args>(args)...));//push back the new component ----- ORIGINALLY THIS WAS WITHOUT THE EXPLICIT CONSTRUCTOR CALL, but that caused errors
		m_ComponentToEntity.push_back(e);//setup a link in the m_ComponentsToEntity vector to the entity
		m_EntityToComponent[e] = index;//store the componentIndex by it's entity
		m_EntityToBitset[e][T::type] = true;//set the type-bit for the added component to true
	}
	void remove(Entity e)
	{
		m_EntityToBitset[e][T::type] = false;
		Index index = m_EntityToComponent[e];//get the index of the component of the entity
		//update m_Components
		m_Components[index] = std::move(m_Components.back());//move the back component to the index where we want to remove a component
		m_Components.pop_back();//pop the last component in order to avoid reallocation
		//update m_EntityToComponent
		m_EntityToComponent[m_ComponentToEntity.back()] = index;//the entity that previously pointed to the last element now points to the component at index "index" because it got moved there
		//update m_ComponentToEntity
		m_ComponentToEntity[index] = m_ComponentToEntity.back();//perform the same operation as with m_Components
		m_ComponentToEntity.pop_back();
	}
	virtual bool tryRemove(Entity e) override
	{
		if (m_EntityToBitset[e][T::type])
		{
			remove(e);
			return true;
		}
		return false;
	}
	Entity getOwner(const T& component) const
	{
		const T* begin = m_Components.data();//get pointer to begin of the componentVec
		size_t index = static_cast<size_t>(&component - begin);//get index of the component
		return m_ComponentToEntity[index];
	}
	std::vector<T>& getAll() { return m_Components; }
private:
	std::vector<T> m_Components;
	std::vector<Entity> m_ComponentToEntity;
	//std::unordered_map<Entity, Index> m_EntityToComponent;//stores component indices by entities
	std::vector<Index>& m_EntityToComponent;//stores component indices by entities
	std::vector<std::bitset<ComponentCount>>& m_EntityToBitset;
};