#pragma once
#include <string>
#include <vector>
#include <cstring>
// In-memory NVS substitute for exercising the real storage adapter.
class Preferences {
public:
  inline static std::vector<unsigned char> bytes;
  inline static bool failWrite=false;
  bool begin(const char*,bool readOnly=false){return !readOnly||!bytes.empty();}
  void end(){}
  bool isKey(const char*){return !bytes.empty();}
  size_t getBytesLength(const char*){return bytes.size();}
  size_t getBytes(const char*,void*out,size_t n){if(n!=bytes.size())return 0;memcpy(out,bytes.data(),n);return n;}
  size_t putBytes(const char*,const void*data,size_t n){if(failWrite)return 0;const auto*p=static_cast<const unsigned char*>(data);bytes.assign(p,p+n);return n;}
};
