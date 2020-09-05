#pragma once

#include "core.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"//so we can log custom types

namespace Engine
{
	class ENGINE_API Log
	{
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_ClientLogger;
	};
}

//core log macros
#define ENG_CORE_TRACE(...)		::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENG_CORE_INFO(...)		::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENG_CORE_WARN(...)		::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENG_CORE_ERROR(...)		::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENG_CORE_FATAL(...)		::Engine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//client log macros
#define ENG_TRACE(...)			::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ENG_INFO(...)			::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define ENG_WARN(...)			::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ENG_ERROR(...)			::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define ENG_FATAL(...)			::Engine::Log::GetCoreLogger()->fatal(__VA_ARGS__)