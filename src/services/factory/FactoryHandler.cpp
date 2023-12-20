#include "FactoryHandler.h"
#include "ServiceHub.h"
#include "Log.h"

FactoryHandler::FactoryHandler(const std::string &name) 
 : Service(name) {
    
}

FactoryHandler::~FactoryHandler() {
    
}

void FactoryHandler::init() {
    
}

void FactoryHandler::registerMessages() {
    ServiceHub::getInstance()->registerMessage(MSG_RX_SERIAL,
        std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void FactoryHandler::handleMessage(std::shared_ptr<Message> &message) {
    switch (message->id) {
        case MSG_RX_SERIAL:
            // parse message
            LOG_INFO("Receive rx message from UART");
            break;
            
        default:
            break;
    }
}