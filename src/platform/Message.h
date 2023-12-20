#pragma once

#include "MessageTable.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

#define STD_SOP1                0x4B
#define STD_SOP2                0x55
#define STD_RESERVED1           0x00
#define STD_RESERVED2           0x00
#define STD_DEBUG               0x00
#define STD_EOP                 0x7E

#define STD_SOP1_OFF            0
#define STD_SOP2_OFF            1
#define STD_RESERVED_HI_OFF     2
#define STD_RESERVED_LO_OFF     3
#define STD_LEN_HI_OFF          4
#define STD_LEN_LO_OFF          5
#define STD_CMD_TYPE_OFF        6
#define STD_DEBUG_OFF           7
#define STD_MODULE1_OFF         8
#define STD_MODULE2_OFF         9
#define STD_MODULE3_OFF         10
#define STD_RESULT_OFF          11
#define STD_USR_LEN_HI_OFF      12
#define STD_USR_LEN_LO_OFF      13
#define STD_USR_DATA_OFF        14

#define STD_MIN_LEN             17

class Message {
public:
    Message();
    virtual ~Message();
    
    static std::shared_ptr<Message> obtain(uint8_t *data, uint32_t len);
    static std::shared_ptr<Message> obtain(const message_id &id, const std::string &data);
    
    void append(uint8_t *data, uint32_t len);
    void append(const std::string &data);
    std::string getString();
    void getRaw(uint8_t *buffer, uint32_t &len);
    void clear();
public:
    message_id id;
    std::vector<uint8_t> bytes;
};
