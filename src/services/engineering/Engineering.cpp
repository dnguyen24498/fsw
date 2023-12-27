#include "Engineering.h"
#include "ServiceHub.h"
#include "Log.h"
#include "Configuration.inc"

#include <unistd.h>
#include <iostream>

Engineering::Engineering(const std::string &name)
    : Service(name), mUart(std::make_unique<Uart>()), 
    mEngineering(false), mPrintPrefix(false), mThread(nullptr) {
    
}

Engineering::~Engineering() {   
    mUart->close();
}

void Engineering::init() {
    
}

void Engineering::registerMessage() {
    ServiceHub::getInstance()->registerMessage(MSG_START_ENGINEERING_MODE,
        std::dynamic_pointer_cast<Service>(shared_from_this()));
}

void Engineering::startEngineeringMode() {
    if (mUart->open(PORT_NAME, ENGINEERING_PORT_BAUDRATE) != -1) {
        LOG_INFO("%s ready to use with baudrate %d (Engineering Mode)", PORT_NAME, ENGINEERING_PORT_BAUDRATE);
        mEngineering = true;
        
        mThread = std::unique_ptr<std::thread>
            (new std::thread(&Engineering::receive, this));
        mThread->detach();
    } else {
        LOG_ERROR("Fail to open %s", PORT_NAME);
    }
}

void Engineering::receive() {
    struct timeval tvTimeout;
    fd_set fsRead;
    
    tvTimeout.tv_sec = 0;
    tvTimeout.tv_usec = 2000;
    
    uint8_t buff[FRAME_BUFFER_SIZE] = {0};
    bool dataAvailable = false;
    
    mUart->write("\n\r***************************************************************\n\r");
    mUart->write("*             If you see this message, it means               *\n\r");
    mUart->write("*             that you are in engineering mode                *\n\r");
    mUart->write("***************************************************************\n\r");
    
    while (mEngineering) {
        if (!mPrintPrefix) {
            mUart->write("fsw@root: ");
            mPrintPrefix = true;
        }
        
        if (!dataAvailable) {
            FD_ZERO(&fsRead);
            FD_SET(mUart->getFd(), &fsRead);
            
            int32_t ret = select(mUart->getFd() + 1, &fsRead, 
                nullptr, nullptr, &tvTimeout);
            
            if (ret == -1) break;
            else if (ret == 0) continue;
            else dataAvailable = true;
        }
        
        ssize_t bytesRead = read(mUart->getFd(), buff, sizeof(buff));
        
        if (bytesRead < 1) continue;
        
        if (buff[0] != 0x08 && buff[0] != 0x09) {
            mEngineeringBuffer.push_back(buff[0]);
            mUart->write(&buff[0], 1);   
        }
                
        // Backspace
        if (buff[0] == 0x08 && mEngineeringBuffer.size() > 0) {
            uint8_t blank = 0x20;
            uint8_t backspace = 0x08;
            
            mUart->write(&backspace, 1);
            mUart->write(&blank, 1);
            mUart->write(&backspace, 1);
            
            mEngineeringBuffer.pop_back();
        }
        
        // Enter
        if (buff[0] == 0x0D) {
            uint8_t newLine = 0x0A;
            mUart->write(&newLine, 1);
            
            std::string command = "";
            for (int i = 0; i < mEngineeringBuffer.size() - 1; i++)
                command += static_cast<char>(mEngineeringBuffer[i]);
            
            if (!command.empty()) executeCommand(command);
            
            mEngineeringBuffer.clear();
            mPrintPrefix = false;
        }
        
        dataAvailable = false;
    }
}

void Engineering::executeCommand(const std::string &command) {
    LOG_INFO("Executing command: %s", command.c_str());
    
    if (command == "exit" || command == "quit") {
        LOG_INFO("Entering Normal mode");
        mEngineering = false;
        mUart->close();
        
        std::shared_ptr<Message> msg = Message::obtain(shared_from_this(), MSG_START_NORMAL_MODE);
        sendToHub(msg);
        
        return;
    }
    
    FILE* pipe = popen(command.c_str(), "r");
    
    if (!pipe) {
        LOG_ERROR("Failed to execute the command");
        return;
    }

    char buffer[FRAME_BUFFER_SIZE];
    std::string currentLine;
    
    while (fgets(buffer, FRAME_BUFFER_SIZE, pipe) != nullptr) {
        currentLine += buffer;
        if (currentLine.back() == '\n') {
            currentLine += '\r';
            mUart->write(currentLine);
            
            currentLine.clear();
        }
    }

    pclose(pipe);
}

void Engineering::handleMessage(std::shared_ptr<Message> &message) {
    switch (message->id) {
        case MSG_START_ENGINEERING_MODE:
            startEngineeringMode();
            break;
            
        default:
            break;
    }
}