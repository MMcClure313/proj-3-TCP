#include <iostream>
#include "UserHandler.hpp" 
using namespace std;

int main() {
    UserHandler handler("users.txt");

    cout << "=== Register Users ===\n";
    bool r1 = handler.registerUser("alice", "pass123");
    bool r2 = handler.registerUser("bob", "hunter2");
    bool r3 = handler.registerUser("alice", "differentpass"); // duplicate

    cout << "Alice registration: " << (r1 ? "Success" : "Failed") << endl;
    cout << "Bob registration: "   << (r2 ? "Success" : "Failed") << endl;
    cout << "Duplicate Alice: "    << (r3 ? "Success" : "Failed") << endl;


    cout << "=== Login Users===\n";
    bool li1 = handler.loginUser("alice", "pass123", 1);
    bool li2 = handler.loginUser("bob", "hunter2", 2);

    cout << "Alice login: " << (li1 ? "Success" : "Failed") << endl;
    cout << "Bob login: "   << (li2 ? "Success" : "Failed") << endl;

    cout << "Current Active Users:\n";
    for (const auto& user : handler.getActiveUsers()) {
        cout << "- " << user.getUsername() << " (socket " << user.getSocketNo() << ")\n";
    }

    cout << "\n=== Change Password ===\n";
    bool cp1 = handler.changePassword("alice", "newpass321");
    bool cp2 = handler.changePassword("nonexistent", "nothing");

    cout << "Alice password change: " << (cp1 ? "Success" : "Failed") << endl;
    cout << "Fake user change: "      << (cp2 ? "Success" : "Failed") << endl;

    cout << "\n=== Logout Users ===\n";
    bool l1 = handler.logoutUser("alice");
    bool l2 = handler.logoutUser("charlie"); // Not logged in

    cout << "Alice logout: " << (l1 ? "Success" : "Failed") << endl;
    cout << "Charlie logout: " << (l2 ? "Success" : "Failed") << endl;

    cout << "\n=== Active Users After Logout ===\n";
    for (const auto& user : handler.getActiveUsers()) {
        cout << "- " << user.getUsername() << " (socket " << user.getSocketNo() << ")\n";
    }

    return 0;
}