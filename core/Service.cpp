#include "Service.h"
#include "Log.h"
#include "ServiceHub.h"

#include <dlfcn.h>
#include <unistd.h>

Service::Service(const std::string &name, ServiceHub* hub) 
	: mName(name), always(true), mThread(nullptr), mServiceHub(hub) {
	mThread = std::unique_ptr<std::thread>(new std::thread(&Service::loop, this));
}

Service::~Service() {  
  always = false;
  mCondition.notify_one();
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
}

int32_t Service::plug() {
	mServiceHub->add(std::dynamic_pointer_cast<Service>(shared_from_this()));
	return getId();
}

void Service::receive(const std::shared_ptr<Message> &message) {
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

void Service::sendToHub(const std::shared_ptr<Message> &message, uint64_t delay) {
  message->timestamp = utils::time::uptimeMillis() + delay;
  //LOG_INFO("Timestamp is: %d", message->timestamp);
	mServiceHub->notify(message);
}

void Service::loop() {
	while (always) {
    std::unique_lock<std::mutex> lock(mLock);
		if (mMessageQueue.empty())
			mCondition.wait(lock);
		if (!mMessageQueue.empty()) {
			std::shared_ptr<Message> msg = mMessageQueue.front();
			mMessageQueue.pop();

      try {
        handleMessage(msg);
      } catch(const std::exception& e) {
        LOG_ERROR("Exception: %s", e.what());
      }
		}
	}
}
