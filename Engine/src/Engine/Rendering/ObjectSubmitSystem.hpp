#pragma once

#include "Engine/ECS/Registry.hpp"
#include "Engine/objects/components.hpp"

namespace Engine
{
	class ObjectSubmitSystem : public System<5, 3>
	{
	public:
		ObjectSubmitSystem(Registry<5, 3>& reg)
			: m_registry(reg)
		{
			setRequirements<MeshComponent, TransformComponent>();
		}
		void sub();//submit all of the meshes in Registry that need to be drawn
	private:
		Registry<5, 3>& m_registry;
	};
}