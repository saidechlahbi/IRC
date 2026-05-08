#ifndef SERVER_HPP
#define SERVER_HPP

// Server.hpp

#include <vector>
#include <map>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;
class Server {
public:
    Server(int port, const std::string& password);
    ~Server();

    void run(); // Main loop (poll/select calls)
    void acceptNewClient();
    void removeClient(int fd);

    // Client management
    Client* getClient(int fd);
    std::vector<Client*>& getClients();

    // Channel management
    Channel* getChannel(const std::string& name);
    void addChannel(const std::string& name, Channel* channel);
    void removeChannel(const std::string& name);

    // Command processing
    void handleCommand(Client& client, const std::string& line);

    // Getters
    const std::string& getPassword() const;
    int getPort() const;

private:
    int _serverSocket;
    int _port;
    std::string _password;

    std::vector<Client*> _clients;                      // All connected clients
    std::map<std::string, Channel*> _channels;          // Active channels by name

    // Helper functions, e.g., socket setup, poll management, etc.

    // Disallow copy
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /*SERVER_HPP*/