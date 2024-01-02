#include "Version.h"
#include "Log.h"
#include "ServiceHub.h"
#include "Configuration.h"

#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/resource.h>
#include <execinfo.h>

#include <iostream>
#include <iomanip>
#include <csignal>
#include <vector>

typedef void (*CreateServicePtr)(ServiceHub*);
typedef void (*DestroyServicePtr)();


void init() {
    std::vector<std::string> fos;
    std::string directory(FO_DIRECTORY);
    
    DIR* dir = opendir(directory.c_str());
    
    if (dir == nullptr) {
        LOG_ERROR("Error opening %s directory", directory.c_str());
        return;
    }
    
    dirent* entry;
    
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) {  // Check if it is a regular file
            std::string filename = entry->d_name;
            size_t extPos = filename.find_last_of(".");
            if (extPos != std::string::npos && filename.substr(extPos) == FO_EXTENSION) {
                fos.push_back(directory + '/' + filename);
            }
        }
    }
    closedir(dir);
    
    for (auto s : fos) {
        void *library = dlopen(s.c_str(), RTLD_LAZY);
        
        if (!library) {
            LOG_ERROR("Failed to load the library: %s", dlerror());
        } else {
            CreateServicePtr createService = reinterpret_cast<CreateServicePtr>(dlsym(library, "__init__"));
            
            if (!createService) {
                LOG_ERROR("Failed to load the symbols: %s", dlerror());
                dlclose(library);
            } else {
                createService(ServiceHub::getInstance());
            }
        }
    }
}

void registerSignal(void) {
    for (int i = 1; i <= NSIG; i++)
        signal(i, [](int signum) {
            LOG_ERROR("Signal received: %d - %s", signum, strsignal(signum));
            if (signum != SIGINT && signum != SIGCHLD) {
                LOG_ERROR("========================= Crashed here =========================");
            }
            
            signal(signum, SIG_DFL);
            raise(signum);   
    });
}

void enableCoreDump() {
    struct rlimit core_limit;
    core_limit.rlim_cur = RLIM_INFINITY;
    core_limit.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &core_limit);
}

int main() {
    #if defined(USE_DLT)
        Log::getInstance()->registerService(std::make_shared<Logger>());
    #endif
 
    LOG_INFO("Version: %s", PROJECT_VERSION_STRING);
    
    registerSignal();
    enableCoreDump();
    
    init();
    
    ServiceHub::getInstance()->start();
    
    return 0;
}