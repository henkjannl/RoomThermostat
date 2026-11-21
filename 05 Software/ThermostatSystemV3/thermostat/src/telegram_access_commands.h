#pragma once
// Included after the worker-owned bot and conversation map are declared.
static bool telegramAccessPending=false;
static TelegramAccess::Record telegramAccessDraft;
static String telegramAccessConfirmation;

inline bool telegramChatAllowed(const String&chat,const String&sender){
  if(!telegramAccessReady)return TelegramAccess::owner(networkData.botChatID.c_str(),chat.c_str(),sender.c_str());
  return TelegramAccess::allowed(telegramUsers,networkData.botChatID.c_str(),chat.c_str(),sender.c_str());
}
inline void serviceTelegramAccessReplies(){
  bool saved;
  if(!telegramAccessPending||xQueueReceive(telegramAccessSaved,&saved,0)!=pdPASS)return;
  if(saved){
    telegramUsers=telegramAccessDraft;
    for(auto it=userConversation.begin();it!=userConversation.end();){
      if(!telegramChatAllowed(it->first,it->first))it=userConversation.erase(it);else ++it;
    }
    bot.sendMessage(networkData.botChatID,telegramAccessConfirmation,"");
  }else bot.sendMessage(networkData.botChatID,"Niet opgeslagen. De gebruikerslijst is niet gewijzigd; probeer opnieuw.","");
  telegramAccessPending=false;
}
inline String telegramAccessHelp(){
  return "/start - thermostaatmenu openen\n/help - deze uitleg\n/toestaan CHAT_ID Naam - toevoegen of naam wijzigen\n/gebruikers - lijst tonen\n/verwijderen CHAT_ID - toegang intrekken\n\nVoorbeeld: /toestaan 123456789 Anna\nVerwijderen: /verwijderen 123456789\nLaat een nieuwe gebruiker eerst /start sturen voor diens ID en na toevoegen opnieuw /start.\nAlleen de beheerder kan de lijst beheren. Wijzigingen blijven na een herstart bewaard.";
}
inline bool handleTelegramAccessMessage(const telegramMessage&message){
  using namespace TelegramAccess;
  const char*admin=networkData.botChatID.c_str();
  const char*chat=message.chat_id.c_str();const char*sender=message.from_id.c_str();
  bool permitted=telegramChatAllowed(message.chat_id,message.from_id);
  if(message.type!="message")return !permitted; // Also gate every old keyboard callback.
  Command command=parse(message.text.c_str());
  if(!permitted){
    // No conversation allocation or controller commands for unknown users.
    // Bound unsolicited /start responses globally to avoid flooding the worker.
    static bool replied=false;static uint32_t lastReply=0;
    if(validId(chat)&&!strcmp(chat,sender)&&(command.operation==Start||command.operation==Help)&&command.valid&&(!replied||millis()-lastReply>=3000)){
      replied=true;lastReply=millis();
      bot.sendMessage(message.chat_id,String("Je hebt nog geen toegang. Geef dit ID aan de beheerder: ")+chat,"");
    }
    return true;
  }
  if(command.operation==Help){
    bot.sendMessage(message.chat_id,owner(admin,chat,sender)?telegramAccessHelp():String("/start - thermostaatmenu openen\n/help - deze uitleg\nGebruik de knoppen in het menu om de thermostaat te bedienen. Alleen de beheerder kan gebruikers toevoegen of verwijderen."),"");return true;
  }
  if(command.operation==None||command.operation==Start)return false;
  if(!owner(admin,chat,sender)){
    bot.sendMessage(message.chat_id,"Alleen de beheerder kan de gebruikerslijst beheren.","");return true;
  }
  if(!command.valid){bot.sendMessage(message.chat_id,telegramAccessHelp(),"");return true;}
  if(command.operation==List){
    String response=String("Beheerder: ")+admin+"\nGebruikers (maximaal 8):\n";
    if(!telegramUsers.count)response+="Geen extra gebruikers.\n";
    for(unsigned i=0;i<telegramUsers.count;++i){const auto&e=telegramUsers.entries[i];response+=String(e.id)+" "+e.name+"\n";}
    response+="\n"+telegramAccessHelp();bot.sendMessage(message.chat_id,response,"");return true;
  }
  if(!telegramAccessReady){bot.sendMessage(message.chat_id,"Gebruikersopslag niet beschikbaar. Herstart de thermostaat.","");return true;}
  if(telegramAccessPending){bot.sendMessage(message.chat_id,"Vorige wijziging wordt nog opgeslagen; probeer zo opnieuw.","");return true;}
  telegramAccessDraft=telegramUsers;
  Result result=change(telegramAccessDraft,admin,chat,sender,command);
  const char*error=result==Full?"De lijst is vol (maximaal 8 extra gebruikers).":result==Missing?"Dit ID staat niet in de gebruikerslijst.":result==OwnerProtected?"De vaste beheerder kan niet worden gewijzigd of verwijderd.":result==Unchanged?"Deze gebruiker staat al zo in de lijst.":"Ongeldige opdracht.";
  if(result!=Changed){bot.sendMessage(message.chat_id,error,"");return true;}
  telegramAccessConfirmation=command.operation==Allow?String("Toegang opgeslagen voor ")+command.id+(command.name[0]?String(" (")+command.name+")":"")+". Laat deze persoon /start sturen.":String("Toegang ingetrokken voor ")+command.id+".";
  if(xQueueSend(telegramAccessWrites,&telegramAccessDraft,0)==pdPASS)telegramAccessPending=true;
  else bot.sendMessage(message.chat_id,"Niet opgeslagen; probeer opnieuw.","");
  return true;
}
