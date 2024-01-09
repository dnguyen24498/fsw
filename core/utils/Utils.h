#pragma once

#include <cstdint>
#include <iostream>
#include <string>

namespace utils {
  namespace io {
    int32_t write(const char* name, const char* data);
    std::string execute(const std::string& command);
  }

  namespace time {
    std::string now(void);
  }
}