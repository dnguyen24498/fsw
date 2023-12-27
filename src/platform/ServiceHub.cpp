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
        
        if (mMessageRegistrant.find(msg->id) != mMessageRegistrant.end()) {
            for (auto service : mMessageRegistrant[msg->id]) {
                // Send to all registrants
                if (mServiceMap.find(service) != mServiceMap.end()) {
                    LOG_INFO("Send message %d from %s to %s", msg->id, msg->sender->getName().c_str(), service.c_str());
                    mServiceMap[service]->receive(msg);
                } else {
                    LOG_ERROR("Message %d doesn't belong to any targets -> aborted", msg->id);
                }
            }
        }
        mMessageQueue.pop();
    }
}

void ServiceHub::add(const std::shared_ptr<Service> &service) {
    if (mServiceMap.find(service->getName()) == mServiceMap.end()) {
        LOG_INFO("%s service was plugged", service->getName().c_str());
        mServiceMap[service->getName()] = service;
        mServiceMap[service->getName()]->init();
        mServiceMap[service->getName()]->registerMessage();
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
