#include "SystemManager.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

namespace {
const char* USERS_FILE = "data/users.txt";
const char* ROOMS_FILE = "data/rooms.txt";
const char* RESERVATIONS_FILE = "data/reservations.txt";

bool readInt(int& value) {
    if (std::cin >> value) {
        return true;
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Input must be an integer. Please try again.\n";
    return false;
}
}  // namespace

SystemManager::SystemManager() {
    initData();
}

void SystemManager::initData() {
    loadUsers();
    loadRooms();
    loadReservations();

    if (students.empty()) {
        students.emplace_back("stu001", "123456", "ZhangSan");
        students.emplace_back("stu002", "123456", "LiSi");
    }
    if (admins.empty()) {
        admins.emplace_back("admin", "123456", "AdminUser");
    }
    if (rooms.empty()) {
        rooms.emplace_back("A101", 8);
        rooms.emplace_back("A102", 10);
        rooms.emplace_back("B201", 12);
    }
}

std::vector<std::string> SystemManager::getTimeSlots() const {
    return {
        "08:00-10:00",
        "10:00-12:00",
        "14:00-16:00",
        "16:00-18:00",
        "19:00-21:00"
    };
}

StudyRoom* SystemManager::findRoomById(const std::string& roomId) {
    for (auto& room : rooms) {
        if (room.getRoomId() == roomId) {
            return &room;
        }
    }
    return nullptr;
}

const StudyRoom* SystemManager::findRoomById(const std::string& roomId) const {
    for (const auto& room : rooms) {
        if (room.getRoomId() == roomId) {
            return &room;
        }
    }
    return nullptr;
}

void SystemManager::mainMenu() {
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n===== Study Room Reservation System =====\n";
        std::cout << "1. Student login\n";
        std::cout << "2. Admin login\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose: ";

        if (!readInt(choice)) {
            continue;
        }

        switch (choice) {
        case 1:
            studentLogin();
            break;
        case 2:
            adminLogin();
            break;
        case 0:
            std::cout << "System exited.\n";
            break;
        default:
            std::cout << "Invalid input. Please try again.\n";
            break;
        }
    }
}

void SystemManager::studentLogin() {
    std::string usernameInput;
    std::string passwordInput;

    std::cout << "\nEnter student username: ";
    std::cin >> usernameInput;
    std::cout << "Enter password: ";
    std::cin >> passwordInput;

    for (auto& student : students) {
        if (student.getUsername() == usernameInput && student.checkPassword(passwordInput)) {
            std::cout << "Login successful. Welcome, " << student.getName() << ".\n";
            studentMenu(student);
            return;
        }
    }

    std::cout << "Student username or password is incorrect.\n";
}

void SystemManager::adminLogin() {
    std::string usernameInput;
    std::string passwordInput;

    std::cout << "\nEnter admin username: ";
    std::cin >> usernameInput;
    std::cout << "Enter password: ";
    std::cin >> passwordInput;

    for (auto& admin : admins) {
        if (admin.getUsername() == usernameInput && admin.checkPassword(passwordInput)) {
            std::cout << "Login successful. Welcome, " << admin.getName() << ".\n";
            adminMenu(admin);
            return;
        }
    }

    std::cout << "Admin username or password is incorrect.\n";
}

void SystemManager::studentMenu(Student& student) {
    int choice = -1;

    while (choice != 0) {
        student.showMenu();
        if (!readInt(choice)) {
            continue;
        }

        switch (choice) {
        case 1:
            showRooms();
            break;
        case 2:
            showSeatsByTime();
            break;
        case 3:
            reserveSeat(student);
            break;
        case 4:
            showMyReservations(student);
            break;
        case 5:
            cancelReservation(student);
            break;
        case 0:
            std::cout << "Student logged out.\n";
            break;
        default:
            std::cout << "Invalid input. Please try again.\n";
            break;
        }
    }
}

void SystemManager::adminMenu(Admin& admin) {
    int choice = -1;

    while (choice != 0) {
        admin.showMenu();
        if (!readInt(choice)) {
            continue;
        }

        switch (choice) {
        case 1:
            showAllReservations();
            break;
        case 2:
            showStatistics();
            break;
        case 0:
            std::cout << "Admin logged out.\n";
            break;
        default:
            std::cout << "Invalid input. Please try again.\n";
            break;
        }
    }
}

void SystemManager::showRooms() {
    std::cout << "\n===== Room List =====\n";
    for (const auto& room : rooms) {
        std::cout << "Room ID: " << room.getRoomId()
                  << ", Seat count: " << room.getSeatCount() << '\n';
    }
}

void SystemManager::showSeatsByTime() {
    std::string roomId;
    int slotChoice = 0;
    const auto timeSlots = getTimeSlots();

    std::cout << "\nEnter room ID: ";
    std::cin >> roomId;

    const StudyRoom* room = findRoomById(roomId);
    if (room == nullptr) {
        std::cout << "Room not found.\n";
        return;
    }

    std::cout << "Choose time slot:\n";
    for (std::size_t i = 0; i < timeSlots.size(); ++i) {
        std::cout << i + 1 << ". " << timeSlots[i] << '\n';
    }
    std::cout << "Enter slot number: ";
    if (!readInt(slotChoice)) {
        return;
    }

    if (slotChoice < 1 || slotChoice > static_cast<int>(timeSlots.size())) {
        std::cout << "Invalid time slot number.\n";
        return;
    }

    room->showSeats(reservations, timeSlots[slotChoice - 1]);
}

void SystemManager::reserveSeat(Student& student) {
    std::string roomId;
    int seatId = 0;
    int slotChoice = 0;
    const auto timeSlots = getTimeSlots();

    std::cout << "\n===== Reserve Seat =====\n";
    std::cout << "Enter room ID: ";
    std::cin >> roomId;

    StudyRoom* room = findRoomById(roomId);
    if (room == nullptr) {
        std::cout << "Room not found.\n";
        return;
    }

    std::cout << "Enter seat ID: ";
    if (!readInt(seatId)) {
        return;
    }
    if (!room->hasSeat(seatId)) {
        std::cout << "Seat ID does not exist.\n";
        return;
    }

    std::cout << "Choose time slot:\n";
    for (std::size_t i = 0; i < timeSlots.size(); ++i) {
        std::cout << i + 1 << ". " << timeSlots[i] << '\n';
    }
    std::cout << "Enter slot number: ";
    if (!readInt(slotChoice)) {
        return;
    }

    if (slotChoice < 1 || slotChoice > static_cast<int>(timeSlots.size())) {
        std::cout << "Invalid time slot number.\n";
        return;
    }

    Reservation newReservation(
        student.getUsername(),
        student.getName(),
        roomId,
        seatId,
        timeSlots[slotChoice - 1]
    );

    for (const auto& reservation : reservations) {
        if (reservation == newReservation) {
            std::cout << "Reservation failed: this seat is already reserved in the selected time slot.\n";
            return;
        }
    }

    reservations.push_back(newReservation);
    saveReservations();
    std::cout << "Reservation created successfully and saved to reservations.txt.\n";
}

void SystemManager::showMyReservations(const Student& student) {
    std::cout << "\n===== My Reservations =====\n";
    bool found = false;

    for (const auto& reservation : reservations) {
        if (reservation.getUsername() == student.getUsername()) {
            reservation.showInfo();
            found = true;
        }
    }

    if (!found) {
        std::cout << "No reservation records found.\n";
    }
}

void SystemManager::cancelReservation(Student& student) {
    std::vector<std::size_t> myReservationIndexes;

    std::cout << "\n===== Cancel Reservation =====\n";
    for (std::size_t i = 0; i < reservations.size(); ++i) {
        if (reservations[i].getUsername() == student.getUsername()) {
            std::cout << myReservationIndexes.size() + 1 << ". ";
            reservations[i].showInfo();
            myReservationIndexes.push_back(i);
        }
    }

    if (myReservationIndexes.empty()) {
        std::cout << "You have no reservation that can be cancelled.\n";
        return;
    }

    int choice = 0;
    std::cout << "Enter the reservation number to cancel (0 to return): ";
    if (!readInt(choice)) {
        return;
    }

    if (choice == 0) {
        return;
    }
    if (choice < 1 || choice > static_cast<int>(myReservationIndexes.size())) {
        std::cout << "Invalid reservation number.\n";
        return;
    }

    reservations.erase(reservations.begin() + myReservationIndexes[choice - 1]);
    saveReservations();
    std::cout << "Reservation cancelled and reservations.txt has been updated.\n";
}

void SystemManager::showAllReservations() {
    std::cout << "\n===== All Reservations =====\n";
    if (reservations.empty()) {
        std::cout << "No reservation records found.\n";
        return;
    }

    for (std::size_t i = 0; i < reservations.size(); ++i) {
        std::cout << i + 1 << ". ";
        reservations[i].showInfo();
    }
}

void SystemManager::showStatistics() {
    const auto timeSlots = getTimeSlots();

    std::cout << "\n===== Room Usage Statistics =====\n";
    std::cout << "Usage rate = reservation count / (seat count x "
              << timeSlots.size() << " fixed time slots)\n";

    for (const auto& room : rooms) {
        int reservationCount = 0;
        for (const auto& reservation : reservations) {
            if (reservation.getRoomId() == room.getRoomId()) {
                ++reservationCount;
            }
        }

        const int totalCapacity = room.getSeatCount() * static_cast<int>(timeSlots.size());
        const double usageRate = (totalCapacity == 0)
            ? 0.0
            : static_cast<double>(reservationCount) / totalCapacity * 100.0;

        std::cout << "Room " << room.getRoomId()
                  << " | reservations: " << reservationCount
                  << " | total reservable slots: " << totalCapacity
                  << " | usage rate: " << std::fixed << std::setprecision(2) << usageRate << "%\n";
    }
}

void SystemManager::loadUsers() {
    students.clear();
    admins.clear();

    std::ifstream input(USERS_FILE);
    if (!input.is_open()) {
        return;
    }

    std::string role;
    std::string username;
    std::string password;
    std::string name;

    while (input >> role >> username >> password >> name) {
        if (role == "student") {
            students.emplace_back(username, password, name);
        } else if (role == "admin") {
            admins.emplace_back(username, password, name);
        }
    }
}

void SystemManager::loadRooms() {
    rooms.clear();

    std::ifstream input(ROOMS_FILE);
    if (!input.is_open()) {
        return;
    }

    std::string roomId;
    int seatCount = 0;
    while (input >> roomId >> seatCount) {
        if (seatCount > 0) {
            rooms.emplace_back(roomId, seatCount);
        }
    }
}

void SystemManager::loadReservations() {
    reservations.clear();

    std::ifstream input(RESERVATIONS_FILE);
    if (!input.is_open()) {
        return;
    }

    std::string username;
    std::string studentName;
    std::string roomId;
    int seatId = 0;
    std::string timeSlot;

    while (input >> username >> studentName >> roomId >> seatId >> timeSlot) {
        reservations.emplace_back(username, studentName, roomId, seatId, timeSlot);
    }
}

void SystemManager::saveUsers() {
    std::ofstream output(USERS_FILE);
    if (!output.is_open()) {
        std::cout << "Failed to save users.txt.\n";
        return;
    }

    for (const auto& student : students) {
        output << "student " << student.getUsername() << ' '
               << student.getPassword() << ' ' << student.getName() << '\n';
    }
    for (const auto& admin : admins) {
        output << "admin " << admin.getUsername() << ' '
               << admin.getPassword() << ' ' << admin.getName() << '\n';
    }
}

void SystemManager::saveRooms() {
    std::ofstream output(ROOMS_FILE);
    if (!output.is_open()) {
        std::cout << "Failed to save rooms.txt.\n";
        return;
    }

    for (const auto& room : rooms) {
        output << room.getRoomId() << ' ' << room.getSeatCount() << '\n';
    }
}

void SystemManager::saveReservations() {
    std::ofstream output(RESERVATIONS_FILE);
    if (!output.is_open()) {
        std::cout << "Failed to save reservations.txt.\n";
        return;
    }

    for (const auto& reservation : reservations) {
        output << reservation.getUsername() << ' '
               << reservation.getStudentName() << ' '
               << reservation.getRoomId() << ' '
               << reservation.getSeatId() << ' '
               << reservation.getTimeSlot() << '\n';
    }
}
