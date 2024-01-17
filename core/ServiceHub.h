#pragma once

#include "MessageMap.h"
#include "Service.h"

#include <memory>
#include <unordered_map>
#include <string>
#include <queue>
#include <thread>
#include <condition_variable>
#include <vector>

class Service;
class Message;

class ServiceHub final {
public:
  ServiceHub(ServiceHub &other) = delete;
  void operator=(const ServiceHub &) = delete;
    
  static ServiceHub* getInstance();

  void init();
  void start();
  void add(const std::shared_ptr<Service> &service);
  void remove(const std::string &name);
  void removeAll();
  void notify(const std::shared_ptr<Message> &message);
  void subscribeMessage(message_id id, const std::shared_ptr<Service> &service);

private:
  ServiceHub();
  virtual ~ServiceHub();
  void monitor();
  void loadDefaultServices();
  void loadDynamicServices();
    
private:
  static ServiceHub* mInstance;
  mutable std::mutex mLock;
  std::unique_ptr<std::thread> mMonitoringThread;
  std::condition_variable mCondition;
  std::unordered_map<std::string, std::shared_ptr<Service>> mServiceMap;
  std::unordered_map<int32_t, std::vector<std::string>> mMessageRegistrantMap;
  std::unordered_map<std::string, void*> mDynamicLibraryMap;
  std::priority_queue<std::shared_ptr<Message>> mMessageQueue;
  volatile bool isRunning;
};
