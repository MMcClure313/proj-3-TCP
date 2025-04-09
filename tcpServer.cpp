/**
 * COP4635, SYSNET2
 * @authors Matthew McClure, Hunter Seth Mercer
 * @date 01/25/2025
 * 
 * Simple application used to start up a TCP server used to serve a TCP Client (webpage) the index.html, testPressence.html and img.jpg included in the project folder.
 * Coded on a Windows 10 machine using Visual Studio code and ran through an Ubantu WSL terminal integreated in VSCode. 
 * 
 * This application is ran using the Ubuntu WSL and going to a browser of choice and typing `localhost:60060` in while the server is running.
 * To close the server, use `ctrl+c` in the terminal.
 * 
 * There is a text input field that sends a message to the server that is recieved. 
 * 
 */


#include <iostream>    
#include <thread>
#include <cstring>      // Cpp string manip
#include <unistd.h>     // POSIX OS function
#include <arpa/inet.h>  // Socket programming
#include <sstream>      // String stream Manip
#include <fstream>      // file reading
#include <sys/types.h>  // socket data types
#include <sys/socket.h> // socket functions
#include "UserHandler.hpp" // User Handling Header




using namespace std;

class TCPServer {
public:
    TCPServer(int port) : server_port(port), userHandler("users.txt"){

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            std::cerr << "Socket creation failed" << endl;
            exit(EXIT_FAILURE);
        }

        // Setup server address
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(server_port);
        server_address.sin_addr.s_addr = INADDR_ANY;  // Listen on all available interfaces
    }

    void start() {
        //Binding the socket to the specified address and port.
        if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))== -1){
            cerr << "Bind failure" << endl;
            exit(1);
        }


        //Listening to connections
        if(listen(server_socket, 5)==-1){
            cerr << "Listening failed?" << endl;
            exit(1);
        }

        cout << "Server listening on port " << server_port << "..." << endl;

        //Client accepting loop
        while(true){
            int client_socket = accept(server_socket, nullptr, nullptr);
            if(client_socket == -1){
                cerr << "Client connection failed" << endl;
                continue;
            }

            thread(&TCPServer::handle_client, this, client_socket).detach();

        }
    }


private:
    int server_socket;
    struct sockaddr_in server_address;
    int server_port;
    string response;
    UserHandler userHandler;

    void handle_client(int client_socket){
        char buffer[512];
        cout << "server reading client socket as: " << client_socket << endl;
        while(true) {
            // Recieving data from cleint
            memset(buffer, 0, sizeof(buffer));
            int received = recv(client_socket, buffer, sizeof(buffer)-1, 0);
            if(received > 0) {
                string message(buffer, received);
                cout << endl << "Received: " << message << endl << endl;


                //Auto response for testing
                //string response = "Server: " + message;

                istringstream iss(message);
                string command;
                iss >> command;

                if (command == "REGISTER") {
                   string username, password;
                   iss >> username >> password;
                   bool success = userHandler.registerUser(username, password);
                   response = success ? "Registration successful." : "Username already exists.";
                }
                else if (command == "LOGIN") {
                    string username, password;
                    iss >> username >> password;
                    bool success = userHandler.loginUser(username, password, client_socket);
                    response = success ? "Login successful." : "Invalid username or password.";
                }
                else if (command == "LOGOUT") {
                    std::string username = userHandler.getUsernameBySocket(client_socket);
                    if (!username.empty()) {
                        bool success = userHandler.logoutUser(username);
                        response = success ? "Logout successful." : "Logout failed.";
                    } else {
                    response = "You are not logged in.";
                    }
                }
                else if (command == "SUBSCRIBE") {
                    std::string location;
                    iss >> location;

                    std::string username = userHandler.getUsernameBySocket(client_socket);
                    if (!username.empty()) {
                        bool success = userHandler.subscribeUser(username, location);
                        response = success ? "Subscribed to " + location + "." : "Subscription failed.";
                    } else {
                        response = "You must be logged in to subscribe.";
                    }
                }
                else if (command == "QUIT") {
                    std::string username = userHandler.getUsernameBySocket(client_socket);
                    if (!username.empty()) {
                        userHandler.logoutUser(username);  // Optional, already handled on disconnect
                    }
                    cout << "User requested quit. Closing connection for socket: " << client_socket << endl;
                    break;
                }
                else if (command == "UNSUBSCRIBE") {
                    int index;
                    iss >> index;

                    std::string username = userHandler.getUsernameBySocket(client_socket);
                    if (!username.empty()) {
                        bool success = userHandler.unsubscribeUser(username, index);
                        response = success ? "Unsubscribed from location index " + std::to_string(index) + "." : "Unsubscribe failed.";
                    } else {
                        response = "You must be logged in to unsubscribe.";
                    }
                }
                else if (command == "CHANGE_PASSWORD") {
                    std::string newPassword;
                    iss >> newPassword;

                    std::string username = userHandler.getUsernameBySocket(client_socket);
                    if (!username.empty()) {
                        bool success = userHandler.changePassword(username, newPassword);
                        response = success ? "Password changed successfully." : "Password change failed.";
                    } else {
                        response = "You must be logged in to change your password.";
                    }
                }
                else if (command == "SEE_SUBSCRIPTIONS") {
                    std::string username = userHandler.getUsernameBySocket(client_socket);
                    if (!username.empty()) {
                        auto locations = userHandler.getUserLocations(username);
                        if (locations.empty()) {
                            response = "You are not subscribed to any locations.";
                        } else {
                            std::ostringstream oss;
                            oss << "Your Subscriptions:\n";
                            for (size_t i = 0; i < locations.size(); ++i) {
                                oss << i << ". " << locations[i] << "\n";
                            }
                            response = oss.str();
                        }
                    } else {
                        response = "You must be logged in to see subscriptions.";
                    }
                }
                else {
                    response = "Unknown command. Use LOGIN or REGISTER.";
                }


                send(client_socket, response.c_str(), response.length(), 0);

            } else if (received == 0) {
                cout << "Client disconnect" << endl;
                break;
            } else{
                cerr << "Recv failed" << endl;
                break;
            }
        }
        close(client_socket);
    }

};

int main(){
    int port = 12345;
    TCPServer server(port);
    server.start();
    
    return 0;

 }
