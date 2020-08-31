#pragma once

#ifdef ENG_PLATFORM_WINDOWS
	#ifdef ENG_BUILD_DLL
		#define ENGINE_API _declspec(dllexport)
	#else
		#define ENGINE_API _declspec(dllimport)
	#endif
#else
	#error Engine only supports Windows!
#endif
