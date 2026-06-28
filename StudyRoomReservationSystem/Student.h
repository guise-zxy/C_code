#pragma once

#include "User.h"

class Student : public User {
public:
    Student();
    Student(const std::string& username, const std::string& password, const std::string& name);

    std::string getRole() const override;
    void showMenu() const override;
};
