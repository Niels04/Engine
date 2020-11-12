#pragma once

namespace Engine
{
	struct timestep
	{
		timestep(const float inTime = 0.0f)
			: time(inTime)
		{

		}

		const float getSeconds() const { return time; }
		const float getMilliseconds() const { return time * 1000.0f; }

		//operators
		operator float() const { return time; }//gets used whenever we try to use the struct like a float
		//members
		float time;
	};

}