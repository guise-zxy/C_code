#pragma once

#include <string>

class User {
protected:
    std::string username;
    std::string password;
    std::string name;

public:
    User();
    User(const std::string& username, const std::string& password, const std::string& name);
    virtual ~User() = default;

    std::string getUsername() const;
    std::string getPassword() const;
    std::string getName() const;

    bool checkPassword(const std::string& input) const;

    virtual std::string getRole() const = 0;
    virtual void showMenu() const = 0;
};
