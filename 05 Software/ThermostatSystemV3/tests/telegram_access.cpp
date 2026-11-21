#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdio>
#include <memory>
#include "telegram_access.h"
class String:public std::string {
public:
  using std::string::string;
  String(const std::string&s):std::string(s){}
};
struct TestQueue {size_t size;std::vector<unsigned char> data;};
using QueueHandle_t=TestQueue*;
constexpr int pdPASS=1;
static std::vector<std::unique_ptr<TestQueue>> queues;
QueueHandle_t xQueueCreate(int,size_t size){queues.emplace_back(new TestQueue{size,{}});return queues.back().get();}
int xQueueSend(QueueHandle_t q,const void*p,int){if(!q||!q->data.empty())return 0;const auto*b=static_cast<const unsigned char*>(p);q->data.assign(b,b+q->size);return pdPASS;}
int xQueueReceive(QueueHandle_t q,void*p,int){if(!q||q->data.empty())return 0;memcpy(p,q->data.data(),q->size);q->data.clear();return pdPASS;}
void xQueueOverwrite(QueueHandle_t q,const void*p){q->data.clear();assert(xQueueSend(q,p,0)==pdPASS);}
struct {void println(const char*){}} Serial;
static uint32_t now=0;
uint32_t millis(){return now;}
#include "telegram_access_storage.h"
struct {String botChatID="100";} networkData;
struct telegramMessage {String text,chat_id,from_id,type;};
struct Sent {String chat,text;};
struct {std::vector<Sent> sent;bool sendMessage(const String&chat,const String&text,const char*){sent.push_back({chat,text});return true;}} bot;
std::map<String,int> userConversation;
#include "telegram_access_commands.h"
using namespace TelegramAccess;
static bool message(const char*text,const char*chat="100",const char*sender="100",const char*type="message"){
  now+=4000;return handleTelegramAccessMessage({text,chat,sender,type});
}
static void save(){assert(telegramAccessPending);serviceTelegramAccessStorage();serviceTelegramAccessReplies();assert(!telegramAccessPending);}
int main(){
  assert(validId("9223372036854775807")&&!validId("9223372036854775808"));
  for(auto id:{"","0","01","-100","1x"," 12","10000000000000000000"})assert(!validId(id));
  assert(parse("/help").operation==Help&&parse("/help").valid);
  assert(parse(" /toestaan 200 Anna Maria ").valid);
  for(auto text:{"/toestaan","/toestaan -1 Anna","/toestaan 01 Anna","/verwijderen 200 extra","/gebruikers extra","/toestaan 200 bad\nname"})assert(!parse(text).valid);
  Record record=empty();assert(valid(record));
  auto add=parse("/toestaan 200 Anna");
  assert(change(record,"100","200","200",add)==Denied&&record.count==0);
  assert(change(record,"100","100","200",add)==Denied);
  assert(change(record,"100","100","100",parse("/verwijderen 100"))==OwnerProtected);
  for(int i=0;i<8;++i){char text[64];snprintf(text,sizeof(text),"/toestaan %d Naam",i+200);assert(change(record,"100","100","100",parse(text))==Changed);}
  assert(change(record,"100","100","100",parse("/toestaan 900 Vol"))==Full);
  assert(change(record,"100","100","100",parse("/toestaan 200 Naam"))==Unchanged);
  assert(change(record,"100","100","100",parse("/toestaan 200 Nieuwe naam"))==Changed&&record.count==8);
  auto corrupt=record;corrupt.entries[0].id[0]^=1;assert(!valid(corrupt));
  corrupt=record;strcpy(corrupt.entries[1].id,corrupt.entries[0].id);seal(corrupt);assert(!valid(corrupt));
  corrupt=record;memset(corrupt.entries[0].name,'a',NameBytes);seal(corrupt);assert(!valid(corrupt));

  assert(initializeTelegramAccess());assert(telegramChatAllowed("100","100"));
  assert(!telegramChatAllowed("200","200"));
  assert(message("/start","200","200")&&userConversation.empty());
  assert(bot.sent.back().text.find("200")!=std::string::npos);
  size_t replies=bot.sent.size();now+=1;
  assert(handleTelegramAccessMessage({"/start","201","201","message"}));assert(bot.sent.size()==replies);
  assert(message("/help")&&bot.sent.back().text.find("/toestaan 123456789 Anna")!=std::string::npos);
  assert(message("/toestaan 200 Anna"));assert(telegramAccessPending&&!telegramChatAllowed("200","200"));
  replies=bot.sent.size();assert(message("/toestaan 201 Piet"));assert(bot.sent.size()==replies+1&&telegramAccessDraft.count==1);
  save();assert(telegramChatAllowed("200","200")&&Preferences::bytes.size()==sizeof(Record));
  assert(!message("/start","200","200"));
  assert(!message("button","200","200","callback_query"));
  assert(message("button","100","200","callback_query"));
  assert(message("/toestaan 300 Hacker","200","200"));assert(!telegramAccessPending);
  assert(message("/toestaan 300 Hacker","-1","100"));assert(!telegramAccessPending);
  assert(message("/help","200","200")&&bot.sent.back().text.find("/toestaan")==std::string::npos);
  assert(message("/gebruikers")&&bot.sent.back().text.find("200 Anna")!=std::string::npos);
  assert(initializeTelegramAccess());assert(telegramChatAllowed("200","200")); // Reboot reload.
  assert(message("/toestaan 200 Anna Maria"));save();assert(!strcmp(telegramUsers.entries[0].name,"Anna Maria"));
  userConversation["200"]=1;
  assert(message("/verwijderen 200"));Preferences::failWrite=true;save();
  assert(telegramChatAllowed("200","200")&&userConversation.count("200"));
  assert(bot.sent.back().text.find("Niet opgeslagen")!=std::string::npos);
  Preferences::failWrite=false;assert(message("/verwijderen 200"));save();
  assert(!telegramChatAllowed("200","200")&&!userConversation.count("200"));
  assert(message("old button","200","200","callback_query"));
  assert(initializeTelegramAccess());assert(!telegramChatAllowed("200","200"));
  assert(message("/verwijderen 100"));assert(!telegramAccessPending&&telegramChatAllowed("100","100"));
  Preferences::bytes[0]^=1;assert(initializeTelegramAccess());assert(telegramUsers.count==0&&telegramChatAllowed("100","100"));
  puts("PASS: Telegram admin/member authorization, command parsing, help, capacity, NVS reboot persistence, failed writes, revocation, old callbacks, integrity and unknown-user rate limit.");
}
