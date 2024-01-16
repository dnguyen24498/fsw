#include "Message.h"
#include "Utils.h"

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

std::shared_ptr<Message> Message::obtain(const std::shared_ptr<Service> &sender, const message_id &id, const uint8_t *data, const uint32_t &len) {
  std::shared_ptr<Message> m = std::make_shared<Message>();
  m->id = id;
  m->sender = sender;
  m->append(data, len);
  return m;
}

void Message::sendToHub(uint64_t delay) {
  sender->sendToHub(shared_from_this(), delay);
}

void Message::append(const std::string &data) {
  for (int i = 0; i < data.length(); i++) {
    bytes.push_back(static_cast<uint8_t>(data[i]));   
  }
}

void Message::append(const uint8_t *data, const uint32_t &len) {
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
