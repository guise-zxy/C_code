#include "StudyRoom.h"

#include <iostream>

#include "Reservation.h"

StudyRoom::StudyRoom() : roomId(""), seats() {}

StudyRoom::StudyRoom(const std::string& roomId, int seatCount) : roomId(roomId), seats() {
    for (int i = 1; i <= seatCount; ++i) {
        seats.emplace_back(i);
    }
}

std::string StudyRoom::getRoomId() const {
    return roomId;
}

std::vector<Seat>& StudyRoom::getSeats() {
    return seats;
}

const std::vector<Seat>& StudyRoom::getSeats() const {
    return seats;
}

int StudyRoom::getSeatCount() const {
    return static_cast<int>(seats.size());
}

bool StudyRoom::hasSeat(int seatId) const {
    for (const auto& seat : seats) {
        if (seat.getSeatId() == seatId) {
            return true;
        }
    }
    return false;
}

int StudyRoom::countFreeSeats(const std::vector<Reservation>& reservations, const std::string& timeSlot) const {
    int freeCount = 0;

    for (const auto& seat : seats) {
        bool reserved = false;
        for (const auto& reservation : reservations) {
            if (reservation.getRoomId() == roomId &&
                reservation.getSeatId() == seat.getSeatId() &&
                reservation.getTimeSlot() == timeSlot) {
                reserved = true;
                break;
            }
        }

        if (!reserved) {
            ++freeCount;
        }
    }

    return freeCount;
}

void StudyRoom::showSeats(const std::vector<Reservation>& reservations, const std::string& timeSlot) const {
    std::cout << '\n' << roomId << " room " << timeSlot << " seat status:\n";

    for (std::size_t i = 0; i < seats.size(); ++i) {
        bool reserved = false;
        for (const auto& reservation : reservations) {
            if (reservation.getRoomId() == roomId &&
                reservation.getSeatId() == seats[i].getSeatId() &&
                reservation.getTimeSlot() == timeSlot) {
                reserved = true;
                break;
            }
        }

        std::cout << seats[i].getSeatId() << '[' << (reserved ? "Reserved" : "Free") << "]  ";
        if ((i + 1) % 4 == 0) {
            std::cout << '\n';
        }
    }

    if (!seats.empty() && seats.size() % 4 != 0) {
        std::cout << '\n';
    }
}
