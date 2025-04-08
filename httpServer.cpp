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
#include <cstring>      // Cpp string manip
#include <unistd.h>     // POSIX OS function
#include <arpa/inet.h>  // Socket programming
#include <sstream>      // String stream Manip
#include <fstream>      // file reading
#include <sys/types.h>  // socket data types
#include <sys/socket.h> // socket functions




using namespace std;


// Read a file and return data
string readFile(const string& filename){
    ifstream file(filename, ios::binary); //Convert data to binary, as it can handle all file types c
    if(file){
        ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();    //return file data as string
    }

    return "";  //return empty string if file DNE
}

// Handle a single client request
void handleClient(int clientSocket){
    char buffer[1024]; //buffer size is 2^10 for multifaceted uses 
    memset(buffer,0,1024);  //take the memory area pointed to by the buffer, then set each byte in the memory area to 0 for 1024 bytes

    // Recieve HTTP request from client
    read(clientSocket, buffer, 1024);
    cout << "Recieved A Request!\n" << buffer << endl;

    // Parse request to extract requested file
    string request(buffer); //convert c-style char array to c++ string, better for manip
    size_t pos = request.find("GET");
    size_t post_pos = request.find("POST");
    //Method for If the request is a GET
    if(pos != string::npos){
        size_t start = request.find("/", pos)+1;
        size_t end = request.find(" ", start);
        string filename = request.substr(start, end - start);

        //If file name not included, default to index.html
        if (filename.empty() || filename == "/"){
            filename = "index.html";
        }

        //Read requested file data
        string fileData = readFile(filename);

        if (!fileData.empty()) {
            // File exists, send HTTP 200 response
            string response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Length: " + to_string(fileData.size()) + "\r\n";
            response += "Content-Type: text/html\r\n\r\n"; // Assume all files are HTML
            response += fileData;

            send(clientSocket, response.c_str(), response.size(), 0);
            cout << "Served file: " << filename << endl;
        } else {
            // File does not exist, send HTTP 404 response
            string response = "HTTP/1.1 404 Not Found\r\n";
            response += "Content-Length: 49\r\n";
            response += "Content-Type: text/html\r\n\r\n";
            response += "<html><body><h1>404 Not Found</h1></body></html>";

            send(clientSocket, response.c_str(), response.size(), 0);
            cout << "File not found: " << filename << endl;
        }
    } else if(post_pos != string::npos){
        size_t inputPos = request.find("input=")+6;
        size_t endPos = request.find(" ", inputPos);

        string input = request.substr(inputPos, endPos-inputPos);

        cout << endl << "Client says: ( " << input << " ) using a POST request" << endl << endl;

        // Respond to POST request
        string fileData = readFile("index.html");

            string response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Length: " + to_string(fileData.size()) + "\r\n";
            response += "Content-Type: text/html\r\n\r\n"; // Assume all files are HTML
            response += fileData;

        send(clientSocket, response.c_str(), response.size(), 0);

    }else {
        // Invalid request, send HTTP 400 Bad Request
        string response = "HTTP/1.1 400 Bad Request\r\n";
        response += "Content-Length: 45\r\n";
        response += "Content-Type: text/html\r\n\r\n";
        response += "<html><body><h1>400 Bad Request</h1></body></html>";

        send(clientSocket, response.c_str(), response.size(), 0);
        cout << "Invalid HTTP request received." << endl;
    }

    // Close the client connection
    close(clientSocket);
    cout << "Client disconnected." << endl;
}


int main(){
    
    int tcp_client_socket;
    int tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_server_socket == -1) {
        cerr << "Failed to create socket." << endl;
    }
    cout << "Server socket created" << endl << endl;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;     //IPv4 family
    serverAddress.sin_port = htons(60060); //Passing the port number.
    serverAddress.sin_addr.s_addr = INADDR_ANY; // accept connections from any IP address

    //Binds socket to ip address and port.
    bind(tcp_server_socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    cout << "server created" << endl << endl;
    //Listens for a client
    cout << "server listening for a connection" << endl << endl;
    listen(tcp_server_socket, 5);

    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientSize = sizeof(clientAddress);
        tcp_client_socket = accept(tcp_server_socket, (struct sockaddr*)&clientAddress, &clientSize);

        if (tcp_client_socket < 0) {
            cerr << "Failed to accept client connection." << endl;
            continue;
        }
        cout << "Client connected." << endl;

        // Handle the client request
        handleClient(tcp_client_socket);
    }


    /* //Artifact code to simply test sending the webpage in
    while(true){
        int tcp_client_socket;
        tcp_client_socket = accept(tcp_server_socket, NULL, NULL);

        if(tcp_client_socket < 0){
            cerr << "Accept failed";
            continue;
        }

        const char* htmlResponse =
            "HTTP/1.1 200 OK\r\n"           //Header
            "Content-Type: text/html\r\n"   //Header
            "\r\n"
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head><title>Simple Server</title></head>\n"
            "<body>\n"
            "<h1>Hello from the TCP Server!</h1>\n"
            "<p>This is a simple HTML page served by the server.</p>\n"
            "</body>\n"
            "</html>\n";

        send(tcp_client_socket, htmlResponse, strlen(htmlResponse), 0);
*/
    //}

    close(tcp_client_socket);

    return 0;

 }