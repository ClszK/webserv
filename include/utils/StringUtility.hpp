#pragma once

#include <string>

class StringUtility {
 private:
  static bool isSpecial(char c);

 public:
  static bool isValidNickName(const std::string& nickName);
  static bool isValidUserName(const std::string& userName);
  static bool isValidChannelName(const std::string& channelName);
  static bool isValidKey(const std::string& key);
};