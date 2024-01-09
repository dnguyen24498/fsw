#pragma once

#include <iostream>
#include <map>

#define STRINGIFY(x) #x
#define STR_VALUE(x) STRINGIFY(x)

using namespace std;

class ConfigStore final {
public:
  ConfigStore(ConfigStore &other) = delete;
  void operator=(const ConfigStore &) = delete;
  
  static ConfigStore* getInstance();
  
  bool parseConfig(std::string config = STR_VALUE(DEFAULT_CONFIG));
  int getInt(std::string tag);
  std::string getString(std::string tag);
  void dump();
private:
  ConfigStore();
  virtual ~ConfigStore();
  
  std::string trim(const std::string &str, const std::string &whitespace = " \t");
  std::string reduce(const std::string& str, const std::string& fill = " ", 
      const std::string& whitespace = " \t");
private:
  static ConfigStore* mInstance;
  std::map<std::string, std::string> mConfigMap;
};