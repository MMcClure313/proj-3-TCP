# COP-Http-Project-3

## https://uwf.instructure.com/courses/58481/assignments/1130121


@authors: Matthew McClure, Hunter Seth Mercer

A client server project that will allow users to connect to a hosted server, login or register, and use services provided by the server application.

In order to construct both the tcpClient and the tcpServer, run 
`make`

If you want one or the other, you can run
`make tcpClient`
or
`make tcpServer`

For debugging purposes for user functions, there is a test main included, that can be constructed with 
`make testMain`


Once you have done the `make` command, you can enter `./tcpServer` on one terminal, and `./tcpClient` on another terminal to connect the client to the terminal, the host will be localhost, which will require 127.0.0.1.
For the port number, we have chosen 60010.

Once your tcpClient has connected to the server, the server should recognize a client on a specified port. On the client you can register or login. 
When registering, the text data will go to Users.txt.
When Logging in, use a user from Users.txt. The text is formatted such that it displays :<username> <password>

Once logged in, there will be multiple services available to you. There are only 6 available out of the 10, those being
* 1. Subscribe to a location
* 2. Unsubscribe from a location
* 5. see all the locations you are subscribed to
* 8. change password
* 9. logout
* 0. Quit

When the TCP client disconnects from the server, if you give it one input, it'll resort to asking for another host and port.

`make clean` will remove the tcpServer and tcpClient from your directory.
