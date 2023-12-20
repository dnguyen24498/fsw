#include "UpdateEngine.h"
#include "ServiceHub.h"
#include "Log.h"

UpdateEngine::UpdateEngine(const std::string &name)
 : Service(name) {
 }
 
UpdateEngine::~UpdateEngine() {
    
}

void UpdateEngine::init() {
    
}

void UpdateEngine::registerMessages() {
    ServiceHub::getInstance()->registerMessage(MSG_OTA_UPDATE,
        std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void UpdateEngine::handleMessage(std::shared_ptr<Message> &message) {
    switch (message->id) {
        case MSG_OTA_UPDATE:
            //do update
            LOG_INFO("Perform OTA update");
            break;
            
        default:
            break;
    }
}