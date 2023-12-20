#pragma once

#include "Service.h"
#include "Uart.h"

constexpr char SOH = 0x01;  // Start of Header
constexpr char EOT = 0x04;  // End of Transmission
constexpr char ACK = 0x06;  // Acknowledgment
constexpr char NAK = 0x15;  // Negative Acknowledgment

#define PACKET_SIZE 128

struct XmodemPacket {
    char header;         // SOH (Start of Header)
    char blockNumber;    // Block number (1 to 255)
    char blockNumberC;   // Block number complement
    char data[PACKET_SIZE]; // Data payload
    char crc;            // CRC checksum
};


class UpdateEngine final : public Service {
public:
    UpdateEngine(const std::string &name);
    virtual ~UpdateEngine();

private:
    void init() override;
    void registerMessages() override;
    void handleMessage(std::shared_ptr<Message> &message) override;
    
    void startUpdateMode();
    void receive();
    char calculateChecksum(const XmodemPacket &packet);
    
private:
    std::unique_ptr<Uart> mUart;
    std::unique_ptr<std::thread> mThread;
};
