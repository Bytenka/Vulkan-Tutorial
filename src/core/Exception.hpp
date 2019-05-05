#pragma once

#include <stdexcept>
#include <string>

class Exception : public std::runtime_error
{
  public:
    explicit Exception(const char* message) : std::runtime_error(message)
    {
    }

    explicit Exception(const std::string& message) : std::runtime_error(message)
    {
    }
};