#pragma once

#ifdef E2000_PLATFORM_WINDOWS
	#ifdef E2000_BUILD_DLL
		#define ENGINE2000_API __declspec(dllexport)
	#else
		#define ENGINE2000_API __declspec(dllimport)
	#endif
#else
	#error Engine2000 only supports Windows!
#endif
