#ifndef _THING_SPEAK_INFO_H_
#define _THING_SPEAK_INFO_H_

struct ThingSpeakInfo
{
    int16_t Port = 80;
    const char *Host = "api.thingspeak.com"; //IP address of the thingspeak server
    const char *APIKeyWrite = ""; //Your own thingspeak write api_key
    const char *APIKeyRead = "";	 //Your own thingspeak read api_key
    unsigned long ChannelID;

    ThingSpeakInfo() = default;

    ThingSpeakInfo(const char* apiKeyWrite, const char* apiKeyRead, unsigned long channelID)
    {
        APIKeyWrite = apiKeyWrite;
        APIKeyRead = apiKeyRead;
        ChannelID = channelID;
    }
};

#endif