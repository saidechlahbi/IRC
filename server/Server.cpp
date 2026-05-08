// Server.cpp

#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <poll.h>

Server::Server(int port, const std::string& password)
    : _port(port), _password(password) {
    // Create server socket
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
        throw std::runtime_error("Failed to create socket");

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Bind failed");

    if (listen(_serverSocket, 10) < 0)
        throw std::runtime_error("Listen failed");

    std::cout << "Server started on port " << _port << std::endl;
}

Server::~Server() {
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete *it;
    }
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
}

void Server::run() {
    struct pollfd fds[1024];
    int nfds = 1;
    fds[0].fd = _serverSocket;
    fds[0].events = POLLIN;

    // Add all client FDs
    while (true) {
        nfds = 1;
        for (size_t i = 0; i < _clients.size(); ++i) {
            fds[nfds].fd = _clients[i]->getFd();
            fds[nfds].events = POLLIN;
            ++nfds;
        }

        int pollResult = poll(fds, nfds, -1);
        if (pollResult < 0)
            throw std::runtime_error("Poll error");

        if (fds[0].revents & POLLIN) {
            acceptNewClient();
        }

        // Check clients for input
        for (int i = 1; i < nfds; ++i) {
            if (fds[i].revents & POLLIN) {
                char buffer[512] = {0};
                ssize_t bytes = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytes <= 0) {
                    removeClient(fds[i].fd);
                } else {
                    Client* c = getClient(fds[i].fd);
                    if (c) {
                        c->getBuffer().append(buffer, bytes);

                        // Example: Dispatch one line at a time
                        size_t pos;
                        while ((pos = c->getBuffer().find('\n')) != std::string::npos) {
                            std::string line = c->getBuffer().substr(0, pos);
                            c->getBuffer().erase(0, pos + 1);
                            handleCommand(*c, line);
                        }
                    }
                }
            }
        }
    }
}

void Server::acceptNewClient() {
    sockaddr_in client_addr = {};
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(_serverSocket, (struct sockaddr*)&client_addr, &len);
    if (client_fd < 0)
        return;
    Client* client = new Client(client_fd);
    _clients.push_back(client);
    std::cout << "New client connected: fd=" << client_fd << std::endl;
}

void Server::removeClient(int fd) {
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->getFd() == fd) {
            close(fd);
            delete *it;
            _clients.erase(it);
            std::cout << "Client disconnected: fd=" << fd << std::endl;
            break;
        }
    }
}

Client* Server::getClient(int fd) {
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i]->getFd() == fd)
            return _clients[i];
    return NULL;
}

std::vector<Client*>& Server::getClients() {
    return _clients;
}

Channel* Server::getChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
        return it->second;
    return NULL;
}

void Server::addChannel(const std::string& name, Channel* channel) {
    _channels[name] = channel;
}

void Server::removeChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        delete it->second;
        _channels.erase(it);
    }
}

void Server::handleCommand(Client& client, const std::string& line) {
    // Hand off to CommandHandler, e.g.:
    // _commandHandler.parseAndDispatch(client, line);
    (void)client;
    std::cout << "Received command from client: " << line << std::endl;
}

const std::string& Server::getPassword() const { return _password; }
int Server::getPort() const { return _port; }














// #include <iostream> 
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// int main ()
// {
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1)
//     {
//         std::cout << "Error :socket failed " <<std::endl;
//         exit(1);
//     }
//     struct sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(6667);
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof (server_addr)) == -1)
//     {
//         std::cout << "Error : bind failed" << std::endl;
//         exit(1);
//     }
//     if (listen(server_fd, 10) == -1)
//     {
//         std::cout << "ERROR: listen failed" << std::endl;
//         exit(1);
//     }
//     struct sockaddr_in client_addr;
//     socklen_t client_len = sizeof(client_addr);
//     int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
//     if (client_fd == -1)
//     {
//         std::cout << "Error: accept failed" << std::endl;
//         exit(1);
//     }
//     std::cout << "new client from:" << ":"
//     << inet_ntoa(client_addr.sin_addr)
//     << ntohs(client_addr.sin_port)
//     << std::endl;
// }