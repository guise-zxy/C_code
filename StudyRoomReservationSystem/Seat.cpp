#include "Seat.h"

Seat::Seat() : seatId(0) {}

Seat::Seat(int seatId) : seatId(seatId) {}

int Seat::getSeatId() const {
    return seatId;
}
