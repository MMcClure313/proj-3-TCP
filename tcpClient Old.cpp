/**
 * COP4635, SYSNET2
 * @authors Matthew McClure, Hunter Seth Mercer
 * @date 04/08/2025
 * 
 * 
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





int main(int argc, char *argv[]){
    //Specify program usage and debug statements.
    if(argc <= 2){
        cout << "please run the application like so: './httpClient <serverIdentifier:portNumber> <fileToRequest/messageToSend>" << endl;
        return -1;
    } else{
        //cout << argv[0] << "\t" << argv[1] << "\t" << argv[2] << endl;
    }


    return 0;
}
