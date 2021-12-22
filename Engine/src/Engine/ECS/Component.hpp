#pragma once

template<typename T>
class Component
{
public:
	static const size_t type;
};

size_t generateComponentType();

template<typename T>
const size_t Component<T>::type = generateComponentType();