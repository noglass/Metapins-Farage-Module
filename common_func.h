
/*   example usage of FARAGE_COMMON_UNDEF_ALL when it has already been included before
#ifdef FARAGE_COMMON_MODULE_FUNC
    #ifndef REMOVE
        #define FARAGE_COMMON_UNDEF_ALL
        #include "common_func.h"
        #define REMOVE
        #include "common_func.h"
    #endif
#endif
*/

#ifdef FARAGE_COMMON_UNDEF_ALL
    #undef FARAGE_COMMON_UNDEF_ALL
    #undef FARAGE_COMMON_MODULE_FUNC
    #undef SUBCALL
    #undef REGSUBEX
    #undef REGSUBEX_STD
    #undef MAKEMENTION
    #undef NOSPACE
    #undef REMOVE
    #undef REMOVEALL
    #undef STRREPLACE
    #undef STR2BOOL
    #undef SPLITSTRING
    #undef SPLITSTRINGANY
    #undef RANDOMNEGATIVEEMOJI
    #undef HEXIFY
#else

#ifndef FARAGE_COMMON_MODULE_FUNC
#define FARAGE_COMMON_MODULE_FUNC

#if defined SUBCALL || defined REGSUBEX || defined REGSUBEX_STD
#ifndef SUBCALL
#define SUBCALL
#endif
typedef std::string (*subcall)(const std::string&);
#endif

#ifdef REGSUBEX
std::string regsubex(std::string subject, const rens::regex &re, const std::string &replace, subcall cb, const std::string &prefix = "", const std::string &suffix = "")
{
    std::string ret;
    rens::smatch ml;
    bool matched = false;
    while (rens::regex_search(subject,ml,re))
    {
        matched = true;
        ret = ret + ml.prefix().str() + prefix + (*cb)(ml.format(replace)) + suffix;
        subject = ml.suffix().str();
    }
    if ((!matched) || (subject.size() > 0))
        ret += subject;
    return std::move(ret);
}
#elif defined REGSUBEX_STD
std::string regsubex(std::string subject, const std::regex &re, const std::string &replace, subcall cb, const std::string &prefix = "", const std::string &suffix = "")
{
    std::string ret;
    std::smatch ml;
    bool matched = false;
    while (std::regex_search(subject,ml,re))
    {
        matched = true;
        ret = ret + ml.prefix().str() + prefix + (*cb)(ml.format(replace)) + suffix;
        subject = ml.suffix().str();
    }
    if ((!matched) || (subject.size() > 0))
        ret += subject;
    return std::move(ret);
}
#endif

#ifdef MAKEMENTION
std::string makeMention(const std::string& userID, const std::string& serverID = "")
{
    std::string ping = "<@";
    if (serverID.size() > 0)
    {
        Farage::ServerMember member = Farage::getServerMember(serverID,userID);
        if (member.nick != member.user.username)
            ping += '!';
    }
    return ping + userID + '>';
}
std::string makeMention(const User& user, const std::string& serverID = "")
{
    std::string ping = "<@";
    if (serverID.size() > 0)
    {
        Farage::ServerMember member = Farage::getServerMember(serverID,user.id);
        if (member.nick != user.username)
            ping += '!';
    }
    return ping + user.id + '>';
}
#endif

#if defined NOSPACE || defined STR2BOOL || defined SPLITSTRING || defined SPLITSTRINGANY
#ifndef NOSPACE
#define NOSPACE
#endif
std::string nospace(std::string text)
{
    bool s = true;
    for (auto it = text.begin();it != text.end();)
    {
        switch (*it)
        {
            case ' ':
            case '\t':
            {
                if (s)
                    it = text.erase(it);
                else
                {
                    ++it;
                    s = true;
                }
                break;
            }
            default:
            {
                ++it;
                s = false;
            }
        }
    }
    if (s)
    {
        switch (text.at(text.size()-1))
        {
            case ' ':
            case '\t':
            {
                text.erase(text.size()-1);
            }
        }
    }
    return std::move(text);
}
#endif

#ifdef REMOVE
std::string remove(std::string str, const std::string &substr)
{
    for (size_t pos = 0, sublen = substr.size();(pos = str.find(substr,pos)) != std::string::npos;str.erase(pos,sublen));
    return std::move(str);
}
#endif

#ifdef REMOVEALL
std::string removeall(std::string str, const std::string &chars)
{
    for (size_t pos = 0;(pos = str.find_first_of(chars,pos)) != std::string::npos;str.erase(pos,1));
    return std::move(str);
}
#endif

#ifdef STRREPLACE
std::string strreplace(std::string str, const std::string &find, const std::string &replace)
{
    for (size_t pos = str.find(find);pos != std::string::npos;pos = str.find(find,pos))
    {
        str.erase(pos,find.size());
        str.insert(pos,replace);
        pos += replace.size();
    }
    return std::move(str);
}
#endif

#ifdef STR2BOOL
bool str2bool(std::string str)
{
    str = strlower(nospace(str));
    if ((str.size() < 1) || (str == "false") || (str == "0"))
        return false;
    return true;
}
#endif

#ifdef SPLITSTRING
std::string* splitString(const std::string &src, const std::string &delim, int &count, const std::string &quote = "\"'“`")
{
    if (src.size() == 0)
        return nullptr;
    std::vector<std::string> list;
    bool open = false;
    char c;
    for (size_t x = 0, slen = src.size(), dlen = delim.size(), last = 0; x < slen;)
    {
        if (quote.find((c = src.at(x))) != std::string::npos)
        {
            open = !open;
            if ((open) && (src.find(c,x+1) == std::string::npos))
                open = false;
        }
        if ((!open) && ((src.compare(x,dlen,delim) == 0) || (x+1 >= slen)))
        {
            std::string arg = nospace(src.substr(last,(x+=dlen)-last-((x+1 < slen) ? 1 : 0)));
            if (((c = arg.front()) == arg.back()) && (quote.find(c) != std::string::npos))
                arg = arg.substr(1,arg.size()-2);
            list.push_back(arg);
            last = x;
        }
        else
            x++;
    }
    std::string* split = new std::string[list.size()];
    count = 0;
    for (auto it = list.begin(), ite = list.end();it != ite;++it)
        split[count++] = *it;
    return split;
}
#endif

#ifdef SPLITSTRINGANY
std::string* splitStringAny(const std::string &src, const std::string &delim, int &count, const std::string &quote = "\"'“`")
{
    if (src.size() == 0)
        return nullptr;
    std::vector<std::string> list;
    bool open = false;
    char c;
    for (size_t x = 0, slen = src.size(), last = 0;x < slen;)
    {
        if (quote.find((c = src.at(x))) != std::string::npos)
        {
            open = !open;
            if ((open) && (src.find(c,x+1) == std::string::npos))
                open = false;
        }
        if ((!open) && ((delim.find(c) != std::string::npos) || (x+1 >= slen)))
        {
            std::string arg = nospace(src.substr(last,(++x)-last-((x+1 < slen) ? 1 : 0)));
            if (((c = arg.front()) == arg.back()) && (quote.find(c) != std::string::npos))
                arg = arg.substr(1,arg.size()-2);
            list.push_back(arg);
            last = x;
        }
        else
            x++;
    }
    std::string* split = new std::string[list.size()];
    count = 0;
    for (auto it = list.begin(), ite = list.end();it != ite;++it)
        split[count++] = *it;
    return split;
}
#endif

#ifdef RANDOMNEGATIVEEMOJI
std::string randomNegativeEmoji()
{
    static std::string emojis[] = {
                                    "%F0%9F%9A%B7",
                                    "%F0%9F%A4%B9",
                                    "%F0%9F%A4%94",
                                    "%F0%9F%98%92",
                                    "%F0%9F%98%A0",
                                    "%F0%9F%A4%90",
                                    "%F0%9F%91%8E",
                                    "%F0%9F%92%83",
                                    "%F0%9F%A4%92",
                                    "%F0%9F%98%B4",
                                    "%F0%9F%98%AA",
                                    "%F0%9F%94%9E",
                                    "%E2%9B%94",
                                    "%F0%9F%9A%AB",
                                    "%E2%9D%8C"
                                  };
    return emojis[mtrand(0,14)];
}
#endif

#ifdef HEXIFY
std::string hexify(const std::string &in)
{
    std::string out;
    out.reserve(in.size()*3);
    for (auto it = in.rbegin(), ite = in.rend();it != ite;++it)
    {
        unsigned char n = (unsigned char)(*it);
        do
        {
            out += "0123456789ABCDEF"[n & 15];
            n >>= 4;
        } while (n);
    }
    out = std::string(out.rbegin(),out.rend());
    std::string result;
    size_t pos = 0;
    for (auto it = out.begin(), ite = out.end();it != ite;++it,++pos)
    {
        if ((pos % 2) == 0)
            result += '%';
        result += *it;
    }
    return std::move(result);
}
#endif











#endif

#endif

