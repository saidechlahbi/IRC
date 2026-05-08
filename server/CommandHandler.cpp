// CommandHandler.cpp

#include "CommandHandler.hpp"
#include <sstream>
#include <iostream>

CommandHandler::CommandHandler() {}
CommandHandler::~CommandHandler() {}

void CommandHandler::registerCommand(const std::string& name, CommandFunc func) {
    _commands[name] = func;
}

void CommandHandler::parseAndDispatch(Server& server, Client& client, const std::string& line) {
    std::string command = extractCommand(line);
    std::string params = extractParams(line);

    std::map<std::string, CommandFunc>::iterator it = _commands.find(command);
    if (it != _commands.end())
        it->second(server, client, params);
    else
        std::cout << "Unknown command: " << command << std::endl;
}

// Utility: Splits "COMMAND param param" into command/params for simplicity
std::string CommandHandler::extractCommand(const std::string& line) const {
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;
    for (size_t i = 0; i < cmd.size(); ++i) cmd[i] = toupper(cmd[i]);
    return cmd;
}
std::string CommandHandler::extractParams(const std::string& line) const {
    size_t pos = line.find(' ');
    if (pos == std::string::npos)
        return "";
    return line.substr(pos + 1);
}