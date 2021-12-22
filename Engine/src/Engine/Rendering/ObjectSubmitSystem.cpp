#include "Engpch.hpp"

#include "ObjectSubmitSystem.hpp"
#include "renderer.hpp"

namespace Engine
{
	void ObjectSubmitSystem::sub()
	{
		for (auto& e : getManagedEntities())
		{
			Renderer::sub(m_registry.get<MeshComponent>(e), m_registry.get<TransformComponent>(e));
		}
	}
}