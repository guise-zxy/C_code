# 2026-08-03 WS63E SLE AT验证日志

```txt
项目：
星闪多锚点 RSSI 室内定位

测试日期：
2026-08-03

测试阶段：
出厂固件 AT 功能验证

节点命名：
T1：移动标签节点
A1：固定锚点节点

串口映射：
T1 → COM10
A1 → COM8

连接方式：
T1、A1 均通过 USB Hub 同时连接电脑并保持供电。

串口配置：
波特率：115200
数据位：8
停止位：1
校验：无
流控：无
命令结尾：CRLF
```

## 一、前置测试

5块 WS63E 开发板均完成以下基础测试：

```txt
AT
```

均能够正常返回：

```txt
OK
```

5块开发板均成功执行：

```txt
AT+SLEENABLE
```

成功输出示例：

```txt
[ACore] sle enable cbk in, result:0
sle enable
OK
```

阶段结论：

```txt
5块开发板基础 AT 通道正常。
5块开发板当前出厂固件均支持并能够使能 SLE 协议栈。
```

---

## 二、T1广播端配置

### 1. 基础AT测试

执行节点：

```txt
T1 / COM10
```

发送：

```txt
AT
```

返回：

```txt
OK
```

### 2. 使能SLE

发送：

```txt
AT+SLEENABLE
```

返回：

```txt
[ACore] sle enable cbk in, result:0
sle enable
OK
xo update temp:4,diff:0,xo:0x3083c
```

结果：

```txt
T1的SLE协议栈成功使能。
```

### 3. 配置广播参数

发送：

```txt
AT+SLESETADVPAR=1,3,200,200,0,0x112233445588,0,0x112233445588
```

返回：

```txt
[ACore] sle set announce param, handle:1, mode:3, min_interval:c8, max_interval:c8, tx_power: 127
[ACore] sle set announce param, own addr:0x11:**:**:**:55:88
[ACore] sle set announce param, peer addr:0x11:**:**:**:55:88
OK
```

参数记录：

```txt
广播句柄：1
广播模式：3（沿用厂家示例，当前手册未解释准确枚举含义）
最小广播间隔：200 × 125 μs = 25 ms
最大广播间隔：200 × 125 μs = 25 ms
T1测试地址：11:22:33:44:55:88
```

结果：

```txt
T1广播参数配置成功。
```

### 4. 配置广播数据

发送：

```txt
AT+SLESETADVDATA=1,10,4,aabbccddeeff11223344,11224455
```

返回：

```txt
OK
```

数据记录：

```txt
广播句柄：1
广播数据长度：10字节
广播数据：aabbccddeeff11223344
扫描响应数据长度：4字节
扫描响应数据：11224455
```

结果：

```txt
T1广播数据配置成功。
```

### 5. 启动SLE广播

发送：

```txt
AT+SLESTARTADV=1
```

返回：

```txt
[ACore] sle start announce in, adv_id:1
OK
[ACore] sle adv cbk in, event:0 status:0
[ACore] sle adv cbk in, event:1 status:0
[ACore] sle adv cbk in, event:2 status:0
[ACore] sle adv cbk in, event:3 status:0
```

结果：

```txt
T1成功启动广播句柄1。
广播相关回调状态均为status:0。
T1进入持续SLE广播状态。
```

---

## 三、A1锚点连接配置

### 1. 基础AT测试

执行节点：

```txt
A1 / COM8
```

发送：

```txt
AT
```

返回：

```txt
OK
```

### 2. 使能SLE

发送：

```txt
AT+SLEENABLE
```

返回：

```txt
[ACore] sle enable cbk in, result:0
sle enable
OK
```

结果：

```txt
A1的SLE协议栈成功使能。
```

### 3. A1连接T1

发送：

```txt
AT+SLECONN=0,112233445588
```

返回：

```txt
OK
[Connected]
addr:11:**:**:**:55:88, conn_id:00
```

结果：

```txt
A1成功连接T1。
T1地址：11:22:33:44:55:88
当前有效连接ID：conn_id=0
```

后续重复发送连接命令时，只返回：

```txt
OK
```

没有产生新的 `[Connected]` 回调，因此没有证据表明建立了新的连接。

---

## 四、A1读取T1 RSSI测试

### 1. 使用有效连接ID

在 A1 / COM8 发送：

```txt
AT+SLEREADPEERRSSI=0
```

多次测试均返回：

```txt
OK
```

完整重复结果：

```txt
AT+SLEREADPEERRSSI=0
OK
AT+SLEREADPEERRSSI=0
OK
AT+SLEREADPEERRSSI=0
OK
AT+SLEREADPEERRSSI=0
OK
AT+SLEREADPEERRSSI=0
OK
AT+SLEREADPEERRSSI=0
OK
AT+SLEREADPEERRSSI=0
OK
```

测试过程中改变 T1 与 A1 的距离，串口仍未输出：

```txt
RSSI具体数值
RSSI回调日志
可供上位机解析的数据行
```

### 2. 使用不存在的连接ID

发送：

```txt
AT+SLEREADPEERRSSI=1
```

返回：

```txt
ERROR
```

对照结果：

```txt
conn_id=0 → OK
conn_id=1 → ERROR
```

说明：

```txt
固件能够识别当前有效连接ID。
AT+SLEREADPEERRSSI=0不是无条件返回OK的空命令。
针对有效连接的RSSI读取请求能够被命令层接受。
```

但当前仍然无法确认：

```txt
底层是否已经产生RSSI数值；
是否产生了回调但没有输出到UART；
是否为出厂AT固件实现或版本限制。
```

---

# 五、AT阶段最终结论

## 已验证

```txt
1. 5块WS63E开发板基础AT通信正常。
2. 5块WS63E均能够成功使能SLE协议栈。
3. T1能够配置广播参数。
4. T1能够配置广播数据。
5. T1能够成功启动SLE广播。
6. A1能够使用T1测试地址建立SLE连接。
7. A1获得有效连接ID conn_id=0。
8. A1对conn_id=0提交RSSI读取请求时返回OK。
9. A1对不存在的conn_id=1读取RSSI时返回ERROR。
```

## 未完成

```txt
1. 未从串口获得T1的具体RSSI数值。
2. 未形成可供电脑上位机解析的RSSI输出格式。
3. 未验证RSSI数值会随T1和A1距离变化而变化。
4. 未验证连续RSSI采样。
5. 未验证A2、A3、A4并行测量T1。
```

## 阶段判断

```txt
出厂AT固件已完成SLE广播、连接和RSSI命令入口的可行性验证。

由于AT+SLEREADPEERRSSI只能返回OK，无法输出具体RSSI数值，
继续在AT层重复测试的收益较低。

AT验证阶段结束，转入WS63E SDK功能开发阶段。
```

# 下一阶段交接目标

SDK阶段当前只定义一个最小功能：

> **实现 A1 与 T1 建立 SLE 单点连接，并由 A1 获取、打印 T1 的具体 RSSI 数值。**

暂时不要扩展到：

```txt
A2、A3、A4
四锚点汇总
RSSI指纹数据集
区域分类模型
实时定位上位机
```

SDK最小验收标准建议记录为：

```txt
1. T1能够启动SLE广播。
2. A1能够发现或连接T1。
3. A1的RSSI回调能够获得有符号RSSI数值。
4. A1通过UART输出RSSI数值。
5. T1靠近和远离A1时，RSSI整体变化方向合理。
6. 断开后能够重新连接并继续读取RSSI。
```

建议 SDK 第一版串口输出至少做到：

```txt
RSSI,A1,T1,-48
```

当前节点定义、测试结果和阶段结论已经完整，可以作为后续“功能定义 → MVP定义 → 原理学习 → SDK实现”的输入基线。