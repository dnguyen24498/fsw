#include "UpdateEngine.h"
#include "ServiceHub.h"
#include "Log.h"
#include "Configuration.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <termios.h>

UpdateEngine::UpdateEngine(const std::string &name)
 : Service(name), mUart(std::make_unique<Uart>()) {
 }
 
UpdateEngine::~UpdateEngine() {
    mUart->close();
}

void UpdateEngine::init() {
    
}

void UpdateEngine::registerMessage() {
    ServiceHub::getInstance()->registerMessage(MSG_START_UPDATE_MODE,
        std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void UpdateEngine::startUpdateMode() {
    if (mUart->open(PORT_NAME, UPDATE_PORT_BAUDRATE) != -1) {
        LOG_INFO("%s ready to use with baudrate %d (Update Mode)", 
            PORT_NAME, UPDATE_PORT_BAUDRATE);
        
        mThread = std::unique_ptr<std::thread>
            (new std::thread(&UpdateEngine::receive, this));
        mThread->detach();
    } else {
        LOG_ERROR("Fail to open %s", PORT_NAME);
    }
}


char UpdateEngine::calculateChecksum(const XmodemPacket &packet) {
    char crc = 0;
    crc += packet.blockNumber;
    crc += packet.blockNumberC;
    for (int i = 0; i < PACKET_SIZE; ++i) {
        crc += packet.data[i];
    }
    return crc;
}

void UpdateEngine::receive() {
    // Remove the old file if exsist
    std::remove("fsw_new");
    
    std::ofstream outputFile("fsw_new", std::ios::binary);
    
    if (!outputFile.is_open()) {
        LOG_INFO("Could not open output file");
        return;
    }
    
    struct timeval tvTimeout;
    fd_set fsRead;
    
    tvTimeout.tv_sec = 0;
    tvTimeout.tv_usec = 2000;
    
    bool dataAvailable = false;
    
    while (true) {
        if (!dataAvailable) {
            FD_ZERO(&fsRead);
            FD_SET(mUart->getFd(), &fsRead);
            
            int32_t ret = select(mUart->getFd() + 1, &fsRead, 
                nullptr, nullptr, &tvTimeout);
            
            if (ret == -1) break;
            else if (ret == 0) continue;
            else dataAvailable = true;
        }
        
        XmodemPacket packet;
        ssize_t bytesRead = read(mUart->getFd(), 
            reinterpret_cast<char*>(&packet), sizeof(XmodemPacket));
        
        if (bytesRead < 1) continue;
        
        if (packet.header == EOT) {
            // End of Transmision
            LOG_INFO("Update completed, reboot the device to take effect");
            
            // Replace binary
            std::remove("fsw");
            std::rename("fsw_new", "fsw");
            
            break;
        }
        
        char receivedCRC = calculateChecksum(packet);
        
        // Check if the received CRC matches the calculated CRC
        if (receivedCRC == packet.crc) {
            // Send acknowledgment to the transmitter
            write(mUart->getFd(), &ACK, 1);
            outputFile.write(packet.data, PACKET_SIZE);
            
            dataAvailable = false;
        } else {
            // Send negative acknowledgment to the transmitter
            LOG_ERROR("Wrong checksum");
            write(mUart->getFd(), &NAK, 1);
            break;
        }
        
        //usleep(2000);
    }
    
    outputFile.close();
    mUart->close();
    
    std::shared_ptr<Message> out = Message::obtain(shared_from_this(), MSG_START_NORMAL_MODE);
    sendToHub(out);
}

void UpdateEngine::handleMessage(std::shared_ptr<Message> &message) {
    switch (message->id) {
        case MSG_START_UPDATE_MODE:
            startUpdateMode();
            break;
            
        default:
            break;
    }
}