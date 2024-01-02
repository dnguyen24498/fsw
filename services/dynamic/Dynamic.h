#pragma once

#include "Service.h"

class Dynamic final : public Service {
public:
    Dynamic(const std::string &name, ServiceHub *hub);
    virtual ~Dynamic();
    
private:
    void init() override;
    void registerMessage() override;
    void handleMessage(std::shared_ptr<Message> &message) override;
};