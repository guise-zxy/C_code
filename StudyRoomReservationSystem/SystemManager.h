#pragma once

#include <string>
#include <vector>

#include "Admin.h"
#include "Reservation.h"
#include "Student.h"
#include "StudyRoom.h"

class SystemManager {
private:
    std::vector<Student> students;
    std::vector<Admin> admins;
    std::vector<StudyRoom> rooms;
    std::vector<Reservation> reservations;

    std::vector<std::string> getTimeSlots() const;
    StudyRoom* findRoomById(const std::string& roomId);
    const StudyRoom* findRoomById(const std::string& roomId) const;

public:
    SystemManager();

    void initData();

    void mainMenu();
    void studentLogin();
    void adminLogin();

    void studentMenu(Student& student);
    void adminMenu(Admin& admin);

    void showRooms();
    void showSeatsByTime();
    void reserveSeat(Student& student);
    void showMyReservations(const Student& student);
    void cancelReservation(Student& student);

    void showAllReservations();
    void showStatistics();

    void loadUsers();
    void loadRooms();
    void loadReservations();

    void saveUsers();
    void saveRooms();
    void saveReservations();
};
