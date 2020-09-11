#pragma once

#ifdef ENG_PLATFORM_WINDOWS
#ifdef ENG_DYNAMIC_LINK
	#ifdef ENG_BUILD_DLL
		#define ENGINE_API _declspec(dllexport)
	#else
		#define ENGINE_API _declspec(dllimport)
	#endif
#else
	#define ENGINE_API
#endif
#else
	#error Engine only supports Windows!
#endif

#ifdef ENG_DEBUG
	#define ENG_ENABLE_ASSERTS
#endif

#ifdef ENG_ENABLE_ASSERTS
	#define ENG_ASSERT(x, ...) { if(!(x)) { ENG_ERROR("ASSERTION FAILED: {0}", __VA_ARGS__); __debugbreak(); } }
	#define ENG_CORE_ASSERT(x, ...) { if(!(x)) { ENG_CORE_ERROR("ASSERTION FAILED: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ENG_ASSERT(x, ...)
	#define ENG_CORE_ASSERT(x, ...)
#endif

//this means the value 1 gets bitshifted by whatever "x" is, so we can easily filter things using this
#define BIT(x) (1 << x)

#define ENG_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)