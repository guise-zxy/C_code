# StudyRoomReservationSystem

## 项目简介

这是一个基于 `C++17` 的控制台课程项目框架，项目名称为“校园自习室预约管理系统”。

当前版本重点是：

- 固定类名、文件名和函数接口
- 搭建适合 4 人并行开发的项目骨架
- 保证项目可以编译、启动，并展示基础菜单流程

当前版本不是完整系统实现，不依赖真实文件读写，不包含完整预约、取消、统计等业务逻辑。

## 当前已完成内容

- 创建了全部 `.h` / `.cpp` 源码文件
- 按文档固定了 `User`、`Student`、`Admin`、`Seat`、`StudyRoom`、`Reservation`、`SystemManager` 接口
- `main.cpp` 仅负责启动 `SystemManager`
- `SystemManager::initData()` 使用硬编码样例数据初始化
- 已提供主菜单、学生菜单、管理员菜单显示
- 已提供自习室列表显示、按时间段查看座位状态、简易预约冲突校验
- 已创建 `data/users.txt`、`data/rooms.txt`、`data/reservations.txt` 样例文件

## 文件结构

```txt
StudyRoomReservationSystem/
|-- main.cpp
|-- User.h
|-- User.cpp
|-- Student.h
|-- Student.cpp
|-- Admin.h
|-- Admin.cpp
|-- Seat.h
|-- Seat.cpp
|-- StudyRoom.h
|-- StudyRoom.cpp
|-- Reservation.h
|-- Reservation.cpp
|-- SystemManager.h
|-- SystemManager.cpp
|-- README.md
|-- data/
|   |-- users.txt
|   |-- rooms.txt
|   `-- reservations.txt
`-- docs/
    `-- superpowers/
        `-- plans/
```

## 类设计说明

### User

- 抽象基类
- 保存公共用户信息：`username`、`password`、`name`
- 定义纯虚函数 `showMenu()`

### Student

- 继承 `User`
- 负责学生身份和学生菜单显示

### Admin

- 继承 `User`
- 负责管理员身份和管理员菜单显示

### Seat

- 表示单个座位
- 当前仅保存 `seatId`

### StudyRoom

- 表示单个自习室
- 保存 `roomId` 和 `seats`
- 提供座位存在性判断和座位状态展示接口

### Reservation

- 表示一条预约记录
- 已实现 `operator==`，用于判断同一自习室、同一座位、同一时间段是否冲突

### SystemManager

- 负责系统主流程调度
- 当前只保留基础样例数据初始化、菜单流程和少量占位逻辑
- 文件读写函数接口已预留，但未接入真实实现

## 编译方式

在项目根目录执行：

```bash
g++ -std=c++17 main.cpp User.cpp Student.cpp Admin.cpp Seat.cpp StudyRoom.cpp Reservation.cpp SystemManager.cpp -o app
```

## 运行方式

Linux / macOS:

```bash
./app
```

## 演示说明

课堂演示步骤和测试脚本见：

- [DEMO_GUIDE.md](D:/Projects/校园自习室预约管理系统/scaffold_work/StudyRoomReservationSystem/DEMO_GUIDE.md)
- [VS_RUN_GUIDE.md](D:/Projects/校园自习室预约管理系统/scaffold_work/StudyRoomReservationSystem/VS_RUN_GUIDE.md)

Windows:

```bash
app.exe
```

## 小组分工建议

- 成员 A：`main.cpp`、`SystemManager.h`、`SystemManager.cpp`
- 成员 B：`User.*`、`Student.*`、`Admin.*`
- 成员 C：`Seat.*`、`StudyRoom.*`
- 成员 D：`Reservation.*`、`data/*`、后续文件读写逻辑

## 当前仍待完成的接口 / 功能

- `cancelReservation(Student& student)`
- `showStatistics()`
- `loadUsers()`
- `loadRooms()`
- `loadReservations()`
- `saveUsers()`
- `saveRooms()`
- `saveReservations()`

后续还可以继续补充：

- 更完整的预约与取消流程
- 管理员统计功能
- 启动加载文件与预约后保存文件
- 更严格的输入校验

## 说明

当前代码以“先搭框架、固定接口、保证可编译”为首要目标，适合小组成员在此基础上并行补全各自模块。
