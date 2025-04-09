#ifndef USER_HANDLER_HPP
#define USER_HANDLER_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include "User.hpp"

using namespace std;

class UserHandler{
    public:
        UserHandler(const string& userFile);

        bool registerUser(const string& username, const string& password);

        bool loginUser(const string& username, const string& password, int socketNo);

        bool logoutUser(const string& username);

        bool changePassword(const string& username, const string& password);

        vector<User>& getActiveUsers();
        

    private: 

    unordered_map<string, string> storedUsers;

    vector<User> activeUsers;

    string fileName;

    void loadUsersFromFile(const string& filename);

};

#endif