#pragma once

#include "ServiceHub.h"
#include "Message.h"

#include <thread>
#include <memory>
#include <string>
#include <condition_variable>
#include <queue>

class Message;
class ServiceHub;

template <typename T>
std::string DYNAMIC_REGISTER(const std::string &name, ServiceHub *hub) {
  std::shared_ptr<T> service = std::make_shared<T>(name, hub);
  service->plug();
  return service->getName();
}

class Service : public std::enable_shared_from_this<Service> {
public:
  Service(const std::string &name, ServiceHub* hub);
  virtual ~Service();
  
  int32_t plug();
  int32_t getId();
  std::string getName();
  int32_t run();
  void destroy();
  void sendToHub(const std::shared_ptr<Message> &message, uint64_t delay = 0);
  void receive(const std::shared_ptr<Message> &message);
  virtual void init();
  virtual void loop();
  virtual void subscribeMessage();
  virtual void handleMessage(std::shared_ptr<Message> &message);

protected:
  ServiceHub* mServiceHub;

private:
  std::string mName;
  volatile bool always;
  mutable std::mutex mLock;
  std::condition_variable mCondition;
  std::unique_ptr<std::thread> mThread;
  std::queue<std::shared_ptr<Message>> mMessageQueue;

  friend class Message;
  friend class ServiceHub;
};