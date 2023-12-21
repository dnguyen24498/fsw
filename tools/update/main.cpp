#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>

// Xmodem control characters
constexpr char SOH = 0x01;  // Start of Header
constexpr char EOT = 0x04;  // End of Transmission
constexpr char ACK = 0x06;  // Acknowledgment
constexpr char NAK = 0x15;  // Negative Acknowledgment

// Xmodem packet size
#define PACKET_SIZE 128

// Xmodem packet structure
struct XmodemPacket {
    char header;         // SOH (Start of Header)
    char blockNumber;    // Block number (1 to 255)
    char blockNumberC;   // Block number complement
    char data[PACKET_SIZE]; // Data payload
    char crc;            // CRC checksum
};

// Calculate CRC for Xmodem packet
char calculateCRC(const XmodemPacket& packet) {
    char crc = 0;
    crc += packet.blockNumber;
    crc += packet.blockNumberC;
    for (int i = 0; i < PACKET_SIZE; ++i) {
        crc += packet.data[i];
    }
    return crc;
}

// Send a file using Xmodem protocol
bool sendFile(const std::string& filename, int uart) {
    struct stat fileStat;
    
    stat(filename.c_str(), &fileStat);
    
    std::ifstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "\r" << "Error: Could not open file " << filename << std::endl;
        return false;
    }
    
    int32_t currentPacket = 0;
    int32_t totalPacket = fileStat.st_size / PACKET_SIZE;
    
    int sec = 3;
    while(sec) {
        std::cout << "\r" << "Update will begin in " << sec-- << "s" << std::flush;
        sleep(1);
    }
    
    while (true) {
        XmodemPacket packet;
        
        packet.header = SOH;
        file.read(packet.data, PACKET_SIZE);
        std::streamsize bytesRead = file.gcount();
        
        if (bytesRead == 0) {
            // End of file
            break;
        }
        
        packet.blockNumber = static_cast<char>((rand() % 255) + 1); // Random block number
        packet.blockNumberC = ~packet.blockNumber;
        packet.crc = calculateCRC(packet);
        
        write(uart, reinterpret_cast<const char*>(&packet), sizeof(XmodemPacket));
        // Wait for acknowledgment from the receiver
        usleep(2000);
        char ack;
        read(uart, &ack, 1);
        if (ack != ACK) {
            std::cerr << "\r" << "Error: Acknowledgment not received. Retrying..." << std::flush;
            return false;;
        } else {
            std::cout << "\r" << "Transferring " << currentPacket++ << "/" << totalPacket << " packets" << std::flush;
        }
    }
    // Send End of Transmission (EOT)
    write(uart, &EOT, 1);
    
    file.close();
    
    std::cout << std::endl;
    
    return true;
}

int openUART(const std::string& device, speed_t baudRate) {
    int uart = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart == -1) {
        std::cerr << "Failed to open UART device: " << device << std::endl;
        return -1;
    }

    termios options;
    if (tcgetattr(uart, &options) != 0) {
        std::cerr << "Failed to get UART attributes." << std::endl;
        close(uart);
        return -1;
    }

    cfsetispeed(&options, baudRate);
    cfsetospeed(&options, baudRate);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CSTOPB;
    options.c_iflag &= ~(ICRNL | IXON);
    options.c_iflag |= (IXANY | IGNBRK | IGNPAR);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE);
    
    tcflush(uart, TCIFLUSH);

    if (tcsetattr(uart, TCSANOW, &options) != 0) {
        std::cerr << "Failed to set UART attributes." << std::endl;
        close(uart);
        return -1;
    }

    return uart;
}

int main() {
    std::string filename = "fsw"; // Replace with your file name
    std::string device = "/dev/virtualcom1";
    speed_t baudRate = B921600;
    
    int uart = openUART(device, baudRate);
    if (uart == -1) {
        std::cerr << "Failed to open UART device." << std::endl;
        return 1;
    }

    if (sendFile(filename, uart)) {
        std::cout << "\r" << "File sent successfully." << std::endl;
    } else {
        std::cout << "\r" << "Failed to send file." << std::endl;
    }
    
    close(uart);

    return 0;
}