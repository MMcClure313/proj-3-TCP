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

bool User::addLocation(const string& location){
    if(location.empty()) return false;
    Locations.push_back(location);
    return true;
}

//When the users want to remove a location, they will type in the index of the location they want to remove.
bool User::removeLocation(const int& index){
    if(index >= 0 && index < static_cast<int>(Locations.size())) {
        Locations.erase(Locations.begin() + index);
        return true;
    }
    return false;
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