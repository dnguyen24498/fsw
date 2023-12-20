#pragma once

#include "Service.h"

class UpdateEngine final : public Service {
public:
    UpdateEngine(const std::string &name);
    virtual ~UpdateEngine();

private:
    void init() override;
    void registerMessages() override;
    void handleMessage(std::shared_ptr<Message> &message) override;
};
