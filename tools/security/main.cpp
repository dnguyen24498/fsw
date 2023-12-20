#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <openssl/evp.h>

using namespace std;

static const char CLT[64] = {
    0x2D,                                                       //-
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, //0 to 9
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, //A to Z
    0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
    0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
    0x5F,                                                       //_
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, //a to z
    0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
    0x75, 0x76, 0x77, 0x78, 0x79, 0x7A
};

string current_datetime() {
    time_t now = time(nullptr);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return buffer;
}

string reverseHexString(const std::string& hexString) {
    string reversedString;
    for (int i = hexString.length() - 2; i >= 0; i -= 2) {
        reversedString += hexString.substr(i, 2);
    }
    return reversedString;
}


string stringToHex(const string& input) {
    stringstream hexStream;
    hexStream << hex << std::setfill('0');
    
    for (char ch : input) {
        hexStream << setw(2) << static_cast<int>(static_cast<unsigned char>(ch));
    }
    
    return hexStream.str();
}

#define MASTER_PASSWORD     "syWTZUFHoxqe7COegmkM"

#define WIP_FILE            "wip.txt"
#define WIP_LEN             15

#define LOG_LEVEL_INFO      0
#define LOG_LEVEL_WARNING   1
#define LOG_LEVEL_ERROR     2

#define CURRENT_LOG_LEVEL LOG_LEVEL_INFO

#define LOG(level, ...) \
    do { \
        if (level >= CURRENT_LOG_LEVEL) { \
            switch (level) { \
                case LOG_LEVEL_INFO: \
                    cout << "[" << current_datetime() << " INFO] " ; \
                    break; \
                case LOG_LEVEL_WARNING: \
                    cout << "[" << current_datetime() << " WARNING] "; \
                    break; \
                case LOG_LEVEL_ERROR: \
                    cerr << "[" << current_datetime() << " ERROR] "; \
                    break; \
            } \
            printf(__VA_ARGS__); \
            cout << endl; \
        } \
    } while(0);

string wip_to_pepu(string wip, string interface = "J2") {
    string master_password(MASTER_PASSWORD);
    unsigned char out[10];
    string salt = "";
    string pepu = "";

    if (wip.length() < WIP_LEN) {
        LOG(LOG_LEVEL_ERROR, "Invalid wip length");
    }

    wip.erase(remove(wip.begin(), wip.end(), '-'), wip.end());
    salt = wip + interface;

    // encrypt key
    char *p = new char[master_password.length() + 1];
    char *s = new char[salt.length() + 1];

    for (int i = 0; i < master_password.length() + 1; i++) {
        p[i] = (char)master_password[i];
    }
    p[master_password.length()] = 0;

    for (int i = 0; i < salt.length(); i++) {
        s[i] = (char)salt[i];
    }
    s[salt.length()] = 0;

    PKCS5_PBKDF2_HMAC(p, strlen(p), (unsigned char *)s, strlen(s), 10000, EVP_sha256(), 8, out);

    for (int i = 0; i < 8; i++) {
        pepu += CLT[out[i] % 64];
    }

    delete[] p;
    delete[] s;

    return pepu;
}

int main(int argc, char* argv[]) {

    if (argc != 1) {
        for (int i = 1; i < argc; i++) {
            if (strlen(argv[i]) < WIP_LEN) {
                LOG(LOG_LEVEL_ERROR, "Invalid WIP length");
                return EXIT_FAILURE;
            }

            string mcu = wip_to_pepu(argv[i], "J1");
            string pepu = wip_to_pepu(argv[i], "J2");

            string mcu_hex = stringToHex(mcu);
            string mcu_hex_p1 = reverseHexString(mcu_hex.substr(0, 8));
            string mcu_hex_p2 = reverseHexString(mcu_hex.substr(8, 16));
            LOG(LOG_LEVEL_INFO, "WIP: %s, PEPU: %s, MCU: %s => Part 0: 0x%s, Part 1: 0x%s", argv[i], pepu.c_str(), mcu.c_str(), mcu_hex_p1.c_str(), mcu_hex_p2.c_str());
        }
        return EXIT_SUCCESS;
    }

    ifstream file(WIP_FILE);

    if (!file.is_open()) {
        LOG(LOG_LEVEL_ERROR, "Failed to open WIP file");
        return EXIT_FAILURE;
    }

    string line;
    while (getline(file, line)) {
        string mcu = wip_to_pepu(line, "J1");
        string pepu = wip_to_pepu(line, "J2");

        string mcu_hex = stringToHex(mcu);
        string mcu_hex_p1 = reverseHexString(mcu_hex.substr(0, 8));
        string mcu_hex_p2 = reverseHexString(mcu_hex.substr(8, 16));

        LOG(LOG_LEVEL_INFO, "WIP: %s, PEPU: %s, MCU: %s => Part 0: 0x%s, Part 1: 0x%s", line.c_str(), pepu.c_str(), mcu.c_str(), mcu_hex_p1.c_str(), mcu_hex_p2.c_str());
    }

    return EXIT_SUCCESS;
}