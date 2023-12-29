#pragma once

#include "MessageTable.h"
#include "Service.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class Service;

class Message {
public:
    Message();
    virtual ~Message();
    
    static std::shared_ptr<Message> obtain(const std::shared_ptr<Service> &sender, const message_id &id);
    static std::shared_ptr<Message> obtainAutoMapId(const std::shared_ptr<Service> &sender, const uint8_t *data, const uint32_t len);
    
    void append(const std::string &data);
    void append(const uint8_t *data, const uint32_t len);
    
    std::string getString();
    void getRaw(uint8_t *buffer, uint32_t &len);
    
    void clear();
    
public:
    message_id id;
    std::vector<uint8_t> bytes;
    std::shared_ptr<Service> sender;
};
