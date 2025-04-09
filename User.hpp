#ifndef USER_HPP
#define USER_HPP

#include <vector>
#include <string>

using namespace std;

class User{
    public:
    //Constructor
    User(const string& username, int socketNo);
    //Getter of all locations
    vector<string> getAllLocations() const;
    
    //Add and remove locations
    void addLocation(const string& location);
    void removeLocation(const int& index);

    //add message (Will check if messages > 10, then remove)
    void addMessage(const string& message);

    // Getter for the username
    string getUsername() const;

    // Getter for the socket number
    int getSocketNo() const;


    private:
        string Username;
        vector<string> Locations;
        int SocketNo;
        vector<string> messages;
};

#endif