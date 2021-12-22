#include "Engpch.hpp"
#include "Component.hpp"

size_t generateComponentType()
{
	static size_t counter = size_t(0);
	return counter++;
}