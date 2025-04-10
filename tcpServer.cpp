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
#include <cstring>         // Cpp string manip
#include <unistd.h>        // POSIX OS function
#include <arpa/inet.h>     // Socket programming
#include <sstream>         // String stream Manip
#include <fstream>         // file reading
#include <sys/types.h>     // socket data types
#include <sys/socket.h>    // socket functions
#include "UserHandler.hpp" // User Handling Header

using namespace std;

class TCPServer
{
public:
    TCPServer(int port) : server_port(port), userHandler("users.txt")
    {

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1)
        {
            std::cerr << "Socket creation failed\n" << endl;
            exit(EXIT_FAILURE);
        }

        // Setup server address
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(server_port);
        server_address.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
    }

    void start()
    {
        // Binding the socket to the specified address and port.
        if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
        {
            cerr << "Bind failure" << endl;
            exit(1);
        }

        // Listening to connections
        if (listen(server_socket, 5) == -1)
        {
            cerr << "Listening failed?" << endl;
            exit(1);
        }

        cout << "Server listening on port " << server_port << "..." << endl;

        // Client accepting loop
        while (true)
        {
            int client_socket = accept(server_socket, nullptr, nullptr);
            if (client_socket == -1)
            {
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

    /**
     * This handles everything and is the handler for every client. Meaning that this is in threads, and we should probably do mutexes.
     */
    void handle_client(int client_socket)
    {
        char buffer[512];
        cout << "server reading client socket as: " << client_socket << endl;
        string welcomeMsg = "Welcome\n Press 1 to Login\n Press 2 to Register\n Type 'exit' to Quit\n\n";
        string loggedInMsg = "1 Subscribe to a location\n2 Unsubscribe from a location\n3 Send a message to a location\n4 send a private message\n5 see all the locations you are subscribed to\n6 see all the online Users\n7 See the last 10 messages\n8 change password\n9 logout\n0 Quit\n\n";
        send(client_socket, welcomeMsg.c_str(), welcomeMsg.length(), 0);
        while (true)
        {
            // Recieving data from cleint
            memset(buffer, 0, sizeof(buffer));
            int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            if (received > 0)
            {
                string message(buffer, received);
                // cout << endl << "Received: " << message << endl << endl;

                // Auto response for testing
                // string response = "Server: " + message;

                istringstream iss(message);
                string command;
                iss >> command;

                string username = userHandler.getUsernameBySocket(client_socket);
                bool isLoggedIn = !username.empty();
                // cout << username << "\t" << isLoggedIn << endl;
                response = "";
                if (!isLoggedIn)
                {
                    if (command == "1")
                    {
                        if (!login(client_socket))
                        {
                            response = welcomeMsg;
                            send(client_socket, response.c_str(), response.length(), 0);
                        }
                        else
                        {
                            response = loggedInMsg;
                            send(client_socket, response.c_str(), response.length(), 0);
                        }
                    }
                    else if (command == "2")
                    {
                        registerUser(client_socket);
                        response = welcomeMsg;
                        send(client_socket, response.c_str(), response.length(), 0);
                    }
                    else if (command == "exit")
                    {
                        cout << "Client on socket " << client_socket << " disconnected" << endl;
                        break;
                    }
                    else
                    {
                        response = welcomeMsg;
                        send(client_socket, response.c_str(), response.length(), 0);
                    }
                }
                else// USER IS NOW LOGGED IN, SHOW LOGGED IN FUNCTIONS
                { 
                    // LOGOUT LINE
                    if (command == "9")
                    {
                        std::string username = userHandler.getUsernameBySocket(client_socket);
                        if (!username.empty())
                        {
                            bool success = userHandler.logoutUser(username);
                            response = success ? "Logout successful." : "Logout failed.";
                            response = welcomeMsg;
                        }
                    }
                    // SUBSCRIBE TO A LOCATION LINE
                    else if (command == "1")
                    {
                            handleSubscribe(client_socket);
                    }
                    // UNSUBSCRIBE TO A LOCATION LINE
                    else if (command == "2")
                    {
                            handleUnsubscribe(client_socket);
                    }
                    // QUIT LINE
                    else if (command == "0")
                    {
                        std::string username = userHandler.getUsernameBySocket(client_socket);
                        if (!username.empty())
                        {
                            userHandler.logoutUser(username); // Optional, already handled on disconnect
                        }
                        cout << "User requested quit. Closing connection for socket: " << client_socket << endl;
                        break;
                    }
                    //CHANGE PASSWORD LINE
                    else if (command == "8")
                    {
                        changePassword(client_socket);
                    }
                    //SEE ALL LOCATIONS SUBSCRIBED TO
                    else if (command == "5")
                    {
                        listSubscriptions(client_socket);
                    }
                    //NONE OF THE ABOVE
                    else
                    {
                        response = "Command either unimplemented or unknown.\n";
                    }
                    send(client_socket, response.c_str(), response.length(), 0);

                    string username = userHandler.getUsernameBySocket(client_socket);
                    bool isLoggedIn = !username.empty();


                    response = isLoggedIn? loggedInMsg : "";
                    send(client_socket, response.c_str(), response.length(), 0);
                }
            }
            else if (received == 0)
            {
                cout << "Client disconnect" << endl;
                break;
            }
            else
            {
                cerr << "Recv failed" << endl;
                break;
            }
        }
        close(client_socket);
    }

    /**
     * Login helper function that steps the user through logging in
     */
    bool login(int client_socket)
    {
        string prompt = "Enter your username: ";
        send(client_socket, prompt.c_str(), prompt.length(), 0);

        // Receive username
        char buffer[512];
        memset(buffer, 0, sizeof(buffer));
        int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0)
        {
            cerr << "Failed to receive username" << endl;
            return false;
        }
        string username(buffer);

        prompt = "Enter your password: ";
        send(client_socket, prompt.c_str(), prompt.length(), 0);

        // Receive password
        memset(buffer, 0, sizeof(buffer));
        received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0)
        {
            cerr << "Failed to receive password" << endl;
            return false;
        }
        string password(buffer);

        // Now try to login the user
        bool success = userHandler.loginUser(username, password, client_socket);
        string response = success ? "Login successful.\n" : "Invalid username or password.\n";
        send(client_socket, response.c_str(), response.length(), 0);
        return success;
    }

    /**
     * Register login function that steps the user through registering
     */
    void registerUser(int client_socket)
    {
        string prompt = "Enter a username: ";
        send(client_socket, prompt.c_str(), prompt.length(), 0);

        // Receive username
        char buffer[512];
        memset(buffer, 0, sizeof(buffer));
        int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0)
        {
            cerr << "Failed to receive username" << endl;
            return;
        }
        string username(buffer);

        prompt = "Enter a password: ";
        send(client_socket, prompt.c_str(), prompt.length(), 0);

        // Receive password
        memset(buffer, 0, sizeof(buffer));
        received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0)
        {
            cerr << "Failed to receive password" << endl;
            return;
        }
        string password(buffer);

        // Now try to register the user
        bool success = userHandler.registerUser(username, password);
        string response = success ? "Registration successful.\n\n" : "Username already exists.\n\n";
        send(client_socket, response.c_str(), response.length(), 0);
    }
    /**
     * 
     */
    void handleLogout(int client_socket){
        string username = userHandler.getUsernameBySocket(client_socket);
        if (!username.empty()) {
            bool success = userHandler.logoutUser(username);
            response = success ? "Logout successful." : "Logout failed.";
        } 
    }

    /**
     * Subscription logic
     */
    bool handleSubscribe(int client_socket) {
        string prompt = "Insert the location you want to subscribe to:\n";
        send(client_socket, prompt.c_str(), prompt.length(), 0);


        // Receive location
        char buffer[512];
        memset(buffer, 0, sizeof(buffer));
        int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0)
        {
            cerr << "Failed to receive location" << endl;
            return false;
        }
        string location(buffer);

    
        std::string username = userHandler.getUsernameBySocket(client_socket);
        if (!username.empty()) {

            bool success = userHandler.subscribeUser(username, location);
            response = success ? "Subscribed to " + location + "." : "Subscription failed.";
            response += "\n";
            return success;
        }
        return false;
    }

    /**
     * Unsubscription logic
     */
    bool handleUnsubscribe(int client_socket){
        listSubscriptions(client_socket);
        response += "Input the number of the location you wish to remove : ";
        send(client_socket, response.c_str(), response.length(), 0);
        response = "";

        char buffer[512];
        memset(buffer, 0, sizeof(buffer));
        int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0)
        {
            cerr << "Failed to receive location" << endl;
            return false;
        }
        int index;
        try{
            index = stoi(buffer);
        } catch (const exception& e) {
            cerr << "Invalid input: " << buffer << endl;
            return false;
        }
        std::string username = userHandler.getUsernameBySocket(client_socket);
        bool success = userHandler.unsubscribeUser(username, index);

            response = success ? "Successfuly unsubscribed from index: " + index : "Unable to unsubscribe from index: " + index;
            response =  + "\n";
        send(client_socket, response.c_str(), response.length(), 0);
        response = "";
        return success;
    }

    /**
     * Listing subscriptions.
     */
    void listSubscriptions(int client_socket){
        std::string username = userHandler.getUsernameBySocket(client_socket);
        if (!username.empty())
        {
            auto locations = userHandler.getUserLocations(username);
            {
                std::ostringstream oss;
                oss << "Your Subscriptions:\n";
                for (size_t i = 0; i < locations.size(); ++i)
                {
                    oss << i << ". " << locations[i] << "\n";
                }
                response = oss.str() +"\n";
            }
        }
    }

    bool changePassword(int client_socket){
        response += "Choose what your new password will be : ";
        send(client_socket, response.c_str(), response.length(), 0);
        response = "";

        char buffer[512];
        memset(buffer, 0, sizeof(buffer));
        int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0)
        {
            cerr << "Failed to receive location" << endl;
            return false;
        }
        string newPassword(buffer);




        std::string username = userHandler.getUsernameBySocket(client_socket);

            bool success = userHandler.changePassword(username, newPassword);
            response = success ? "Password changed successfully.\n" : "Password change failed.\n";
        
            return success;
    }

};

int main()
{
    int port = 60010;
    TCPServer server(port);
    server.start();

    return 0;
}
