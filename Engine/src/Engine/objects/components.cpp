#include "Engpch.hpp"
#include "components.hpp"

namespace Engine
{
	void TransformComponent::setShear(const float xy, const float xz, const float yx, const float yz, const float zx, const float zy)
	{
		m_shear[0] = xy; m_shear[1] = xz; m_shear[2] = yx; m_shear[3] = yz;
		m_shear[4] = zx; m_shear[5] = zy; recalcLocalTransform();
	}
}