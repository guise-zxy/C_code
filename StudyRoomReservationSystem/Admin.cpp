#include "Admin.h"

#include <iostream>

Admin::Admin() : User() {}

Admin::Admin(const std::string& username, const std::string& password, const std::string& name)
    : User(username, password, name) {}

std::string Admin::getRole() const {
    return "admin";
}

void Admin::showMenu() const {
    std::cout << "\n===== Admin Menu =====\n";
    std::cout << "1. Show all reservations\n";
    std::cout << "2. Show statistics\n";
    std::cout << "0. Logout\n";
    std::cout << "Choose: ";
}
