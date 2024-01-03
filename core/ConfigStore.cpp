#include "ConfigStore.h"
#include "Log.h"

#include <fstream>
#include <algorithm>

ConfigStore* ConfigStore::mInstance = nullptr;

ConfigStore::ConfigStore() {
    mConfigMap.clear();
}

ConfigStore::~ConfigStore() {
    mConfigMap.clear();
}

ConfigStore* ConfigStore::getInstance() {
    if (mInstance == nullptr) {
        mInstance = new ConfigStore();
        mInstance->parseConfig();
    }

    return mInstance;
}

int ConfigStore::getInt(std::string tag) {
    std::map<std::string, std::string>::iterator it;
    it = mConfigMap.find(tag);
    if (it != mConfigMap.end()) {
        return atoi((it->second).c_str());
    }
    return -1;
}

std::string ConfigStore::getString(std::string tag) {
    std::map<std::string, std::string>::iterator it;
    it = mConfigMap.find(tag);
    if (it != mConfigMap.end()) {
        return it->second;
    }
    return "";
}

bool ConfigStore::parseConfig(std::string config) {
    ifstream file;
    file.open(config.c_str());
    string delimeter = "=";
    int initPos = 0;
    
    if (file.fail()) {
        LOG_ERROR("Unable to find %s", config.c_str());
        return false;
    }
    
    string line = "";
    while (getline(file, line)) {
        size_t found = line.find_first_of("#");
        string data = line.substr(0, found);
        
        data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
        
        if (data.empty()) continue;
        
        uint32_t len = data.find(delimeter);
        
        string tag, value;
        
        if (len != string::npos) {
            tag = data.substr(initPos, len);
            value = data.substr(len + 1);
        }
        
        tag = reduce(tag);
        value = reduce(value);
        
        if (tag.empty() || value.empty()) continue;
            
        std::map<std::string, std::string>::iterator it = mConfigMap.find(tag);
        if (it != mConfigMap.end()) {
            mConfigMap.erase(tag);   
        }
        mConfigMap.insert(std::pair<string, string>(tag, value));
    }
    
    return true;
}

std::string ConfigStore::trim(const std::string &str, const std::string &whitespace) {
    size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) {
        return "";
    }
        
    size_t strEnd = str.find_last_not_of(whitespace);
    size_t strRange = strEnd - strBegin + 1;
    
    return str.substr(strBegin, strRange);
}

std::string ConfigStore::reduce(const std::string& str, const std::string& fill, 
        const std::string& whitespace) {
   string result = trim(str, whitespace);

   size_t beginSpace = result.find_first_of(whitespace);
   while (beginSpace != std::string::npos)
   {
      size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
      size_t range = endSpace - beginSpace;

      result.replace(beginSpace, range, fill);

      size_t newStart = beginSpace + fill.length();
      beginSpace = result.find_first_of(whitespace, newStart);
   }

   return result;            
}

void ConfigStore::dump() {
    map<string, string>::iterator it;
    for (it = mConfigMap.begin(); it != mConfigMap.end(); ++it) {
        LOG_DEBUG("%s = %s", it->first.c_str(), it->second.c_str());
    }
}

