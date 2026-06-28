#pragma once

#include <string>

class Reservation {
private:
    std::string username;
    std::string studentName;
    std::string roomId;
    int seatId;
    std::string timeSlot;

public:
    Reservation();
    Reservation(const std::string& username,
                const std::string& studentName,
                const std::string& roomId,
                int seatId,
                const std::string& timeSlot);

    std::string getUsername() const;
    std::string getStudentName() const;
    std::string getRoomId() const;
    int getSeatId() const;
    std::string getTimeSlot() const;

    void showInfo() const;

    bool operator==(const Reservation& other) const;
};
