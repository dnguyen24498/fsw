#pragma once

#include <iostream>
#include <map>
#include <string>

enum message_id {
    MSG_SERIAL_REQUEST                       = 0,
    MSG_SERIAL_REPONSE                       = 1,
    MSG_START_NORMAL_MODE                    = 16777213,    /* Enter normal mode */
    MSG_START_ENGINEERING_MODE               = 16777214,    /* Enter engineering mode */
    MSG_START_UPDATE_MODE                    = 16777215,    /* Enter update mode */
    MSG_UNDEFINED
};

inline std::map<message_id, std::string> messageMap = {
    {MSG_SERIAL_REQUEST, "MSG_SERIAL_REQUEST"},
    {MSG_SERIAL_REPONSE, "MSG_SERIAL_REPONSE"},
    {MSG_START_NORMAL_MODE, "MSG_START_NORMAL_MODE"},
    {MSG_START_ENGINEERING_MODE, "MSG_START_ENGINEERING_MODE"},
    {MSG_START_UPDATE_MODE, "MSG_START_UPDATE_MODE"},
    {MSG_UNDEFINED, "MSG_UNDEFINED"}
};

inline std::string getMessageName(message_id value) {
    auto it = messageMap.find(value);
    if (it != messageMap.end()) {
        return it->second;
    } else {
        return "MSG_UNDEFINED";
    }
}