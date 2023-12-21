#pragma once

#include "MessageTable.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class Message {
public:
    Message();
    virtual ~Message();
    
    static std::shared_ptr<Message> obtain(const message_id &id);
    static std::shared_ptr<Message> obtain(const message_id &id, const std::string &data);
    
    static std::shared_ptr<Message> obtainAutoMapId(uint8_t *data, uint32_t len);
    
    void append(uint8_t *data, uint32_t len);
    void append(const std::string &data);
    std::string getString();
    void getRaw(uint8_t *buffer, uint32_t &len);
    void clear();
public:
    message_id id;
    std::vector<uint8_t> bytes;
};
