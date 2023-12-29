#include "File.h"

#include <iostream>
#include <fstream>
#include <cstring>

namespace utils {
    namespace file {
        int32_t write(const char* name, const char* data) {
            std::ofstream file(name, std::ios::app);
            
            if (!file.is_open()) return -1;
            
            file << data;
            file.close();
            return std::strlen(data);
        }
    }
}