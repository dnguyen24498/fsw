#pragma once

#include "ServiceHub.h"
#include "Message.h"

#include <thread>
#include <memory>
#include <string>
#include <condition_variable>
#include <queue>

class ServiceHub;

class Service : public std::enable_shared_from_this<Service> {
public:
    Service(const std::string &name, ServiceHub* hub);
    virtual ~Service();
    
    int32_t plug();
    int32_t getId();
    std::string getName();
    
protected:  
    int32_t run();
    void destroy();
    void sendToHub(std::shared_ptr<Message> &message);
    void receive(std::shared_ptr<Message> &message);
    virtual void init();
    virtual void loop();
    virtual void registerMessage();
    virtual void handleMessage(std::shared_ptr<Message> &message);
    
protected:
    std::string mName;
    volatile bool always;
    mutable std::mutex mLock;
    std::condition_variable mCondition;
    std::unique_ptr<std::thread> mThread;
    std::queue<std::shared_ptr<Message>> mMessageQueue;
    ServiceHub* mServiceHub;
    
    friend class ServiceHub;
};