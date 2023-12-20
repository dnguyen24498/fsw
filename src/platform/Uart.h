#pragma once

#include "Configuration.inc"

#include <stdint.h>
#include <string>
#include <time.h>
#include <map>
#include <deque>
#include <termios.h>
#include <semaphore.h>

class Uart
{
public:
    Uart();
    virtual ~Uart();

public:
    int32_t     open(const char *dev_name, uint32_t speed);
    void        close(void);
    int32_t     read(void *pdata, int32_t len);
    int32_t     write(void *pdata, int32_t len);
    int32_t     write(const std::string &data);
    
    int32_t     getFd(void) { return mFd; }

private:
    int32_t mFd;
};
