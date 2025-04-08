/**
 * COP4635, SYSNET2
 * @authors Matthew McClure, Hunter Seth Mercer
 * @date 02/08/2025
 * 
 * An application used to connect to the httpServer from project 1, and request files and send messages as needed.
 * Coded on a Windows 10 machine using Visual Studio code and ran through an Ubantu WSL terminal integreated in VSCode. 
 * 
 * To close the client, you press ctrl+c in the terminal.
 */


#include <iostream>
#include <stdio.h> //Standard library
#include <stdlib.h> //Standard library

#include <string.h> // For strlen, strcpy, strtok

#include <sys/socket.h> //API and definitions for the sockets
#include <sys/types.h> //more definitions
#include <netinet/in.h> //Structures to store address information
#include <arpa/inet.h> // For inet_pton
#include <sys/types.h>  // socket data types
#include <sys/socket.h> // socket functions
#include <unistd.h>

using namespace std;

int findSymbol(const char arr[], char target){

    if (arr == nullptr) return -1; 

    for(int i = 0; arr[i] != '\0'; i++){
        if(arr[i] == target){
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[]){
    //Specify program usage and debug statements.
    if(argc <= 2){
        cout << "please run the application like so: './httpClient <serverIdentifier:portNumber> <fileToRequest/messageToSend>" << endl;
        return -1;
    } else{
        //cout << argv[0] << "\t" << argv[1] << "\t" << argv[2] << endl;
    }

    //If the argument is ./httpClient 0.0.0.0:60060 GET index.html then the argc is 4 and we need to combine the two in order to get the right message.
    char fullMessage[256] = {0}; //setting the message size to be 256
    for(int i = 2; i < argc; i++){
        if (strlen(fullMessage) + strlen(argv[i]) + 1 >= sizeof(fullMessage)) {
            cerr << "Error: Message too long!" << endl;
            return -1;
        }
        
        if (i > 2) strncat(fullMessage, " ", sizeof(fullMessage) - strlen(fullMessage)); // Add space
        strncat(fullMessage, argv[i], sizeof(fullMessage) - strlen(fullMessage) - 1);
    }

        //Separate address from the port
        int separator = findSymbol(argv[1], ':');
        if(separator == -1){
            cout << "Please indicate the server as <serverAddress>:<port#>" << endl;
            return -1;
        }

            // Extract IP and Port
            char serverIP[separator + 1];
            strncpy(serverIP, argv[1], separator);
            serverIP[separator] = '\0';
            int port = atoi(argv[1] + separator + 1);

            cout << "Client ready. \n\n";
            cout << "Connecting to " << serverIP << " on port " << port << "\n";








        while(true){
            //Because the client opens with a message, we HAVE to check for the first message.
            if(fullMessage[0] == '\0'){
                cout << "enter request (or 'exit' to quit)\nReminder, running a GET requires GET /<filename>:  ";
                cin.getline(fullMessage, sizeof(fullMessage));
            }

            if(strcmp(fullMessage, "exit") == 0){
                cout << "exiting application\n\n";
                break;
            }

            int tcp_client_socket; //Socket descriptor
            tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0); //Calling the socket function - args: socket domain, socket stream type, TCP protocol (default)
            struct sockaddr_in tcp_server_address;
            tcp_server_address.sin_family = AF_INET; //Structure Fields' definition: Sets the address family of the address the client would connect to

            tcp_server_address.sin_port = htons(60060); //Specify and pass the port number to connect - converting in right network byte order

            if(inet_pton(AF_INET, serverIP, &tcp_server_address.sin_addr) <= 0){ //connects to the ip address specified. <= 0 means unsupported.
                cerr << ("Invalid or unsupported address") << endl;
                return -1;
            }

            int connection_status = connect(tcp_client_socket, (struct sockaddr *) &tcp_server_address, sizeof(tcp_server_address)); //params: which socket, castfor address to the specific structure type, size of address
        
            if (connection_status == -1){
            //return value of 0 means all okay, -1 means a problem
            printf(" Problem connecting to the socket! Sorry!! \n");
            }


        cout << "Sending message: " << fullMessage << " to the server";

        //Send the loaded message.
        send(tcp_client_socket, fullMessage, sizeof(fullMessage), 0);
        
        //As we know what the message sent from the server buffer is, we just set it to the same value there.
        char tcp_server_response[1024];
        recv(tcp_client_socket, &tcp_server_response, sizeof(tcp_server_response), 0);
        // params: where (socket), what (string), how much - size of the server response, flags (0)
        //Output, as received from Server
        cout << "\n\n Server says: \n" << tcp_server_response << "\n\n";
        //closing the socket
        close(tcp_client_socket);

        memset(tcp_server_response, 0, sizeof(fullMessage));  //Clear both messages
        memset(fullMessage, 0, sizeof(fullMessage));
        }


    return 0;
}
