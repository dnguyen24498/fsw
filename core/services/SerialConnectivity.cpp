#include "SerialConnectivity.h"
#include "Log.h"
#include "ConfigStore.h"
#include "ServiceHub.h"
#include "MessageMap.h"

#include <unistd.h>
#include <iostream>

SerialConnectivity::SerialConnectivity(const std::string &name, ServiceHub *hub) 
 : Service(name, hub), mReceiveThread(nullptr), mIsNormalMode(true), mPrintPrefix(false),
 mTransmitThread(nullptr), mUart(std::make_unique<Uart>()) {
	 
}

SerialConnectivity::~SerialConnectivity() {
	mUart->close();
	mIsNormalMode = false;
	mCondition.notify_one();
}

void SerialConnectivity::init() {
  if (mUart->open(ConfigStore::getInstance()->getString("PORT_NAME").c_str(), 
      ConfigStore::getInstance()->getInt("NORMAL_PORT_BAUDRATE")) != -1) {
		LOG_INFO("Normal port opnened");
		mIsNormalMode = true;
			
		mReceiveThread = std::unique_ptr<std::thread>
			(new std::thread(&SerialConnectivity::receive, this));
		mReceiveThread->detach();
		
		mTransmitThread = std::unique_ptr<std::thread>
			(new std::thread(&SerialConnectivity::transmit, this));
		mTransmitThread->detach();
	 } else {
		LOG_ERROR("Fail to open normal port");
	 }
}

void SerialConnectivity::registerMessage() {
	// Register messages
	mServiceHub->registerMessage(MSG_SERIAL_REPONSE, 
    std::dynamic_pointer_cast<Service>(shared_from_this()));		
	mServiceHub->registerMessage(MSG_START_NORMAL_MODE, 
    std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void SerialConnectivity::receive() {
	 struct timeval tvTimeout;
	 fd_set fsRead;
	 
	 tvTimeout.tv_sec = 0;
	 tvTimeout.tv_usec = 2000;
	 
	 uint8_t buff[ConfigStore::getInstance()->getInt("FRAME_BUFFER_SIZE")] = {0};
	 bool dataAvailable = false;
	 
	 while (mIsNormalMode) {
			if (!dataAvailable) {
				FD_ZERO(&fsRead);
				FD_SET(mUart->getFd(), &fsRead);
				
				int32_t ret = select(mUart->getFd() + 1, &fsRead, 
					 nullptr, nullptr, &tvTimeout);
				
				if (ret == -1) break;
				else if (ret == 0) continue;
				else dataAvailable = true;
			}
			
			ssize_t bytesRead = read(mUart->getFd(), buff, sizeof(buff));
			
			if (bytesRead < 1) continue;
			
			mReceiveQueue.push(Message::obtain(shared_from_this(), MSG_SERIAL_REQUEST, buff, bytesRead));
			mCondition.notify_one();
			
			dataAvailable = false;
	 }
}

void SerialConnectivity::transmit() {
	 while (mIsNormalMode) {
			std::unique_lock<std::mutex> lock(mLock);
			if (mReceiveQueue.empty() && mTransmitQueue.empty())
				mCondition.wait(lock);
				
			if (!mReceiveQueue.empty()) {
				std::shared_ptr<Message> msg = mReceiveQueue.front();
        if (msg->bytes.size() == 3) {
          if (msg->bytes[0] == 0xFF && msg->bytes[1] == 0xFF &&  msg->bytes[2] == 0xFE) {
            LOG_INFO("Entering Engineering mode");
            msg->id = MSG_START_ENGINEERING_MODE;
            mIsNormalMode = false;
            mUart->close();
          } else if (msg->bytes[0] == 0xFF && msg->bytes[1] == 0xFF && msg->bytes[2] == 0xFF) {
            LOG_INFO("Entering Update mode");
            msg->id = MSG_START_UPDATE_MODE;
            mIsNormalMode = false;
            mUart->close();
          } else {
            LOG_ERROR("Unknown mode");
          }
        }
        
				sendToHub(msg);
				mReceiveQueue.pop();
			}
			
			if (!mTransmitQueue.empty()) {
				uint8_t buff[ConfigStore::getInstance()->getInt("FRAME_BUFFER_SIZE")] = {0};
				uint32_t len = 0;
				
				mTransmitQueue.front()->getRaw(buff, len);
				if (len > 0) mUart->write(buff, len);
				mTransmitQueue.pop();
			}
	 }
}

void SerialConnectivity::handleMessage(std::shared_ptr<Message> &message) {
	 switch (message->id) {
			case MSG_START_NORMAL_MODE:
				init();
				break;
				
			case MSG_SERIAL_REPONSE:
				mTransmitQueue.push(message);
				mCondition.notify_one();
				break;
				
			default:
				break;
	 }
}