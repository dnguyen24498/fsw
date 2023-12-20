#pragma once

enum message_id {
    MSG_RX_SERIAL = 0,
    MSG_TX_SERIAL,
    MSG_START_NORMAL_MODE = 16777213, /* Enter normal mode */
    MSG_START_ENGINEERING_MODE = 16777214, /* Enter engineering mode */
    MSG_OTA_UPDATE  = 16777215, /* 0xFFFFFF */
    MSG_UNDEFINED
};