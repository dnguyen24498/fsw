#pragma once

#include "Service.h"
#include "Message.h"

#include <memory>
#include <unordered_map>
#include <string>
#include <queue>
#include <condition_variable>
#include <vector>

class Service;

class ServiceHub final {
public:
    ServiceHub(ServiceHub &other) = delete;
    void operator=(const ServiceHub &) = delete;
    
    static ServiceHub* getInstance();
    
    void start();
    void add(const std::shared_ptr<Service> &service);
    void remove(const std::string &name);
    void notify(std::shared_ptr<Message> &message);
    void registerMessage(message_id id, const std::shared_ptr<Service> &service);

private:
    ServiceHub();
    virtual ~ServiceHub();
    
    void broadcast(std::shared_ptr<Message> &message);
    
private:
    static ServiceHub* mInstance;
    mutable std::mutex mLock;
    std::condition_variable mCondition;
    std::unordered_map<std::string, std::shared_ptr<Service>> mServiceMap;
    std::unordered_map<int32_t, std::vector<std::string>> mMessageRegistrant;
    std::queue<std::shared_ptr<Message>> mMessageQueue;
};
