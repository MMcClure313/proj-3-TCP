/**
 * COP4635, SYSNET2
 * @authors Matthew McClure, Hunter Seth Mercer
 * @date 04/08/2025
 *
 * An application used to connect to the tcpServer from project 3, and request functionality and messages as directed by the user.
 * Coded on a Windows 10 machine using Visual Studio code and ran through an Ubantu WSL terminal integreated in VSCode.
 *
 * To close the client, you press ctrl+c in the terminal.
 */

#include <iostream>
#include <stdio.h>  //Standard library
#include <stdlib.h> //Standard library
#include <thread>

#include <string.h>

#include <sys/socket.h> //API and definitions for the sockets
#include <sys/types.h>  //more definitions
#include <netinet/in.h> //Structures to store address information
#include <arpa/inet.h>  // For inet_pton
#include <unistd.h>

using namespace std;

class TCPClient
{
public:
    /**
     * All constructor does is take the host, and set up all it needs in order
     * to connect to a server as specified by the parameters.
     */
    TCPClient(const string &host, int port) : server_ip(host), server_port(port)
    {
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1)
        {
            cerr << "Socket creation failed\n";
            exit(0);
        }
        cout << "Socket created\n";
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(server_port);
        server_address.sin_addr.s_addr = inet_addr(server_ip.c_str());
    }

    /**
     * When you make a TCPClient object, the host and port are already specified.
     * We generated a seperate function to handle the connection, and go ahead and
     * set up all the connection params to quickly connect on this function call.
     */
    bool connect_to_server()
    {
        if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
        {
            cerr << "Connection failed\n";
            return false;
        }
        cout << "Connected to server: " << server_ip << ":" << server_port << endl;
        return true;
    }

    void send_data(const string &data)
    {
        if (send(client_socket, data.c_str(), data.length(), 0) == -1)
        {
            cerr << "Send failed\n";
        }
        else
        {
            //cout << "Sent: " << data << endl; // DEBUG
        }
    }

    void receive_data()
    {
        char buffer[512]; // Keeping it small considering the amount of data is limited to just text.
        while (true)
        {
            memset(buffer, 0, sizeof(buffer));
            int recieved = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

            if (recieved > 0)
            {
                cout << " " << string(buffer, recieved);
                cout.flush();
            }
            else if (recieved == 0)
            {
                cout << "Connection closed by server\n";
                running = false;
                break;
            }
            else
            {
                cerr << "Recv failed\n";
                running = false;
                break;
            }
        }
    }

    void start_receiving()
    {
        receive_thread = thread(&TCPClient::receive_data, this);
    }

    bool is_running() const
    {
        return running;
    }

    ~TCPClient()
    { // Cleanup, ensure that all threads are done.
        if (receive_thread.joinable())
        {
            receive_thread.join();
        }
        close(client_socket);
    }

    /**
     * These are where the client variables live.
     */
private:
    int client_socket;
    struct sockaddr_in server_address;
    std::string server_ip;
    int server_port;
    thread receive_thread;
    bool running = true;
};

int main()
{
    string host;
    string strPort;
    int port;

    while (true)
    {
        host = "";
        cout << "Enter hostname : ";
        getline(cin, host);

        strPort = "";
        cout << "Enter port : ";
        getline(cin, strPort);

        try{
            port = stoi(strPort);
        } catch(const invalid_argument& e){
            cout << "Invalid port. Please enter a number.\n";
            continue;
        }

        TCPClient client(host, port);
        if (client.connect_to_server())
        {
            cout << "Connected\n\n";
            client.start_receiving();
            string input;
            while (true)
            {
                std::getline(std::cin, input);
                client.send_data(input);
                if (!client.is_running())
                {
                    cout << "Disconnected from server\n";
                    break;
                }
            }
        } else{
            cout << "Couldn't connect (for sake of project 3, port is 60010, and host is 127.0.0.1)\n";
        }
    }

    return 0;
}
