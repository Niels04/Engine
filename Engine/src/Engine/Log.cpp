#include "Engpch.hpp"

#include "Log.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Engine
{
	std::shared_ptr<spdlog::logger> Log::m_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::m_ClientLogger;

	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");//this pattern will color messages in the right way, they will have a timestamp and the name of the logger(and of course the accual message)
		m_CoreLogger = spdlog::stdout_color_mt("Engine");
		m_ClientLogger = spdlog::stdout_color_mt("APP");
		m_CoreLogger->set_level(spdlog::level::trace);
		m_ClientLogger->set_level(spdlog::level::trace);
	}
}