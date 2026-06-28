#include "Student.h"

#include <iostream>

Student::Student() : User() {}

Student::Student(const std::string& username, const std::string& password, const std::string& name)
    : User(username, password, name) {}

std::string Student::getRole() const {
    return "student";
}

void Student::showMenu() const {
    std::cout << "\n===== Student Menu =====\n";
    std::cout << "1. Show rooms\n";
    std::cout << "2. Show seat status by time slot\n";
    std::cout << "3. Reserve seat\n";
    std::cout << "4. Show my reservations\n";
    std::cout << "5. Cancel reservation\n";
    std::cout << "0. Logout\n";
    std::cout << "Choose: ";
}
