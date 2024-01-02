#include "Log.h"
#include "Dynamic.h"
#include "ServiceHub.h"
#include "Message.h"
#include "Configuration.h"

extern "C" void __init__(ServiceHub *hub) {
    std::shared_ptr<Dynamic> dynamic = std::make_shared<Dynamic>("Dynamic", hub);
    dynamic->plug();
}

Dynamic::Dynamic(const std::string &name, ServiceHub *hub)
    : Service(name, hub) {
        
}

Dynamic::~Dynamic() {
    
}

void Dynamic::init() {
    
}

void Dynamic::registerMessage() {
}

void Dynamic::handleMessage(std::shared_ptr<Message> &message) {
    switch (message->id) {
        default:
            break;
    }
}