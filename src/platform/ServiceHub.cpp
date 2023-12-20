#include "ServiceHub.h"
#include "Log.h"

#include <unistd.h>
#include <algorithm>

ServiceHub* ServiceHub::mInstance = nullptr;

ServiceHub::ServiceHub() {
}

ServiceHub::~ServiceHub() {
}

ServiceHub* ServiceHub::getInstance() {
    if (mInstance == nullptr)
        mInstance = new ServiceHub();
    return mInstance;
}

void ServiceHub::start() {
    for (;;) {
        std::unique_lock<std::mutex> lock(mLock);
        if (mMessageQueue.empty()) {
            mCondition.wait(lock);
        }
        
        std::shared_ptr<Message> msg = mMessageQueue.front();
        LOG_INFO("Broadcast message %d", msg->id);
        mMessageQueue.pop();
        
        this->broadcast(msg);
    }
}

void ServiceHub::add(const std::shared_ptr<Service> &service) {
    if (mServiceMap.find(service->getName()) == mServiceMap.end()) {
        LOG_INFO("%s service was plugged", service->getName().c_str());
        mServiceMap[service->getName()] = service;
        mServiceMap[service->getName()]->init();
        mServiceMap[service->getName()]->registerMessages();
        mServiceMap[service->getName()]->run();
    }
}

void ServiceHub::remove(const std::string &name) {
    if (mServiceMap.find(name) != mServiceMap.end()) {
        LOG_INFO("%s service was unplugged", name.c_str());
        mServiceMap[name]->destroy();
        mServiceMap.erase(name);
    }
}

void ServiceHub::notify(std::shared_ptr<Message> &message) {
    std::unique_lock<std::mutex> lock(mLock);
    mMessageQueue.push(message);
    mCondition.notify_one();
}

void ServiceHub::registerMessage(message_id id, const std::shared_ptr<Service> &service) {
    auto it = std::find(mMessageRegistrant[id].begin(), mMessageRegistrant[id].end(), service->getName());
    if (it == mMessageRegistrant[id].end())
        mMessageRegistrant[id].push_back(service->getName());
}

void ServiceHub::broadcast(std::shared_ptr<Message> &message) {
    if (mMessageRegistrant.find(message->id) != mMessageRegistrant.end()) {
        for (auto s : mMessageRegistrant[message->id]) {
            // Send to all registrants
            if (mServiceMap.find(s) != mServiceMap.end()) {
                mServiceMap[s]->receive(message);
            } else {
                LOG_ERROR("Message doesn't belong to any targets -> aborted");
            }
        }
    }
}
