#pragma once

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

enum message_id {
  MSG_TEST_REQUEST = 0,
  MSG_TEST_REPONSE,
  MSG_START_NORMAL_MODE = 16777213,       /* Enter normal mode */
  MSG_START_ENGINEERING_MODE = 16777214,  /* Enter engineering mode */
  MSG_START_UPDATE_MODE = 16777215,       /* Enter update mode */
  MSG_UNDEFINED
};