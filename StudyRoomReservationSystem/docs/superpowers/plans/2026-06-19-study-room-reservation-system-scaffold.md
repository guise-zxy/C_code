# Study Room Reservation System Scaffold Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a C++17 console-project skeleton with fixed interfaces, sample data, placeholder business methods, and a verified compilable entry point for parallel team development.

**Architecture:** Keep each domain class in its own header/source pair and centralize only menu orchestration inside `SystemManager`. Use hardcoded sample data for startup, keep file I/O APIs declared but empty, and limit implemented behavior to menu flow plus minimal display helpers.

**Tech Stack:** C++17, standard library (`iostream`, `vector`, `string`), g++

---

### Task 1: Create project layout

**Files:**
- Create: `main.cpp`
- Create: `User.h`
- Create: `User.cpp`
- Create: `Student.h`
- Create: `Student.cpp`
- Create: `Admin.h`
- Create: `Admin.cpp`
- Create: `Seat.h`
- Create: `Seat.cpp`
- Create: `StudyRoom.h`
- Create: `StudyRoom.cpp`
- Create: `Reservation.h`
- Create: `Reservation.cpp`
- Create: `SystemManager.h`
- Create: `SystemManager.cpp`
- Create: `README.md`
- Create: `data/users.txt`
- Create: `data/rooms.txt`
- Create: `data/reservations.txt`

- [ ] Add all required files using the exact names from the project spec.
- [ ] Use `#pragma once` in every header.
- [ ] Keep one class per header/source pair to reduce merge conflicts during team work.

### Task 2: Define fixed class interfaces

**Files:**
- Modify: `User.h`
- Modify: `Student.h`
- Modify: `Admin.h`
- Modify: `Seat.h`
- Modify: `StudyRoom.h`
- Modify: `Reservation.h`
- Modify: `SystemManager.h`

- [ ] Declare all member variables exactly as required by the spec.
- [ ] Declare all constructors, getters, menus, and manager methods using the exact signatures from the spec.
- [ ] Forward declare `Reservation` in `StudyRoom.h` to avoid circular include issues.

### Task 3: Add minimal implementations

**Files:**
- Modify: `User.cpp`
- Modify: `Student.cpp`
- Modify: `Admin.cpp`
- Modify: `Seat.cpp`
- Modify: `StudyRoom.cpp`
- Modify: `Reservation.cpp`
- Modify: `SystemManager.cpp`

- [ ] Implement constructors and getters for every class so the project links cleanly.
- [ ] Implement `showMenu()` for student and admin with Chinese console output.
- [ ] Implement `SystemManager::initData()` with only hardcoded sample users and rooms.
- [ ] Keep file I/O methods empty and avoid loading data from `data/*.txt`.
- [ ] Limit business logic to basic menu flow, room listing, seat display, and lightweight placeholder behavior.

### Task 4: Add documentation and sample data

**Files:**
- Modify: `README.md`
- Modify: `data/users.txt`
- Modify: `data/rooms.txt`
- Modify: `data/reservations.txt`

- [ ] Document project purpose, file structure, compile command, run command, and unfinished interfaces.
- [ ] Add sample rows to `users.txt` and `rooms.txt`.
- [ ] Add an empty or sample reservation row to `reservations.txt` without wiring runtime logic to it.

### Task 5: Verify compilation

**Files:**
- Test: project root

- [ ] Run `g++ -std=c++17 main.cpp User.cpp Student.cpp Admin.cpp Seat.cpp StudyRoom.cpp Reservation.cpp SystemManager.cpp -o app`.
- [ ] Confirm exit code `0`.
- [ ] Report the verified build result and call out which methods remain placeholders.
