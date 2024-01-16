#include "ServiceHub.h"
#include "Log.h"
#include "ConfigStore.h"
#include "SerialConnectivity.h"
#include "Engineering.h"
#include "UpdateEngine.h"
#include "Message.h"
#include "Utils.h"

#include <unistd.h>
#include <algorithm>
#include <dlfcn.h>
#include <dirent.h>

typedef std::string (*CreateServicePtr)(ServiceHub*);
ServiceHub* ServiceHub::mInstance = nullptr;

ServiceHub::ServiceHub()
  : isRunning(true) {
}

ServiceHub::~ServiceHub() {
  isRunning = false;
  usleep(500);
  removeAll();
  mCondition.notify_one();
}

ServiceHub* ServiceHub::getInstance() {
  if (mInstance == nullptr)
    mInstance = new ServiceHub();
  return mInstance;
}

void ServiceHub::init() {
  isRunning = true;
  LOG_INFO("Loading default services...");
  loadDefaultServices();
  
  LOG_INFO("Loading dynamic services...");
  loadDynamicServices();
}

void ServiceHub::start() {
  while (isRunning) {
    std::unique_lock<std::mutex> lock(mLock);
    if (mMessageQueue.empty()) {
        mCondition.wait(lock);
    }
    
    std::shared_ptr<Message> msg = mMessageQueue.top();
    if (msg->timestamp > utils::time::uptimeMillis()) {
      usleep(500);
      continue;
    }
      
    if (mMessageRegistrantMap.find(msg->id) != mMessageRegistrantMap.end()) {
      for (auto service : mMessageRegistrantMap[msg->id]) {
        // Send to all registrants
        if (mServiceMap.find(service) != mServiceMap.end()) {
          LOG_INFO("Send %s message from %s to %s", getMessageName(msg->id).c_str(), msg->sender->getName().c_str(), service.c_str());
          mServiceMap[service]->receive(msg);
        } else {
          LOG_ERROR("Service %s was unplugged from message registrant map", service.c_str());
        }
      }
    } else {
      LOG_INFO("Message %d doesn't belong to any services -> aborted", msg->id);
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
    mServiceMap.erase(name);
    usleep(500);
  }
  
  if (mDynamicLibraryMap.find(name) != mDynamicLibraryMap.end()) {
    if (mDynamicLibraryMap[name] != nullptr) {
      LOG_INFO("Unloading %s service", name.c_str());
      dlclose(mDynamicLibraryMap[name]);
    }
    mDynamicLibraryMap.erase(name);
  }
}

void ServiceHub::removeAll() {
  for (auto service : mServiceMap) {
    remove(service.first);
  }
}

void ServiceHub::notify(const std::shared_ptr<Message> &message) {
  std::unique_lock<std::mutex> lock(mLock);
  
  mMessageQueue.push(message);
  mCondition.notify_one();
}

void ServiceHub::registerMessage(message_id id, const std::shared_ptr<Service> &service) {
  auto it = std::find(mMessageRegistrantMap[id].begin(), mMessageRegistrantMap[id].end(), service->getName());
  
  if (it == mMessageRegistrantMap[id].end()) {
    mMessageRegistrantMap[id].push_back(service->getName());
  }
}

void ServiceHub::loadDefaultServices() {
  std::shared_ptr<SerialConnectivity> serialService 
    = std::make_shared<SerialConnectivity>("SerialConnectivity", ServiceHub::getInstance());
  add(serialService);
  
  std::shared_ptr<Engineering> engineeringService
    = std::make_shared<Engineering>("Engineering", ServiceHub::getInstance());
  add(engineeringService);
  
  std::shared_ptr<UpdateEngine> updateEngineService
    = std::make_shared<UpdateEngine>("UpdateEngine", ServiceHub::getInstance());
  add(updateEngineService);
}

void ServiceHub::loadDynamicServices() {
  std::vector<std::string> fos;
  std::string directory(ConfigStore::getInstance()->getString("FO_DIR"));
  DIR* dir = opendir(directory.c_str());
  dirent* entry;

  if (dir == nullptr) {
    LOG_ERROR("Error opening %s directory", directory.c_str());
    return;
  }

  while ((entry = readdir(dir)) != nullptr) {
    if (entry->d_type == DT_REG) {  // Check if it is a regular file
      std::string filename = entry->d_name;
      size_t extPos = filename.find_last_of(".");
      if (extPos != std::string::npos && filename.substr(extPos) 
          == ConfigStore::getInstance()->getString("FO_EXTENSION")) {
          fos.push_back(directory + '/' + filename);
      }
    }
  }
  
  closedir(dir);

  for (auto s : fos) {
    void *library = dlopen(s.c_str(), RTLD_LAZY);

    if (!library) {
      LOG_ERROR("Failed to load the library: %s", dlerror());
    } else {
      CreateServicePtr initService = reinterpret_cast<CreateServicePtr>(dlsym(library, "__init__"));

      if (!initService) {
        LOG_ERROR("Failed to load the symbols: %s", dlerror());
        dlclose(library);
      } else {
        mDynamicLibraryMap[initService(ServiceHub::getInstance())] = library;
      }
    }
  }
}