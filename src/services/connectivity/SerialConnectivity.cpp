#include "SerialConnectivity.h"
#include "Log.h"
#include "Configuration.inc"
#include "ServiceHub.h"
#include "MessageTable.h"

#include <unistd.h>
#include <iostream>

SerialConnectivity::SerialConnectivity(const std::string & name) 
 : Service(name), mReceiveThread(nullptr), mIsNormalMode(true), mPrintPrefix(false),
 mTransmitThread(nullptr), mUart(std::make_unique<Uart>()) {
     
}

SerialConnectivity::~SerialConnectivity() {
    mUart->close();
}

void SerialConnectivity::init() {
    if (mUart->open(PORT_NAME, PORT_BAUDRATE) != -1) {
        LOG_INFO("%s ready to use with baudrate %d (Normal mode)", PORT_NAME, PORT_BAUDRATE);
        mIsNormalMode = true;
        
        mReceiveThread = std::unique_ptr<std::thread>
            (new std::thread(&SerialConnectivity::receive, this));
        mReceiveThread->detach();
        
        mTransmitThread = std::unique_ptr<std::thread>
            (new std::thread(&SerialConnectivity::transmit, this));
        mTransmitThread->detach();
    } else {
        LOG_ERROR("Fail to open %s", PORT_NAME);
    }
}

void SerialConnectivity::registerMessages() {
    // Register messages
    ServiceHub::getInstance()->registerMessage(MSG_TX_SERIAL, 
        std::dynamic_pointer_cast<Service>(shared_from_this()));
        
    ServiceHub::getInstance()->registerMessage(MSG_START_NORMAL_MODE, 
        std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void SerialConnectivity::receive() {
    struct timeval tvTimeout;
    fd_set fsRead;
    
    tvTimeout.tv_sec = 0;
    tvTimeout.tv_usec = 10000;
    
    uint8_t buff[FRAME_BUFFER_SIZE] = {0};
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
        
        mReceiveQueue.push(Message::obtain(buff, bytesRead));
        mCondition.notify_one();
        
        usleep(2000);
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
            if (msg->id == MSG_START_ENGINEERING_MODE) {
                LOG_INFO("Entering Engineering mode");
                mIsNormalMode = false;
                mUart->close();
            } else if (msg->id == MSG_START_UPDATE_MODE) {
                LOG_INFO("Entering Update mode");
                mIsNormalMode = false;
                mUart->close();
            } else {
                mIsNormalMode = true;
            }
            
            sendToHub(msg);
            mReceiveQueue.pop();   
        }
        
        if (!mTransmitQueue.empty()) {
            uint8_t buff[FRAME_BUFFER_SIZE] = {0};
            uint32_t len = 0;
            
            std::shared_ptr<Message> msg = mTransmitQueue.front();
            msg->getRaw(buff, len);
            
            if (len > 0) mUart->write(buff, len);
            mTransmitQueue.pop();
        }
        
        usleep(2000);
    }
}

void SerialConnectivity::handleMessage(std::shared_ptr<Message> &message) {
    switch (message->id) {
        case MSG_START_NORMAL_MODE:
            init();
            break;
            
        case MSG_TX_SERIAL:
            mTransmitQueue.push(message);
            mCondition.notify_one();
            break;
            
        default:
            break;
    }
}