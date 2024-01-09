#pragma once

enum message_id {
  MSG_SERIAL_REQUEST                       = 0,
  MSG_SERIAL_REPONSE                       = 1,
  MSG_START_NORMAL_MODE                    = 16777213,    /* Enter normal mode */
  MSG_START_ENGINEERING_MODE               = 16777214,    /* Enter engineering mode */
  MSG_START_UPDATE_MODE                    = 16777215,    /* Enter update mode */
  MSG_UNDEFINED
};