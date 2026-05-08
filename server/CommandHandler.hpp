// CommandHandler.hpp

#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#include <map>
#include <functional>

class Server;
class Client;

typedef void (*CommandFunc)(Server&, Client&, const std::string& params);

class CommandHandler {
public:
    CommandHandler();
    ~CommandHandler();

    void registerCommand(const std::string& name, CommandFunc func);
    void parseAndDispatch(Server& server, Client& client, const std::string& line);

private:
    std::map<std::string, CommandFunc> _commands;
    std::string extractCommand(const std::string& line) const;
    std::string extractParams(const std::string& line) const;
};


#endif /*COMMANDHANDLER_HPP*/