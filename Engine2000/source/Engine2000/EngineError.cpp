#include "EngineError.h"

EngineError::EngineError() : msg(SDL_GetError()) {}

EngineError::EngineError(const std::string& message) : msg(message) {}

const char* EngineError::what() const noexcept
{
	return msg.c_str();
}
