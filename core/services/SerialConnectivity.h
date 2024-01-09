#pragma once

#include "Service.h"
#include "Uart.h"
#include "Message.h"

#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>

class SerialConnectivity final : public Service {
public:
  SerialConnectivity(const std::string& name, ServiceHub* hub);
  virtual ~SerialConnectivity();

private:
  void init() override;
  void registerMessage() override;
  void handleMessage(std::shared_ptr<Message>& message) override;
  void receive();
  void transmit();
    
private:
  std::unique_ptr<Uart> mUart;
  std::unique_ptr<std::thread> mReceiveThread;
  std::unique_ptr<std::thread> mTransmitThread;
  std::condition_variable mCondition;
  std::queue<std::shared_ptr<Message>> mReceiveQueue;
  std::queue<std::shared_ptr<Message>> mTransmitQueue;
  mutable std::mutex mLock;
  bool mIsNormalMode;
  bool mPrintPrefix;
};