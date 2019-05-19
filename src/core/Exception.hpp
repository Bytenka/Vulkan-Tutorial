#pragma once

#include <stdexcept>
#include <string>

class Exception : public std::runtime_error
{
  public:
    explicit Exception(const char* message, bool isFatal = false)
        : std::runtime_error(message), m_isFatal(isFatal)
    {
    }

    explicit Exception(const std::string& message, bool isFatal = false)
        : std::runtime_error(message), m_isFatal(isFatal)
    {
    }

    inline bool isFatal() const noexcept { return m_isFatal; }

  private:
    bool m_isFatal;
};