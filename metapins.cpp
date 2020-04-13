#include "api/farage.h"
#include <iostream>
#include "shared/libini.h"
using namespace Farage;

#define MAKEMENTION
#include "common_func.h"

#define VERSION "v0.0.8"

extern "C" Info Module
{
    "Metta-Pins",
    "nigel",
    "Mettaton's Pinned Messages",
    VERSION,
    "http://you.justca.me/",
    FARAGE_API_VERSION
};

namespace MetaPin
{
    int pinCmd(Handle&,int,const std::string[],const Message&);
    int addPinsCmd(Handle&,int,const std::string[],const Message&);
    int cyclePinsCmd(Handle&,int,const std::string[],const Message&);
    INIObject pins;
};

extern "C" int onModuleStart(Handle &handle, Global *global)
{
    recallGlobal(global);
    handle.createGlobVar("metapin_version",VERSION,"MetaPins Version",GVAR_CONSTANT);
    handle.regChatCmd("pin",&MetaPin::pinCmd,PIN,"Pin a message forever.");
    handle.regChatCmd("addpins",&MetaPin::addPinsCmd,PIN,"Stock up on some pins.");
    handle.regChatCmd("cyclepins",&MetaPin::cyclePinsCmd,PIN,"Gimmie some fresh pins.");
    MetaPin::pins.open("metapins.ini");
    return 0;
}

int MetaPin::pinCmd(Handle& handle, int argc, const std::string argv[], const Message& message)
{
    Global *global = recallGlobal();
    if (argc < 2)
        sendMessage(message.channel_id,"Usage: `" + global->prefix(message.guild_id) + "pin <message_id>`");
    else
    {
        ObjectResponse<Message> response = getMessage(message.channel_id,argv[1]);
        if (response.response.error())
            sendMessage(message.channel_id,"Error: Cannot find the message `" + argv[1] + "`!");
        else
        {
            MetaPin::pins(message.channel_id,argv[1]) = "1";
            MetaPin::pins.write("metapins.ini");
            auto topic = MetaPin::pins.topic_it(message.channel_id);
            if (topic->items() > 50)
            {
                for (auto it = topic->begin(), ite = topic->end();it != ite;++it)
                {
                    if (it->value == "1")
                    {
                        it->value = "0";
                        unpinMessage(message.channel_id,it->item);
                        break;
                    }
                }
            }
            pinMessage(message.channel_id,argv[1]);
            sendMessage(message.channel_id,makeMention(message.author.id,message.guild_id) + " has pinned a message https://discordapp.com/channels/" + message.guild_id + '/' + message.channel_id + '/' + argv[1]);
        }
    }
    return PLUGIN_HANDLED;
}

int MetaPin::addPinsCmd(Handle& handle, int argc, const std::string argv[], const Message& message)
{
    Global *global = recallGlobal();
    auto pinned = getPinnedMessages(message.channel_id);
    if (pinned.response.error())
        sendMessage(message.channel_id,"Error getting the pins! Maybe there aren't any?");
    else
    {
        auto topic = MetaPin::pins.topic_it(message.channel_id);
        if (topic != MetaPin::pins.end())
            for (auto it = topic->begin(), ite = topic->end();it != ite;++it)
                it->value = "0";
        for (auto it = pinned.array.rbegin(), ite = pinned.array.rend();it != ite;++it)
        {
            MetaPin::pins(message.channel_id,it->id) = "1";
        }
        MetaPin::pins.write("metapins.ini");
        sendMessage(message.channel_id,"Successfully stocked up on " + std::to_string(MetaPin::pins.items(message.channel_id)) + " pins!");
    }
    return PLUGIN_HANDLED;
}

int MetaPin::cyclePinsCmd(Handle& handle, int argc, const std::string argv[], const Message& message)
{
    Global *global = recallGlobal();
    auto topic = MetaPin::pins.topic_it(message.channel_id);
    if (topic == MetaPin::pins.end())
        sendMessage(message.channel_id,"There are no pins to cycle!");
    else if (topic->items() < 51)
    {
        int count = 0;
        for (auto it = topic->begin();it != topic->end();)
        {
            if (it->value == "0")
            {
                pinMessage(message.channel_id,it->item);
                std::string item = it->item;
                it = topic->erase(it);
                (*topic)(item) = "1";
                ++count;
            }
            else
                ++it;
        }
        if (count == 0)
            sendMessage(message.channel_id,"There are not enough pins to cycle!");
        else
        {
            MetaPin::pins.write("metapins.ini");
            sendMessage(message.channel_id,"Repinned " + std::to_string(count) + " message(s)!");
        }
    }
    else
    {
        int need = topic->items();
        if (need < 100)
            need %= 50;
        else
            need = 50;
        int rem = need;
        std::vector<std::string> bottom;
        std::vector<INIObject::INIItem> top;
        auto it = topic->begin();
        for (;it != topic->end();)
        {
            if (it->value != "0")
            {
                --rem;
                unpinMessage(message.channel_id,it->item);
                top.emplace_back(it->item,"0");
                it = topic->erase(it);
            }
            else
                ++it;
            if (!rem)
                break;
        }
        for (;it != topic->end();)
        {
            if (it->value == "0")
            {
                --need;
                //it->value = "1";
                pinMessage(message.channel_id,it->item);
                bottom.push_back(it->item);
                it = topic->erase(it);
            }
            else
                ++it;
            if (!need)
                break;
        }
        if (need)
        {
            for (it = topic->begin();it != topic->end();)
            {
                if (it->value == "0")
                {
                    --need;
                    //it->value = "1";
                    pinMessage(message.channel_id,it->item);
                    bottom.push_back(it->item);
                    it = topic->erase(it);
                }
                else
                    ++it;
                if (!need)
                    break;
            }
        }
        for (auto& i : bottom)
            (*topic)(i) = "1";
        topic->insert_range(topic->begin(),top.begin(),top.end());
        MetaPin::pins.write("metapins.ini");
        sendMessage(message.channel_id,"It has been done.");
    }
    return PLUGIN_HANDLED;
}










