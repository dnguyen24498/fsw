#include "Engineering.h"
#include "ServiceHub.h"
#include "Log.h"
#include "ConfigStore.h"
#include "Utils.h"

#include <unistd.h>
#include <iostream>

Engineering::Engineering(const std::string &name, ServiceHub *hub)
  : Service(name, hub), mUart(std::make_unique<Uart>()), 
  mEngineering(false), mPrintPrefix(false), mThread(nullptr) {
    
}

Engineering::~Engineering() {
  mEngineering = false;
  mUart->close();
}

void Engineering::init() {
    
}

void Engineering::subscribeMessage() {
  mServiceHub->subscribeMessage(MSG_START_ENGINEERING_MODE,
    std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void Engineering::startEngineeringMode() {
  if (mUart->open(ConfigStore::getInstance()->getString("PORT_NAME").c_str(), 
    ConfigStore::getInstance()->getInt("ENGINEERING_PORT_BAUDRATE")) != -1) {
      LOG_INFO("Engineering port opened");
      mEngineering = true;
      
      mThread = std::unique_ptr<std::thread>
          (new std::thread(&Engineering::receive, this));
      mThread->detach();
  } else {
      LOG_ERROR("Fail to open engineering port");
  }
}

void Engineering::receive() {
  struct timeval tvTimeout;
  fd_set fsRead;

  tvTimeout.tv_sec = 0;
  tvTimeout.tv_usec = 2000;

  uint8_t buff[ConfigStore::getInstance()->getInt("FRAME_BUFFER_SIZE")] = {0};
  bool dataAvailable = false;
  
  mUart->write("\n\r***************************************************************\n\r");
  mUart->write("*             If you see this message, it means               *\n\r");
  mUart->write("*             that you are in engineering mode                *\n\r");
  mUart->write("***************************************************************\n\r");
  
  while (mEngineering) {
    if (!mPrintPrefix) {
      mUart->write("fsw@root: ");
      mPrintPrefix = true;
    }
    
    if (!dataAvailable) {
      FD_ZERO(&fsRead);
      FD_SET(mUart->getFd(), &fsRead);
      
      int32_t ret = select(mUart->getFd() + 1, &fsRead, 
          nullptr, nullptr, &tvTimeout);
      if (!mEngineering) return;
      if (ret == -1) break;
      else if (ret == 0) continue;
      else dataAvailable = true;
    }
    
    ssize_t bytesRead = read(mUart->getFd(), buff, sizeof(buff));
    
    if (bytesRead < 1) continue;
    
    if (buff[0] != 0x08 && buff[0] != 0x09) {
      mEngineeringBuffer.push_back(buff[0]);
      mUart->write(&buff[0], 1);   
    }
            
    // Backspace
    if (buff[0] == 0x08 && mEngineeringBuffer.size() > 0) {
      uint8_t blank = 0x20;
      uint8_t backspace = 0x08;
      
      mUart->write(&backspace, 1);
      mUart->write(&blank, 1);
      mUart->write(&backspace, 1);
      
      mEngineeringBuffer.pop_back();
    }
    
    // Enter
    if (buff[0] == 0x0D) {
      uint8_t newLine = 0x0A;
      mUart->write(&newLine, 1);
      
      std::string command = "";
      for (int i = 0; i < mEngineeringBuffer.size() - 1; i++)
          command += static_cast<char>(mEngineeringBuffer[i]);
      
      if (!command.empty()) executeCommand(command);
      
      mEngineeringBuffer.clear();
      mPrintPrefix = false;
    }
    dataAvailable = false;
  }
}

void Engineering::executeCommand(std::string &command) {
  // Handle special commands first
  if (command == "exit" || command == "quit") {
    LOG_INFO("Entering Normal mode");
    mEngineering = false;
    mUart->close();
    
    Message::obtain(shared_from_this(), MSG_START_NORMAL_MODE)->sendToHub();
    return;
  }

  if (command == "log") {
    command = "cat " + ConfigStore::getInstance()->getString("LOG_DIR");
  }

  LOG_INFO("Executing command: %s", command.c_str());
  
  for (auto s : utils::io::execute(command)) {
    uint8_t tmp = static_cast<uint8_t>(s);
    if (s == '\n') mUart->write("\r");
    mUart->write(&s, 1);
  }
}

void Engineering::handleMessage(std::shared_ptr<Message> &message) {
  switch (message->id) {
    case MSG_START_ENGINEERING_MODE:
      startEngineeringMode();
      break;
    
    case MSG_HEARTBEAT_CHECK:
      LOG_INFO("Heartbeating...");
      break;
    
    default:
      break;
  }
}