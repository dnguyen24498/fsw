#include "Version.h"
#include "Log.h"
#include "ServiceHub.h"
#include "SerialConnectivity.h"
#include "FactoryHandler.h"
#include "UpdateEngine.h"
#include "Engineering.h"

#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <sys/resource.h>
#include <execinfo.h>
#include <csignal>

void init() {
    std::shared_ptr<SerialConnectivity> mSerialConnectivity = std::make_shared<SerialConnectivity>("SerialConnectivity");
    mSerialConnectivity->plug();
    
    std::shared_ptr<FactoryHandler> mFactoryHandler = std::make_shared<FactoryHandler>("FactoryHandler");
    mFactoryHandler->plug();
    
    std::shared_ptr<UpdateEngine> mUpdateEngine = std::make_shared<UpdateEngine>("UpdateEngine");
    mUpdateEngine->plug();
    
    std::shared_ptr<Engineering> mEngineering = std::make_shared<Engineering>("Engineering");
    mEngineering->plug();
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

int main() {
    #if defined(USE_DLT)
        Log::getInstance()->registerService(std::make_shared<Logger>());
    #endif

    LOG_INFO("Version: %s", PROJECT_VERSION_STRING);
    
    init();
    
    registerSignal();
    
    ServiceHub::getInstance()->start();
    
    return 0;
}