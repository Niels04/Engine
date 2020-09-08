#pragma once

#include "Engpch.hpp"

#include "core.hpp"
#include "layer.hpp"

namespace Engine
{
	class ENGINE_API layerStack//really just a wrapup of the m_layers vector
	{
	public:
		layerStack();
		~layerStack();

		void pushLayer(layer* layer);
		void pushOverlay(layer* overlay);
		void popLayer(layer* layer);
		void popOverlay(layer* overlay);

		std::vector<layer*>::iterator begin() { return m_layers.begin(); };
		std::vector<layer*>::iterator end() { return m_layers.end(); };
	private:
		std::vector<layer*> m_layers;
		unsigned int m_layerInsertIndex = 0;
	};
}