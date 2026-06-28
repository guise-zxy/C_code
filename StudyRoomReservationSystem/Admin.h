#pragma once

#include "User.h"

class Admin : public User {
public:
    Admin();
    Admin(const std::string& username, const std::string& password, const std::string& name);

    std::string getRole() const override;
    void showMenu() const override;
};
