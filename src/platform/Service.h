#pragma once

#include "ServiceHub.h"
#include "Message.h"

#include <thread>
#include <memory>
#include <string>
#include <condition_variable>
#include <queue>

class SerivceHub;

class Service : public std::enable_shared_from_this<Service> {
public:
    Service(const std::string &name);
    virtual ~Service();
    
    int32_t plug();
    std::string getName();
    int32_t getId();
    
protected:
    virtual void handleMessage(std::shared_ptr<Message> &message);
    void receive(std::shared_ptr<Message> &message);
    virtual void init();
    int32_t run();
    void destroy();
    void sendToHub(std::shared_ptr<Message> &message);
    virtual void loop();
    virtual void registerMessages();
    
protected:
    std::unique_ptr<std::thread> mThread;
    std::string mName;
    volatile bool always;
    mutable std::mutex mLock;
    std::condition_variable mCondition;
    std::queue<std::shared_ptr<Message>> mMessageQueue;

    friend class ServiceHub;
};