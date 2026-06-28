#pragma once

class Seat {
private:
    int seatId;

public:
    Seat();
    Seat(int seatId);

    int getSeatId() const;
};
