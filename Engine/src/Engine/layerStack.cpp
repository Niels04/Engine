#include "Engpch.hpp"
#include "layerStack.hpp"

namespace Engine
{
	layerStack::layerStack()
	{
		
	}

	layerStack::~layerStack()
	{
		for (layer* layer : m_layers)
		{
			delete layer;
		}
	}

	void layerStack::pushLayer(layer* layer)
	{
		m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
		m_layerInsertIndex++;
	}

	void layerStack::pushOverlay(layer* overlay)//we do it this way, because overlays should always be handled in the end, so we make sure theire at the end
	{
		m_layers.emplace_back(overlay);
	}

	void layerStack::popLayer(layer* layer)//remember: even if a layer is removed from the layerstack, it doesn't get dealocated until the layerStack is destroyed
	{
		auto it = std::find(m_layers.begin(), m_layers.end(), layer);
		if (it != m_layers.end())
		{
			m_layers.erase(it);
			m_layerInsertIndex--;
		}
	}

	void layerStack::popOverlay(layer* overlay)//remember: even if a layer is removed from the layerstack, it doesn't get dealocated until the layerStack is destroyed
	{
		auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
		if (it != m_layers.end())
			m_layers.erase(it);
	}
}