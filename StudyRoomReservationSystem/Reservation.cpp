#include "Reservation.h"

#include <iostream>

Reservation::Reservation() : username(""), studentName(""), roomId(""), seatId(0), timeSlot("") {}

Reservation::Reservation(const std::string& username,
                         const std::string& studentName,
                         const std::string& roomId,
                         int seatId,
                         const std::string& timeSlot)
    : username(username), studentName(studentName), roomId(roomId), seatId(seatId), timeSlot(timeSlot) {}

std::string Reservation::getUsername() const {
    return username;
}

std::string Reservation::getStudentName() const {
    return studentName;
}

std::string Reservation::getRoomId() const {
    return roomId;
}

int Reservation::getSeatId() const {
    return seatId;
}

std::string Reservation::getTimeSlot() const {
    return timeSlot;
}

void Reservation::showInfo() const {
    std::cout << "username: " << username
              << ", name: " << studentName
              << ", room: " << roomId
              << ", seat: " << seatId
              << ", time slot: " << timeSlot << '\n';
}

bool Reservation::operator==(const Reservation& other) const {
    return roomId == other.roomId && seatId == other.seatId && timeSlot == other.timeSlot;
}
