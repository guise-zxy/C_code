# Visual Studio 运行说明

## 适用目录

正式演示目录：

- `D:\Projects\StudyRoomReservationSystem`

如果你是给老师现场演示，建议直接使用这个目录。

## 演示前确认

请先确认电脑已经安装：

- Visual Studio 2022
- 已勾选 `使用 C++ 的桌面开发`

还要确认项目目录下存在这些文件：

- `main.cpp`
- `User.cpp`
- `Student.cpp`
- `Admin.cpp`
- `Seat.cpp`
- `StudyRoom.cpp`
- `Reservation.cpp`
- `SystemManager.cpp`
- `data\users.txt`
- `data\rooms.txt`
- `data\reservations.txt`

## 方法一：用 Visual Studio 直接打开文件夹运行

这是最推荐的方式，最省事。

### 第一步：打开项目目录

1. 打开 Visual Studio
2. 点击 `打开本地文件夹`
3. 选择目录：

```txt
D:\Projects\StudyRoomReservationSystem
```

### 第二步：确认源码文件都被识别

打开后，在右侧或“解决方案资源管理器/文件夹视图”里确认能看到：

- `main.cpp`
- `User.cpp`
- `Student.cpp`
- `Admin.cpp`
- `Seat.cpp`
- `StudyRoom.cpp`
- `Reservation.cpp`
- `SystemManager.cpp`

### 第三步：设置启动文件

如果 Visual Studio 没有自动识别启动项：

1. 打开 `main.cpp`
2. 在编辑器里右键
3. 如果有类似 `设为启动项`、`Set as Startup Item` 的选项，就点击它

因为程序入口就在：

```cpp
int main() {
    SystemManager system;
    system.mainMenu();
    return 0;
}
```

### 第四步：运行

点击顶部绿色三角按钮，或者按：

- `Ctrl + F5`：不调试直接运行
- `F5`：调试运行

建议你课堂演示时优先用：

- `Ctrl + F5`

这样控制台窗口不会一闪而过。

## 方法二：新建一个 Visual C++ 空项目后把文件加进去

如果你的 Visual Studio 对“打开本地文件夹”支持不稳定，可以用这个方法。

### 第一步：新建项目

1. 打开 Visual Studio
2. 点击 `创建新项目`
3. 选择 `空项目`
4. 项目名称随便起，例如：

```txt
StudyRoomReservationSystemDemo
```

### 第二步：把源码加进项目

在“解决方案资源管理器”中：

1. 右键 `源文件`
2. 选择 `添加 -> 现有项`
3. 把下面这些 `.cpp` 文件全部加入：

- `main.cpp`
- `User.cpp`
- `Student.cpp`
- `Admin.cpp`
- `Seat.cpp`
- `StudyRoom.cpp`
- `Reservation.cpp`
- `SystemManager.cpp`

然后：

1. 右键 `头文件`
2. 选择 `添加 -> 现有项`
3. 把下面这些 `.h` 文件全部加入：

- `User.h`
- `Student.h`
- `Admin.h`
- `Seat.h`
- `StudyRoom.h`
- `Reservation.h`
- `SystemManager.h`

### 第三步：确认运行目录

程序运行时会读取：

- `data/users.txt`
- `data/rooms.txt`
- `data/reservations.txt`

所以你需要确保运行时工作目录能找到 `data` 文件夹。

最稳的方法是：

1. 把整个 `data` 文件夹复制到生成的可执行文件旁边
2. 或者把项目的“调试工作目录”设置为：

```txt
D:\Projects\StudyRoomReservationSystem
```

如果不设置，程序可能找不到数据文件，但它仍会回退到默认样例数据。

## 推荐的项目属性设置

如果你是“空项目手动添加文件”的方式，建议检查以下设置：

### C++ 语言标准

项目属性里确认：

- `C/C++ -> 语言 -> C++ 语言标准`
- 选择 `ISO C++17 Standard (/std:c++17)`

### 源文件编码

建议源码使用 UTF-8。

如果你担心中文显示问题，可以在项目属性里确认：

- 使用 `/utf-8`

## 如果想在 VS 终端里手动编译

可以在 Visual Studio 的开发者命令环境里运行：

```bat
cl /EHsc /std:c++17 /utf-8 main.cpp User.cpp Student.cpp Admin.cpp Seat.cpp StudyRoom.cpp Reservation.cpp SystemManager.cpp /Fe:app.exe
```

然后运行：

```bat
app.exe
```

## 课堂演示推荐操作

建议你给老师演示时按这个顺序来：

1. 打开项目
2. 运行程序
3. 学生登录：`stu001 / 123456`
4. 查看座位状态
5. 演示重复预约失败
6. 演示新预约成功
7. 查看我的预约
8. 取消刚刚新增的预约
9. 退出学生端
10. 管理员登录：`admin / 123456`
11. 查看所有预约
12. 查看统计信息

详细输入步骤可以看：

- [DEMO_GUIDE.md](D:/Projects/校园自习室预约管理系统/scaffold_work/StudyRoomReservationSystem/DEMO_GUIDE.md)

## 常见问题

### 1. 点运行后提示找不到数据文件

先检查项目目录下是否有：

- `data\users.txt`
- `data\rooms.txt`
- `data\reservations.txt`

再检查当前工作目录是不是项目根目录。

### 2. 控制台一闪而过

请用：

- `Ctrl + F5`

而不是直接双击运行。

### 3. 中文显示乱码

优先保证：

- 源码文件使用 UTF-8
- 使用 `/utf-8`

### 4. Visual Studio 报找不到 `main`

一般是因为：

- `main.cpp` 没加入项目
- 启动项不是这个项目

### 5. 运行时内容不对

先检查：

- `data\reservations.txt` 是否被上一次演示改动过

如需恢复成初始样例，可以改成：

```txt
stu001 ZhangSan A101 3 19:00-21:00
```

## 最后建议

上课前你可以先自己在 Visual Studio 里完整跑一遍，再按 [DEMO_GUIDE.md](D:/Projects/校园自习室预约管理系统/scaffold_work/StudyRoomReservationSystem/DEMO_GUIDE.md) 预演一次。
