#include "Uart.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ioctl.h>

Uart::Uart() : mFd(-1) {
}

Uart::~Uart() {
  (void) close();
}

int32_t Uart::open(const char *dev, uint32_t speed) {
  uint32_t speed_arr[] = { B921600, B460800, B115200, B9600, B38400, B19200, B4800 };
  uint32_t name_arr[] = { 921600, 460800, 115200, 9600, 38400, 19200, 4800 };
  uint32_t baud_rate = 0;
  struct termios opt;
    
  mFd = ::open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if(mFd <= 0) return -1;
    
  /** get the port attr */
  if (tcgetattr(mFd, &opt) < 0) {
      (void) close();
      return -1;
  }

  /** set the port attr */
  opt.c_cflag |= (CLOCAL | CREAD);
  opt.c_cflag &= ~CSIZE;
  opt.c_cflag &= ~CRTSCTS;
  opt.c_cflag |= CS8;
  opt.c_cflag &= ~CSTOPB;
  opt.c_iflag &= ~(ICRNL | IXON);
  opt.c_iflag |= (IXANY | IGNBRK | IGNPAR);
  opt.c_oflag &= ~OPOST;
  opt.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE);

  /** set the port baud rate */
  for (uint32_t i = 0; i < sizeof(speed_arr) / sizeof(uint32_t); i++) {
      if (speed == name_arr[i]) {
          cfsetispeed(&opt, speed_arr[i]);
          cfsetospeed(&opt, speed_arr[i]);
          baud_rate = speed;
      }
  }

  if (!baud_rate) {
      (void) close();
      return -1;
  }

  tcflush(mFd, TCIFLUSH);

  /** update the port attr */
  if (tcsetattr(mFd, TCSANOW, &opt) < 0) {
      (void) close();
      return -1;
  }
    
  return mFd;
}

void Uart::close(void) {
  if(mFd > 0)
      ::close(mFd);
  mFd = -1;
}

int32_t Uart::read(void *pdata, int32_t len) {
  if((mFd <= 0) || (!pdata))
    return -1;
  
  return ::read(mFd, pdata, len);
}

int32_t Uart::write(void *pdata, int32_t len) {
  if((mFd <= 0) || (!pdata))
    return -1;
  return ::write(mFd, pdata, len);
}


int32_t Uart::write(const std::string &data) {
  if (data.empty()) return -1;
  
  for (int i = 0; i < data.length(); i++) {
    uint8_t tmp = static_cast<uint8_t>(data[i]);
    write(&tmp, 1);
  }
  
  return data.length();
}
