#include "Time.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace utils {
    namespace time {
        std::string now(void) {
            auto now = std::chrono::system_clock::now();
            auto currentTime = std::chrono::system_clock::to_time_t(now);
            auto nanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()) % std::chrono::seconds(1);
        
            std::stringstream ss;
            ss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
            ss << '.' << std::setfill('0') << std::setw(3) << nanoSeconds.count() / 1000000;
        
            return ss.str();
        }
    }
}