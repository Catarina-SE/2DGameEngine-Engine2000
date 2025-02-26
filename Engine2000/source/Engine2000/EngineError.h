#pragma once

#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>

class EngineError : public std::exception
{
protected:
	std::string msg;

public:
	EngineError(); // SDL Errors
	EngineError(const std::string& message); // Custom message
	virtual ~EngineError() noexcept = default;
	virtual const char* what() const noexcept override;
};
