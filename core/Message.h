#pragma once

#include "Service.h"
#include "Utils.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class Service;
class Message : public std::enable_shared_from_this<Message> {
public:
  Message();
  virtual ~Message();

  static std::shared_ptr<Message> obtain(const std::shared_ptr<Service> &sender, const message_id &id);
  static std::shared_ptr<Message> obtain(const std::shared_ptr<Service> &sender, const message_id &id, const uint8_t *data, const uint32_t &len);

  void append(const std::string &data);
  void append(const uint8_t *data, const uint32_t &len);

  std::string getString();
  void getRaw(uint8_t *buffer, uint32_t &len);

  void clear();
  void sendToHub(uint64_t delay = 0);

  bool operator<(const Message& other) const { return timestamp > other.timestamp; }
    
public:
  message_id id;
  std::vector<uint8_t> bytes;
  std::shared_ptr<Service> sender;
  uint64_t timestamp;
};
