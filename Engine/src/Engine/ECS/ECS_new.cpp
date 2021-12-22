#include <iostream>

#include "Registry.hpp"

#define ArraySize(x) sizeof(x) / sizeof(x[0])

class Velocity : public Component<Velocity>
{
public:
	Velocity(const float X, const float Y, const float Z)
		: x(X), y(Y), z(Z)
	{

	}
	float x, y, z;
};

class Position : public Component<Position>
{
public:
	Position(const float X, const float Y, const float Z)
		: x(X), y(Y), z(Z)
	{

	}
	float x, y, z;
};

class Vertices : public Component<Vertices>
{
public:
	Vertices() = default;
	void* m_vertices;
};

constexpr size_t componentTypeCount = 3;
constexpr size_t systemCount = 1;

class PhysicsSystem : public System<componentTypeCount, systemCount>
{
public:
	PhysicsSystem(Registry<componentTypeCount, systemCount>& reg)
		: m_registry(reg)
	{
		setRequirements<Position, Velocity>();
	}

	void update(float ts)
	{
		for (const auto& e : getManagedEntities())
		{
			auto [pos, velocity] = m_registry.get<Position, Velocity>(e);
			pos.x += velocity.x * ts;
			pos.y += velocity.y * ts;
			pos.z += velocity.z * ts;
		}
	}
private:
	Registry<componentTypeCount, systemCount>& m_registry;
};

int main()
{
	std::cout << Velocity::type << " " << Position::type << " " << Vertices::type << std::endl;

	Registry<componentTypeCount, systemCount> registry;
	registry.registerComponent<Velocity>();
	registry.registerComponent<Position>();
	registry.registerComponent<Vertices>();
	PhysicsSystem* physicsSystem = registry.createSystem<PhysicsSystem>(registry);
	Entity ents[100000];
	for(uint32_t i = 0; i < ArraySize(ents); i++)
	{
		ents[i] = registry.create();
		registry.addComponent<Velocity>(ents[i], 10.0f, 20.0f, 4.0f);
		registry.addComponent<Position>(ents[i], 0.0f, 0.0f, 0.0f);
		registry.addComponent<Vertices>(ents[i]);
	}
	physicsSystem->update(1.0f);


	return 0;
}