# COP-Http-Project-2

## https://uwf.instructure.com/courses/58481/assignments/1101019


@authors: Matthew McClure, Hunter Seth Mercer

A client project with the previously made HTTP Server from the first project where a client is able to make calls and requests to the server to serve up files on request.

The application was tested by running make in a ubantu wsl terminal
using `make` will generate the httpServer and httpClient executable, to be executed using `./httpServer` and `./httpClient` in the terminal respectively.

the server is opened on port 60060, if there as another application that specifically uses 60060, it will not work.

With the server running, the httpClient needs the argument of the server to connect to in order to begin functionality.

in order to run the command, there is ./httpClient <address:port> <FileToGet>.

With httpServer running, 0.0.0.0:60060 and 127.0.0.1:60060 are valid arguments for <address:port> if the httpServer is running on your machine

the <FileToGet> argument is actually all arguments past the address and port, so you can do ./httpClient <address:port> <arg2> <arg3> <arg4> ...

Proper file to get commands could get `GET /index.html` `GET /img.jpg`. Any other files within the same directory of httpServer can be requested. 

The httpClient will see the server response in the terminal. It will show all the file details, and show the status of creating and deleting the sockets and connection to the httpServer. If the server can't be connected to, it will not be connected to.



`make clean` will remove the httpServer and httpClient from your directory.