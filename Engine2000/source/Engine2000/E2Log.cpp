#include "E2Log.h"
#include <windows.h>

void E2Log::setTextColor(Level level) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (level) {
	case Log:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case Warning:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case Error:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		break;
	}
}

void E2Log::resetTextColor() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}