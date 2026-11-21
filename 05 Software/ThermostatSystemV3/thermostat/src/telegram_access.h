#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Bounded, platform-independent policy. Owner is supplied by existing credentials;
// this record never contains tokens or changes the owner's identity.
namespace TelegramAccess {
constexpr unsigned Capacity=8, IdBytes=20, NameBytes=49;
struct Entry {char id[IdBytes];char name[NameBytes];};
struct Record {uint32_t magic,version,count;Entry entries[Capacity];uint32_t checksum;};
inline uint32_t checksum(const Record&r){
  uint32_t hash=2166136261u;const auto*p=reinterpret_cast<const unsigned char*>(&r);
  for(size_t i=0;i<offsetof(Record,checksum);++i){hash^=p[i];hash*=16777619u;}return hash;
}
inline void seal(Record&r){r.checksum=checksum(r);}
inline Record empty(){Record r={};r.magic=0x54474131;r.version=1;seal(r);return r;}
inline bool validId(const char*s){
  if(!s||*s<'1'||*s>'9')return false;
  uint64_t value=0;unsigned n=0;
  for(;*s;++s){if(*s<'0'||*s>'9'||++n>=IdBytes)return false;unsigned digit=*s-'0';
    if(value>(uint64_t(INT64_MAX)-digit)/10)return false;value=value*10+digit;}
  return true;
}
inline bool validName(const char*s){
  if(!s)return false;unsigned n=0;
  for(;*s;++s){if(++n>=NameBytes||static_cast<unsigned char>(*s)<32||*s==127)return false;}
  return true;
}
inline bool valid(const Record&r){
  if(r.magic!=0x54474131||r.version!=1||r.count>Capacity||r.checksum!=checksum(r))return false;
  for(unsigned i=0;i<r.count;++i){const auto&e=r.entries[i];
    if(!memchr(e.id,0,IdBytes)||!memchr(e.name,0,NameBytes)||!validId(e.id)||!validName(e.name))return false;
    for(unsigned j=0;j<i;++j)if(!strcmp(e.id,r.entries[j].id))return false;
  }return true;
}
inline int find(const Record&r,const char*id){for(unsigned i=0;i<r.count;++i)if(!strcmp(r.entries[i].id,id))return i;return -1;}
inline bool owner(const char*admin,const char*chat,const char*sender){
  return validId(admin)&&!strcmp(admin,chat)&&!strcmp(admin,sender);
}
inline bool allowed(const Record&r,const char*admin,const char*chat,const char*sender){
  return validId(chat)&&!strcmp(chat,sender)&&(owner(admin,chat,sender)||find(r,chat)>=0);
}
enum Operation {None,Start,Help,List,Allow,Remove};
struct Command {Operation operation=None;bool valid=false;char id[IdBytes]={},name[NameBytes]={};};
inline bool space(char c){return c==' '||c=='\t'||c=='\r'||c=='\n';}
inline Command parse(const char*text){
  Command result;while(space(*text))++text;const char*end=text+strlen(text);while(end>text&&space(end[-1]))--end;
  const char*token=text;while(text<end&&!space(*text))++text;size_t n=text-token;
  if(n==6&&!memcmp(token,"/start",6))result.operation=Start;
  else if(n==5&&!memcmp(token,"/help",5))result.operation=Help;
  else if(n==11&&!memcmp(token,"/gebruikers",11))result.operation=List;
  else if(n==9&&!memcmp(token,"/toestaan",9))result.operation=Allow;
  else if(n==12&&!memcmp(token,"/verwijderen",12))result.operation=Remove;
  else return result;
  while(text<end&&space(*text))++text;
  if(result.operation==Start||result.operation==Help||result.operation==List){result.valid=text==end;return result;}
  token=text;while(text<end&&!space(*text))++text;n=text-token;
  if(!n||n>=IdBytes)return result;memcpy(result.id,token,n);if(!validId(result.id))return result;
  while(text<end&&space(*text))++text;n=end-text;
  if(result.operation==Remove){result.valid=n==0;return result;}
  if(n>=NameBytes)return result;memcpy(result.name,text,n);result.valid=validName(result.name);return result;
}
enum Result {Changed,Unchanged,Full,Missing,OwnerProtected,Invalid,Denied};
// Authorization is part of the mutation, so callers cannot accidentally expose it.
inline Result change(Record&r,const char*admin,const char*chat,const char*sender,const Command&cmd){
  if(!owner(admin,chat,sender))return Denied;
  if(!cmd.valid||(cmd.operation!=Allow&&cmd.operation!=Remove)||!validId(cmd.id)||!validName(cmd.name))return Invalid;
  if(!strcmp(admin,cmd.id))return OwnerProtected;
  int index=find(r,cmd.id);
  if(cmd.operation==Allow){
    if(index<0){if(r.count==Capacity)return Full;index=r.count++;}
    else if(!strcmp(r.entries[index].name,cmd.name))return Unchanged;
    auto&e=r.entries[index];memset(&e,0,sizeof(e));strcpy(e.id,cmd.id);strcpy(e.name,cmd.name);
  }else{
    if(index<0)return Missing;
    for(unsigned i=index+1;i<r.count;++i)r.entries[i-1]=r.entries[i];
    memset(&r.entries[--r.count],0,sizeof(Entry));
  }seal(r);return Changed;
}
}
