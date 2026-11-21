#pragma once
#include <string>
#include <vector>
using String=std::string;
struct WiFiClient {};
enum { HTTPC_DISABLE_FOLLOW_REDIRECTS };
extern int httpCode;
extern std::vector<std::string> requests;
struct HTTPClient {
  void setConnectTimeout(int) {} void setTimeout(int) {} void setReuse(bool) {}
  void setFollowRedirects(int) {}
  bool begin(WiFiClient &,const char *) { return true; }
  void addHeader(const char *,const String &) {}
  int POST(uint8_t *body,size_t length) {requests.emplace_back(reinterpret_cast<char*>(body),length);return httpCode;}
  void end() {}
};
