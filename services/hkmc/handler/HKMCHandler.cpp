#include "HKMCHandler.h"
#include "Log.h"

extern "C" std::string __init__(ServiceHub *hub) {
  return DYNAMIC_REGISTER<HKMCHandler>("HKMCHandler", hub);
}

HKMCHandler::HKMCHandler(const std::string &name, ServiceHub *hub)
  : Service(name, hub) {
}

HKMCHandler::~HKMCHandler() {
}

void HKMCHandler::init() {
  
}

void HKMCHandler::registerMessage() {
  mServiceHub->registerMessage(MSG_SERIAL_REQUEST,
    std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void HKMCHandler::handleMessage(std::shared_ptr<Message> &message) {
  switch (message->id) {
    case MSG_SERIAL_REQUEST:
      {
        LOG_INFO("Receive test request");
      }
      break;
    
    default:
      break;
  }
}
