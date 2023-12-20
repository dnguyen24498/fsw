#pragma once

#include "Service.h"

class FactoryHandler final : public Service {
public:
    FactoryHandler(const std::string &name);
    virtual ~FactoryHandler();
    
private:
    void init() override;
    void registerMessages() override;
    void handleMessage(std::shared_ptr<Message> &message) override;
};