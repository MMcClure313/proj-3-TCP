#include "User.hpp"

#include <algorithm>

/**
 * When the server is fresh, no message or location context is saved.
 */
User::User(const string& username, int socketNo){
    this->Username = username;
    this->SocketNo = socketNo;
}

vector<string> User::getAllLocations() const{
    return Locations;
}

void User::addLocation(const string& location){
    Locations.push_back(location);
}

//When the users want to remove a location, they will type in the index of the location they want to remove.
void User::removeLocation(const int& index){
    if(index >= 0 && index < Locations.size()) {
        Locations.erase(Locations.begin() + index);
    }
}

void User::addMessage(const string& message){
    if(messages.size() >= 10){
        messages.pop_back();
        messages.push_back(message);
    }
}

string User::getUsername() const{
    return Username;
}

int User::getSocketNo() const {
    return SocketNo;
}