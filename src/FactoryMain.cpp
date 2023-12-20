#include "Version.h"
#include "Log.h"
#include "ServiceHub.h"
#include "SerialConnectivity.h"
#include "FactoryHandler.h"
#include "UpdateEngine.h"
#include "Engineering.h"

#include <iostream>
#include <unistd.h>

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

int main() {
    LOG_INFO("Version: %s", PROJECT_VERSION_STRING);
    
    init();
    ServiceHub::getInstance()->start();
    
    return 0;
}