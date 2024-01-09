#include "Log.h"
#include "ServiceHub.h"
#include "ConfigStore.h"

#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/resource.h>
#include <execinfo.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <cstdlib>
#include <cstdio>
#include <execinfo.h>
#include <iostream>
#include <iomanip>
#include <csignal>
#include <vector>

void registerSignal(void) {
  for (int i = 1; i <= NSIG; i++)
      signal(i, [](int signum) {
          LOG_ERROR("Signal received: %d - %s", signum, strsignal(signum));
          if (signum != SIGINT && signum != SIGCHLD) {
              LOG_ERROR("========================= Crashed here =========================");
              void* callstack[1024];
              int numFrames = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
              char** symbols = backtrace_symbols(callstack, numFrames);

              if (symbols != nullptr) {
                  for (int i = 0; i < numFrames; ++i) {
                      printf("%s\n", symbols[i]);
                  }
                  free(symbols);
              }
          }
          signal(signum, SIG_DFL);
          raise(signum);   
  });
}

void enableCoreDump() {
  struct rlimit corelim;

  corelim.rlim_cur = -1;
  corelim.rlim_max = -1;

  if (setrlimit (RLIMIT_CORE, &corelim) != 0)
  {
      LOG_ERROR("Couldn't set core limit");
  }
}

int main() {
#if defined(USE_DLT)
  Log::getInstance()->registerService(std::make_shared<Logger>());
#endif

  LOG_INFO("Version: %d.%d.%d", PROJECT_VERSION_MAJOR, 
      PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);

  registerSignal();
  enableCoreDump();
  
  ServiceHub::getInstance()->init();
  ServiceHub::getInstance()->start();
  
  return 0;
}