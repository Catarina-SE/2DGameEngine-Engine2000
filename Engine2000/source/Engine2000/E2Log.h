#pragma once
#include "Core.h"
#include <stdio.h>

// Define log macro for Engine2000
#define E2_LOG(level, format, ...) E2Log::print(E2Log::level, format, ##__VA_ARGS__)

class ENGINE2000_API E2Log {
public:
	enum Level {
		Log,        // Normal log (white)
		Warning,    // Warning (yellow)
		Error       // Error (red)
	};

	template<typename... Args>
	static void print(Level level, const char* format, Args... args) {
		setTextColor(level);
		printf(format, args...);
		printf("\n");
		resetTextColor();
	}

private:
	static void setTextColor(Level level);
	static void resetTextColor();
};