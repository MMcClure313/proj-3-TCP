#include "UserHandler.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

/**
 * A seperate handler for the user objects and to read from Users.txt, as well as do input checking and validation.
 * This will be the shared resource by all the server threads to read and write from, meaning we have our favorite race conditions.
 * 
 * Because of that, this file requires locking on near every method within it. The individual threads (besides messaging) can freely modify
 * their own users alone, with locations and messaging, but if it is something to do with the shared database, it needs to be locked and unlocked
 * in accordance.
 */

 /**
  * Immediately opens the file and makes a map from the file
  */
 UserHandler::UserHandler(const string& userFile){
    this->fileName = userFile;
    loadUsersFromFile(userFile);
 }

 /**
 * This works because in Users.txt, we're simply storing it as username password, to allow for easy write and read access.
 */
void UserHandler::loadUsersFromFile(const string& filename){
    //Reads and updates from file
    ifstream file(filename);
    string line;
    while(getline (file, line)) {
        istringstream iss(line);
        string username, password;
        if (iss >> username >> password){
            storedUsers[username] = password;
        }
    }
 }

 /**
  * CRITICAL SECTION, REQUIRES LOCKS.
  */
bool UserHandler::registerUser(const string& username, const string& password){
    if(storedUsers.find(username) != storedUsers.end()) {
        return false; // case if the  username exists in the map.
    }

    storedUsers[username] = password;

    //File management, ios::app to append
    ofstream file("Users.txt", ios::app);
    if(file.is_open()) {
        file << username << " " << password << "\n";
        file.close();
        return true;
    } else {
        return false;
    }
}

 /**
  * CRITICAL SECTION
  */
bool UserHandler::changePassword(const string& username, const string& password){
    if (storedUsers.find(username) == storedUsers.end()) {
        return false;
    }

    storedUsers[username] = password;

    ofstream file("users.txt");
    if(!file.is_open()) return false;

    for(const auto& pair : storedUsers) {
        file << pair.first << " " << pair.second << "\n";
    }

    file.close();
    return true;

}


 /**
 * CRITICAL
 */
 bool UserHandler::loginUser(const string& username, const string& password, int socketNo) {
    // Check if username exists and password matches
    if (storedUsers.find(username) != storedUsers.end() &&
        storedUsers[username] == password) {

        User newUser(username, socketNo);
        activeUsers.push_back(newUser);
        return true;
    }
    return false;
}
 
/**
 * CRITICAL
 */
 bool UserHandler::logoutUser(const string& username){
    for (auto it = activeUsers.begin(); it != activeUsers.end(); ++it){
        if(it->getUsername() == username){
            activeUsers.erase(it);
            return true;
        }
    }
    return false;
}

vector<User>& UserHandler::getActiveUsers() {
    return activeUsers;
}

std::string UserHandler::getUsernameBySocket(int socketNo){
    for(const auto& user : activeUsers){
        if(user.getSocketNo() == socketNo){
            return user.getUsername();
        }
    }
    return " ";
}

bool UserHandler::subscribeUser(const std::string& username, const std::string& location) {
    for (auto& user : activeUsers) {
        if (user.getUsername() == username) {
            user.addLocation(location);
            return true;
        }
    }
    return false;
}

bool UserHandler::unsubscribeUser(const std::string& username, int index) {
    for (auto& user : activeUsers) {
        if (user.getUsername() == username) {
            user.removeLocation(index);
            return true;
        }
    }
    return false;
}

std::vector<std::string> UserHandler::getUserLocations(const std::string& username) {
    for (auto& user : activeUsers) {
        if (user.getUsername() == username) {
            return user.getAllLocations();
        }
    }
    return {};
}
