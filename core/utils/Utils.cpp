#include "Utils.h"
#include "ConfigStore.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <cstring>
#include <iomanip>

namespace utils {
  namespace io {
    int32_t write(const char* name, const char* data) {
      std::ofstream file(name, std::ios::app);

      if (!file.is_open()) return -1;

      file << data;
      file.close();
      return std::strlen(data);
    }
    
    std::string execute(const std::string& command) {
      int pipefd[2];
      if (pipe(pipefd) == -1) {
          return "Failed to create pipe!\n";
      }

      pid_t pid = fork();
      if (pid == -1) {
          return "Failed to fork!\n";
      }

      if (pid == 0) {
          // Child process
          close(pipefd[0]); // Close the read end of the pipe

          // Redirect stdout to the write end of the pipe
          if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
              return "dup2() failed!\n";
          }

          // Execute the command
          if (execlp("sh", "sh", "-c", command.c_str(), nullptr) == -1) {
              return "execlp() failed!\n";
          }
      } else {
          // Parent process
          close(pipefd[1]); // Close the write end of the pipe

          // Read the command output from the read end of the pipe
          int FRAME_BUFFER_SIZE = ConfigStore::getInstance()->getInt("FRAME_BUFFER_SIZE");
          char buffer[FRAME_BUFFER_SIZE];
          std::string output;

          ssize_t bytesRead;
          while ((bytesRead = read(pipefd[0], buffer, FRAME_BUFFER_SIZE)) > 0) {
              output += std::string(buffer, bytesRead);
          }

          close(pipefd[0]); // Close the read end of the pipe

          // Wait for the child process to finish
          int status;
          waitpid(pid, &status, 0);

          if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
              return "Command execution failed!\n";
          }
          return output;
        }
      }
    }

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

    uint64_t uptimeMillis() {
      auto now = std::chrono::system_clock::now();
      auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
      return dur.count();
    }
  }
}
