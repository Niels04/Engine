#pragma once

#include <memory>
#include <array>

#include "Entity.hpp"
#include "EntityContainer.hpp"
#include "Component.hpp"
#include "ComponentContainer.hpp"
#include "System.hpp"

template<size_t ComponentCount, size_t SystemCount>
class Registry
{
public:
	void clear()
	{
		m_Entities.clear();//clear the entityContainer
		for (auto& componentContainer : m_ComponentContainers)//clear all componentContainers(delete their contents but keep the empty containers)
			componentContainer->clear();
		for (auto& system : m_Systems)//clear all systems(delete their managedEntities but keep the empty systems)
			system->clear();
	}
	template<typename T>
	void registerComponent()//inform the registry that components of type "T" will be stored
	{
		checkComponentType<T>();
		//m_ComponentContainers[T::type] = std::make_unique<ComponentContainer<T, ComponentCount, SystemCount>>(m_Entities.getEntityToBitset());
		m_ComponentContainers[T::type] = std::make_unique<ComponentContainer<T, ComponentCount, SystemCount>>(m_Entities.getEntityToComponent(T::type), m_Entities.getEntityToBitset());
	}
	template<typename T, typename ... Args>
	T* createSystem(Args&& ... args)
	{
		size_t id = m_Systems.size();
		T* system = static_cast<T*>(m_Systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)).get());
		//system->setUp(id);
		system->setUp(id, &m_Entities.getEntityToManagedEntity(id));
		return system;
	}
	void reserve(size_t count)
	{
		for (size_t i = 0; i < ComponentCount; i++)//originally ++i(no difference I suppose)
		{
			if (m_ComponentContainers[i])//if this ComponentConainer exists
				m_ComponentContainers[i]->reserve(count);
		}
		m_Entities.reserve(count);
	}
	Entity create()
	{
		return m_Entities.create();
	}
	void removeEntity(Entity e)
	{
		//first remove the components
		for (size_t i = 0; i < ComponentCount; i++)
		{
			if (m_ComponentContainers[i])
				m_ComponentContainers[i]->tryRemove(e);
		}
		//then notify the systems
		for (auto& sytem : m_Systems)
			system->onEntityRemoved(e);
		//lastly delete the entity
		m_Entities.remove(e);
	}
	template<typename T>
	bool hasComponent(Entity e) const
	{
		checkComponentType<T>();//first check if components of this type are even stored
		return m_Entities.getBitset(e)[T::type];//then check if the entity has that type
	}
	template<typename ... Types>//same but with fold expressions
	bool hasComponents(Entity e)
	{
		checkComponentType<Types ...>();
		auto requirements = std::bitset<ComponentCount>();
		//(requirements.set(Types::type), ...);
		requirements.set(Types::type ...);
		return (requirements & m_Entities.getBitset(e)) == requirements;
	}
	template<typename T>
	T& get(Entity e)
	{
		checkComponentType<T>();
		return getComponentContainer<T>()->get(e);
	}
	template<typename ... Types>
	std::tuple<Types& ...> getS(Entity e)
	{
		checkComponentTypes<Types ...>();
		return std::tie(getComponentContainer<Types>()->get(e)...);
	}
	template<typename T>
	std::vector<T>& getAllOf()
	{
		return getComponentContainer<T>()->getAll();
	}
	template<typename T, typename ... Args>
	void addComponent(Entity e, Args&& ... args)
	{
		checkComponentType<T>();
		getComponentContainer<T>()->add(e, std::forward<Args>(args)...);
		//update systems
		const auto& bitset = m_Entities.getBitset(e);
		for (auto& system : m_Systems)
			system->onEntityUpdated(e, bitset);
	}
	template<typename T>
	void removeComponent(Entity e)
	{
		checkComponentType<T>();
		getComponentContainer<T>()->remove(e);
		//update systems
		const auto& bitset = m_Entities.getBitset(e);
		for (auto& system : m_Systems)
			system->onEntityEntityUpdated(e, bitset);
	}
	template<typename T>
	Entity getOwner(const T& component) const
	{
		return getComponentContainer<T>()->getOwner(component);
	}
private:
	std::array<std::unique_ptr<BaseComponentContainer>, ComponentCount> m_ComponentContainers;
	EntityContainer<ComponentCount, SystemCount> m_Entities;
	std::vector<std::unique_ptr<System<ComponentCount, SystemCount>>> m_Systems;//why isn't this an array(we already now the max systemCount at compile time)???  -> maybe we can't construct systems on the stack because thei're virtual
	//I think it isn't an array because at times we need to loop over all currently used systems and it wouldn't be ideal if there were empty unique_ptrs floating around in our array
	template<typename T>
	void checkComponentType() const
	{
		static_assert(std::is_base_of_v<Component<T>, T>, "Component must be derived from base component class.");
	}
	template<typename ... Types>
	void checkComponentTypes() const
	{
		(checkComponentType<Types>(), ...);
	}
	template<typename T>
	auto getComponentContainer()//is auto here just BaseComponentContainer???
	{
		return static_cast<ComponentContainer<T, ComponentCount, SystemCount>*>(m_ComponentContainers[T::type].get());
	}
	template<typename T>
	auto getComponentContainer() const
	{
		return static_cast<const ComponentContainer<T, ComponentCount, SystemCount>*>(m_ComponentContainers[T::type].get());
	}
};