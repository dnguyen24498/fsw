#include "Service.h"
#include "Log.h"
#include "ServiceHub.h"

#include <unistd.h>

Service::Service(const std::string &name, ServiceHub* hub) 
    : mName(name), always(true), mThread(nullptr), mServiceHub(hub) {
    mThread = std::unique_ptr<std::thread>(new std::thread(&Service::loop, this));
}

Service::~Service() {
    destroy();
}

void Service::handleMessage(std::shared_ptr<Message> &message) {
    LOG_INFO("Processing message: %d", message->id);
}

int32_t Service::run() {
    always = true;
    mThread->detach();
    
    return getId();
}

void Service::destroy() {
    always = false;
    mCondition.notify_one();
    
    if (mThread->joinable())
        mThread->join();
}

int32_t Service::plug() {
    mServiceHub->add(std::dynamic_pointer_cast<Service>(shared_from_this()));
    return getId();
}

void Service::receive(std::shared_ptr<Message> &message) {
    std::unique_lock<std::mutex> lock(mLock);
    mMessageQueue.push(message);
    mCondition.notify_one();
}

void Service::init() {
    
}

void Service::registerMessage() {
    
}

std::string Service::getName() {
    return mName;
}

int32_t Service::getId() {
    std::hash<std::thread::id> hasher;
    return static_cast<int32_t>(hasher(mThread->get_id()));
}

void Service::sendToHub(std::shared_ptr<Message> &message) {
    mServiceHub->notify(message);
}

void Service::loop() {
    while (always) {
        std::unique_lock<std::mutex> lock(mLock);
        if (mMessageQueue.empty())
            mCondition.wait(lock);
        
        std::shared_ptr<Message> msg = mMessageQueue.front();
        mMessageQueue.pop();
        
        handleMessage(msg);
    }
}