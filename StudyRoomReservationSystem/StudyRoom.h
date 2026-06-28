#pragma once

#include <string>
#include <vector>

#include "Seat.h"

class Reservation;

class StudyRoom {
private:
    std::string roomId;
    std::vector<Seat> seats;

public:
    StudyRoom();
    StudyRoom(const std::string& roomId, int seatCount);

    std::string getRoomId() const;
    std::vector<Seat>& getSeats();
    const std::vector<Seat>& getSeats() const;
    int getSeatCount() const;

    bool hasSeat(int seatId) const;
    int countFreeSeats(const std::vector<Reservation>& reservations, const std::string& timeSlot) const;
    void showSeats(const std::vector<Reservation>& reservations, const std::string& timeSlot) const;
};
