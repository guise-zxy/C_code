#include "User.h"

User::User() : username(""), password(""), name("") {}

User::User(const std::string& username, const std::string& password, const std::string& name)
    : username(username), password(password), name(name) {}

std::string User::getUsername() const {
    return username;
}

std::string User::getPassword() const {
    return password;
}

std::string User::getName() const {
    return name;
}

bool User::checkPassword(const std::string& input) const {
    return password == input;
}
