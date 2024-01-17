#pragma once

#include "Service.h"
#include "Uart.h"

#include <vector>

class Engineering final : public Service {
public:
  Engineering(const std::string &name, ServiceHub *hub);
  virtual ~Engineering();
private:
  void init() override;
  void subscribeMessage() override;
  void handleMessage(std::shared_ptr<Message> &message) override;
  
  void startEngineeringMode();
  void receive();
  
  void executeCommand(std::string &command);

private:
  std::unique_ptr<Uart> mUart;
  std::unique_ptr<std::thread> mThread;
  std::vector<uint8_t> mEngineeringBuffer;
  bool mEngineering;
  bool mPrintPrefix;
};