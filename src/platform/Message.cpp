#include "Message.h"

Message::Message()
    : id(MSG_UNDEFINED) {
}

Message::~Message() {
}

std::shared_ptr<Message> Message::obtain(const std::shared_ptr<Service> &sender, const message_id &id) {
    std::shared_ptr<Message> m = std::make_shared<Message>();
    m->id = id;
    m->sender = sender;
    return m;
}

std::shared_ptr<Message> Message::obtainAutoMapId(const std::shared_ptr<Service> &sender, const uint8_t *data, const uint32_t len) {
    std::shared_ptr<Message> m = std::make_shared<Message>();
    
    if (len >= STD_MIN_LEN) {
        m->id = static_cast<message_id>(((data[STD_MODULE1_OFF] << 8) |
            data[STD_MODULE2_OFF]) << 8 | data[STD_MODULE3_OFF]);
            
        int32_t dataLen = static_cast<int32_t>(data[STD_USR_LEN_LO_OFF] << 8) | 
            data[STD_USR_LEN_HI_OFF];
        
        if (dataLen > 0) {
            for (int i = 0; i < dataLen; i++) {
                m->append(&data[STD_USR_DATA_OFF + i], 1);
            }
        }
    }
    m->sender = sender;
    return m;
}

void Message::append(const std::string &data) {
    for (int i = 0; i < data.length(); i++) {
        bytes.push_back(static_cast<uint8_t>(data[i]));   
    }
}

void Message::append(const uint8_t *data, const uint32_t len) {
    for (int i = 0; i < len; ++i) {
        bytes.push_back(data[i]);   
    }
}

std::string Message::getString() {
    std::string ret = "";
    for (int i = 0; i < bytes.size(); i++) {
        ret += static_cast<uint8_t>(bytes[i]);   
    }
    return ret;
}

void Message::getRaw(uint8_t *buffer, uint32_t &len) {
    for (int i = 0; i < bytes.size(); i++) {
        buffer[i] = bytes[i];   
    }
    len = bytes.size();
}

void Message::clear() {
    bytes.clear();
}
