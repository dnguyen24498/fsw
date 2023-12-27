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

void FactoryHandler::registerMessage() {
    ServiceHub::getInstance()->registerMessage(MSG_TEST_REQUEST,
        std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void FactoryHandler::handleMessage(std::shared_ptr<Message> &message) {
    switch (message->id) {
        case MSG_TEST_REQUEST:
            // parse message
            LOG_INFO("Receive rx message from UART");
            break;
            
        default:
            break;
    }
}