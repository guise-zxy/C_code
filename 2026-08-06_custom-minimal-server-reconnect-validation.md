# WS63E 自研最小 SLE Server 集成与断线恢复验证日志

## 1. 基本信息

- 日期：2026-08-06
- 项目：星闪多锚点 RSSI 室内定位
- 当前阶段：自研最小单锚点 Server 构建集成、广播连接及断线恢复验证
- SDK 根目录：`D:\NearLink\workspace\fbb_ws63`
- 工程目录：`D:\NearLink\workspace\fbb_ws63\src`
- 构建目标：`ws63-liteos-app`
- 开发板：WS63E × 2
- 开发工具：HiSpark Studio、PowerShell

### 节点角色

| 节点 | 本次固件角色 | 代码来源 | 串口 |
|---|---|---|---|
| T1 | SLE Server | 项目自研最小 Server | COM10 |
| A1 | SLE RSSI Ranging Client | 本地 SDK 官方示例 | COM8 |

### 代码位置

- 自研 Server 组件：`application\ws63\nearlink_rssi_positioning`
- 自研 Server 源文件：`application\ws63\nearlink_rssi_positioning\src\nearlink_rssi_server.c`
- 官方 Client 示例：`application\samples\bt\sle\sle_rssi_ranging`

### 记录边界

本文件保存本轮构建和实板验证中已经观察到的关键证据，以及基于本地源码能够确认的结论。串口部分来源于本轮用户提供的日志片段，不冒充带时间戳的串口原始全量导出文件。

---

## 2. 本阶段目标

本阶段只验证以下最小功能切片：

1. 自研 `nearlink_rssi_server.c` 被 SDK 构建系统正式纳入；
2. 不依赖 `Enable Sample` 也能选择和构建自研 Server；
3. 自研 Server 能使能 SLE、注册广播与连接回调并启动广播；
4. 官方 SDK Client 能发现自研 Server 并建立连接；
5. 官方 Client 能周期读取连接态 RSSI；
6. Client 断电后，Server 能检测断连并恢复广播；
7. Client 重新上电后能再次扫描、连接并恢复 RSSI；
8. 上述 Client 断电恢复流程连续验证 3 次。

本阶段暂不包含：

- 一米 RSSI 标定；
- GPIO13 与 LED 校准流程验证；
- NV 标定数据验证；
- 距离模型精度评估；
- 多锚点并发连接；
- 上位机多路 RSSI 汇总；
- 四区域分类。

---

## 3. 构建系统问题与修正

### 3.1 原问题

最初在 Menuconfig 中，自研 `NearLink RSSI Positioning` 入口实际受到：

```text
Enable Sample
```

控制。关闭 `Enable Sample` 后，即使选择自研 RSSI Server，源文件也没有参与构建。

### 3.2 当前构建注册方式

自研业务 Kconfig 入口放在 `SAMPLE_ENABLE` 条件块之外：

```kconfig
if SAMPLE_ENABLE
osource "application/samples/Kconfig"
endif

osource "application/ws63/nearlink_rssi_positioning/Kconfig"
```

WS63 应用层 CMake 注册业务组件：

```cmake
add_subdirectory_if_exist(nearlink_rssi_positioning)
```

WS63 target 组件列表包含：

```python
'nearlink_rssi_positioning',
```

业务组件根据独立配置项选择 Server 源文件：

```cmake
if(DEFINED CONFIG_NEARLINK_RSSI_POSITIONING_MIN_SERVER)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nearlink_rssi_server.c
    )
endif()
```

当前 Menuconfig 保存状态中的关键配置为：

```text
# CONFIG_SAMPLE_ENABLE is not set
CONFIG_SUPPORT_SLE_PERIPHERAL=y
CONFIG_NEARLINK_RSSI_POSITIONING_MIN_SERVER=y
```

由此可以确认：自研业务组件已经与 SDK 官方 Sample 开关解耦。

### 3.3 涉及文件

- `application/Kconfig`
- `application/ws63/CMakeLists.txt`
- `build/config/target_config/ws63/config.py`
- `application/ws63/nearlink_rssi_positioning/Kconfig`
- `application/ws63/nearlink_rssi_positioning/CMakeLists.txt`
- `application/ws63/nearlink_rssi_positioning/src/nearlink_rssi_server.c`
- `build/config/target_config/ws63/menuconfig/acore/ws63_liteos_app.config`（Menuconfig 保存结果）

说明：`ws63_liteos_app.config` 是配置工具保存的目标配置状态，不应当把其中所有机械变化都解释为本业务手工设计。

---

## 4. 常用配置与构建命令

### 打开 Menuconfig

```powershell
python build.py ws63-liteos-app menuconfig
```

### 清理后重新构建

```powershell
python build.py -c ws63-liteos-app
```

如果从普通 PowerShell 启动构建，需要保证 HiSpark 工具链中的 CMake 和 Ninja 已加入当前终端的 `Path`。本轮曾出现：

```text
FileNotFoundError: [WinError 2] 系统找不到指定的文件
```

在当前终端补充 HiSpark 工具路径后，`Get-Command cmake` 和 `Get-Command ninja` 均能正确定位工具。

本轮还观察到 Python 脚本的：

```text
SyntaxWarning: invalid escape sequence
```

该警告来自 SDK 构建脚本，不是本次 Server C 源码的编译错误。

---

## 5. 源文件进入编译的直接验证

为了判断 `nearlink_rssi_server.c` 是否真正参与编译，曾临时删除第 131 行语句末尾的分号，然后执行重新编译。

编译器明确报错：

```text
application/ws63/nearlink_rssi_positioning/CMakeFiles/
nearlink_rssi_positioning.dir/src/nearlink_rssi_server.c.obj

nearlink_rssi_server.c: In function 'nearlink_server_announce_enable_cb':
nearlink_rssi_server.c:132:1: error: expected ';' before '}' token
}
^
ninja: build stopped: subcommand failed.
######### Build target:ws63_liteos_app failed
```

结论：

1. 构建系统生成了 `nearlink_rssi_server.c.obj` 的编译规则；
2. 编译器实际读取并解析了该源文件；
3. 自研 Server 已经进入 `ws63-liteos-app` 构建目标；
4. 本次失败是人为制造的语法错误，不是新的集成故障。

恢复分号后重新构建并烧录成功。后续串口中出现自研 Server 日志，进一步证明新固件实际运行了该源文件。

---

## 6. 自研 Server 当前最小流程

当前 Server 的关键调用链为：

```text
app_run(nearlink_server_entry)
→ 创建 NearlinkRssiSrv 任务
→ enable_sle()
→ 注册广播/扫描类回调表
→ 注册连接状态回调表
→ sle_set_announce_param()
→ sle_set_announce_data()
→ sle_start_announce()
→ announce_enable_cb() 报告广播启动结果
→ connect_state_changed_cb() 报告连接或断连
→ 断连后再次调用 sle_start_announce()
```

主要广播参数：

```text
名称：sle_rssi_server
广播句柄：1
广播模式：connectable + scannable
广播间隔：0xC8 / 0xC8
连接间隔：50 / 50
连接监督超时配置值：500
广播功率：18 dBm
广播地址：41:42:43:44:45:46
```

本地官方示例对监督超时字段的注释为单位 10 ms，因此配置值 500 对应约 5 秒；实际断电检测耗时仍应以带时间戳的实测为准。

---

## 7. 首次启动、广播与连接验证

### 7.1 Server 关键日志

```text
[nearlink rssi server] task started
[nearlink rssi server] initialization begin
[ACore] sle enable cbk in, result:0
sle enable
[nearlink rssi server] SLE enabled successfully
[nearlink rssi server] required callbacks registered
[ACore] sle set announce param, handle:1, mode:3, min_interval:c8, max_interval:c8, tx_power: 18
[ACore] sle set announce param, own addr:0x41:**:**:**:45:46
[ACore] sle start announce in, adv_id:1
[ACore] sle adv cbk in, event:0 status:0
[ACore] sle adv cbk in, event:1 status:0
[ACore] sle adv cbk in, event:2 status:0
[ACore] sle adv cbk in, event:3 status:0
[nearlink rssi server] server ready: announcement running, id=1
[nearlink rssi server] announce start request accepted: name=sle_rssi_server, handle=1, anchor_addr=41:42:43:44:45:46
[nearlink rssi server] initialization requests submitted; waiting for announce callback
[Connected]
addr:ee:**:**:**:8d:5c, conn_id:00
[ACore] sle adv cbk in, event:7 status:0
[nearlink rssi server] connected: conn_id=0x00, pair_state=0x1, peer=ee:**:**:**:8d:5c
```

### 7.2 Client 关键日志

```text
[sle rssi client] task start
[sle rssi cal] no valid NV calibration, use default A=-45 dBm
[ACore] sle enable cbk in, result:0
[sle rssi client] SLE enabled, status=0x0
[sle rssi client] start seek
[sle rssi client] seek enabled, status=0x0
[adv_report] event_type: 0x03, addr_type: 0x0000, addr:41:**:**:**:45:46
[adv_report] event_type: 0x0b, addr_type: 0x0000, addr:41:**:**:**:45:46
[sle rssi client] found sle_rssi_server, scan_rssi=-60 dBm, stop seek
[sle rssi client] seek disabled, status=0x0
[sle rssi client] connect request sent, status=0x0
[Connected]
addr:41:**:**:**:45:46, conn_id:00
[sle rssi client] connected, conn_id=0x00, calibration=-45 dBm@1m, path_loss=2.0
[sle rssi client] range: raw=-57 dBm, median=-57 dBm, filtered=-57.0 dBm, samples=1, distance=398 cm, zone=middle
[sle rssi client] range: raw=-54 dBm, median=-55 dBm, filtered=-56.5 dBm, samples=6, distance=376 cm, zone=middle
[sle rssi client] range: raw=-52 dBm, median=-52 dBm, filtered=-52.8 dBm, samples=7, distance=246 cm, zone=middle
```

### 7.3 结果

- 自研 Server 成功使能 SLE；
- 广播与连接回调注册成功；
- 广播参数和广播数据设置成功；
- 广播启动完成回调返回成功；
- 官方 Client 成功识别名称 `sle_rssi_server`；
- 官方 Client 连接的地址与 Server 广播地址一致；
- 双方进入连接状态；
- 官方 Client 成功周期读取连接态 RSSI。

`conn_id` 是每台设备协议栈内部的本地连接句柄。双方本次都显示 `0x00`，不能仅凭该数字证明是同一条连接；连接对应关系主要由设备地址、事件时间关系和连接后的 RSSI 数据链路共同确认。

`pair_state=0x1` 在本地头文件中对应 `SLE_PAIR_NONE`，表示已经连接但未配对。

---

## 8. 广播回调与 API 返回的时序观察

首次启动时观察到：

```text
server ready: announcement running
announce start request accepted
initialization requests submitted; waiting for announce callback
```

断线后恢复广播时观察到：

```text
restart announce requested
server ready: announcement running
```

由两种不同顺序能够确认：

> 业务代码不能假设广播完成回调必然发生在 `sle_start_announce()` 返回后的业务代码之后。

日志只能证明广播回调有机会在调用者继续执行后续语句前完成，不能据此确认它一定由 `sle_start_announce()` 在同一调用栈内同步调用，也可能涉及协议栈任务调度。

当前功能逻辑正确，但下面的日志语义存在误导：

```text
initialization requests submitted; waiting for announce callback
```

因为打印该句时，广播回调可能已经执行完毕。建议后续改为中性表达：

```text
initialization API calls completed successfully
```

或者删除该额外成功日志。广播是否真正启动，应继续以 `announce_enable_cb()` 的 `status` 为准。

---

## 9. Client 断电恢复测试

### 9.1 测试方法

每轮按照以下步骤操作：

```text
Server 与 Client 已连接且 Client 持续输出 RSSI
→ Client 断电
→ 等待 Server 协议栈报告断连
→ Server 重新启动广播
→ Client 重新上电
→ Client 扫描并发现 Server
→ Client 再次连接
→ Client 恢复周期 RSSI 输出
```

共执行 3 轮。

### 9.2 第 1 轮

Server 关键日志：

```text
[Disconnected]
addr:ee:**:**:**:8d:5c, conn_id:00
[nearlink rssi server] disconnected: conn_id=0x00, reason=0x7
[ACore] sle start announce in, adv_id:1
[ACore] sle adv cbk in, event:0 status:0
[ACore] sle adv cbk in, event:1 status:0
[ACore] sle adv cbk in, event:2 status:0
[ACore] sle adv cbk in, event:3 status:0
[nearlink rssi server] restart announce requested
[nearlink rssi server] server ready: announcement running, id=1
[Connected]
addr:ee:**:**:**:8d:5c, conn_id:00
[ACore] sle adv cbk in, event:7 status:0
[nearlink rssi server] connected: conn_id=0x00, pair_state=0x1, peer=ee:**:**:**:8d:5c
```

结果：Server 检测断连、恢复广播并接受 Client 重连，PASS。

### 9.3 第 2 轮

Server 关键日志：

```text
[Disconnected]
addr:ee:**:**:**:8d:5c, conn_id:00
[nearlink rssi server] disconnected: conn_id=0x00, reason=0x7
[ACore] sle start announce in, adv_id:1
[ACore] sle adv cbk in, event:0 status:0
[ACore] sle adv cbk in, event:1 status:0
[ACore] sle adv cbk in, event:2 status:0
[ACore] sle adv cbk in, event:3 status:0
[nearlink rssi server] restart announce requested
[nearlink rssi server] server ready: announcement running, id=1
[Connected]
addr:ee:**:**:**:8d:5c, conn_id:00
[ACore] sle adv cbk in, event:7 status:0
[nearlink rssi server] connected: conn_id=0x00, pair_state=0x1, peer=ee:**:**:**:8d:5c
```

结果：Server 检测断连、恢复广播并接受 Client 重连，PASS。

### 9.4 第 3 轮

Server 关键日志：

```text
[Disconnected]
addr:ee:**:**:**:8d:5c, conn_id:00
[nearlink rssi server] disconnected: conn_id=0x00, reason=0x7
[ACore] sle start announce in, adv_id:1
[ACore] sle adv cbk in, event:0 status:0
[ACore] sle adv cbk in, event:1 status:0
[ACore] sle adv cbk in, event:2 status:0
[ACore] sle adv cbk in, event:3 status:0
[nearlink rssi server] restart announce requested
[nearlink rssi server] server ready: announcement running, id=1
[Connected]
addr:ee:**:**:**:8d:5c, conn_id:00
[ACore] sle adv cbk in, event:7 status:0
[nearlink rssi server] connected: conn_id=0x00, pair_state=0x1, peer=ee:**:**:**:8d:5c
```

Client 重新上电后的关键日志：

```text
[sle rssi cal] no valid NV calibration, use default A=-45 dBm
[ACore] sle enable cbk in, result:0
[sle rssi client] SLE enabled, status=0x0
[sle rssi client] start seek
[sle rssi client] seek enabled, status=0x0
[adv_report] event_type:0x03, addr_type:0x0000, addr:41:**:**:**:45:46
[adv_report] event_type:0x0b, addr_type:0x0000, addr:41:**:**:**:45:46
[sle rssi client] found sle_rssi_server, scan_rssi=-47 dBm, stop seek
[sle rssi client] seek disabled, status=0x0
[sle rssi client] connect request sent, status=0x0
[Connected]
addr:41:**:**:**:45:46, conn_id:00
[sle rssi client] connected, conn_id=0x00, calibration=-45 dBm@1m, path_loss=2.0
[sle rssi client] range: raw=-50 dBm, median=-50 dBm, filtered=-50.0 dBm, samples=1, distance=178 cm, zone=middle
[sle rssi client] range: raw=-46 dBm, median=-46 dBm, filtered=-48.2 dBm, samples=5, distance=145 cm, zone=near
[sle rssi client] range: raw=-49 dBm, median=-46 dBm, filtered=-46.9 dBm, samples=7, distance=125 cm, zone=near
```

结果：Server 检测断连并恢复广播；Client 重新扫描、连接并恢复 RSSI，PASS。

### 9.5 汇总

| 验证项 | 第 1 轮 | 第 2 轮 | 第 3 轮 | 总结果 |
|---|---:|---:|---:|---:|
| Server 检测断连 | PASS | PASS | PASS | 3/3 PASS |
| Server 恢复广播 | PASS | PASS | PASS | 3/3 PASS |
| Client 重新连接 | PASS | PASS | PASS | 3/3 PASS |
| Client 恢复 RSSI | 已由本轮整体实验确认 | 已由本轮整体实验确认 | 有明确 Client 片段 | 3/3 PASS |

说明：第 1、2 轮在本记录中保留的是 Server 端完整关键链路；第 3 轮同时保留了 Client 重新上电和 RSSI 恢复片段。3/3 的整体结果依据本轮实际操作报告及对应日志，不将缺失的第 1、2 轮 Client 原始逐行输出补写为虚构日志。

---

## 10. 断线原因的事实边界

三次 Client 断电后，Server 均报告：

```text
reason=0x7
```

本地公开头文件明确列出的主动断开状态只有：

```c
SLE_DISCONNECT_BY_REMOTE = 0x10
SLE_DISCONNECT_BY_LOCAL  = 0x11
```

本地公开定义中没有找到 `0x7` 的明确映射。因此：

- 已确认：`0x7` 是三次 Client 断电后 Server 实际收到的断连原因值；
- 根据实验推断：它很可能与底层链路丢失或连接监督超时有关；
- 尚未确认：`0x7` 在当前 SLE 协议栈中的正式名称与精确定义。

不得直接套用 BLE 通用错误码把它写成已确认事实。

---

## 11. 内存与日志状态观察

第三轮附近的 Server 状态为：

```text
断开前：used=94100 / 94156
断开后：used=93212 / 93164
重连后：used=94180
```

前两轮重连后的已观察值还包括：

```text
used=94160
used=94172
```

判断：

- 断开后连接相关资源占用下降；
- 重连后内存回到约 94 KB；
- 目前没有观察到每轮重连后持续单调增长；
- 三轮样本不足以证明长期无内存泄漏，只能说明本轮没有明显泄漏迹象。

状态日志均显示：

```text
log: drop/all[0/0]
```

在本次观察窗口内未发现日志丢弃。

---

## 12. RSSI 与距离结果边界

官方 Client 当前打印：

```text
no valid NV calibration, use default A=-45 dBm
```

因此：

- `raw` 是本阶段需要确认的真实连接态 RSSI 观测值；
- `median` 是最多 7 点滑动中值；
- `filtered` 是中值结果进一步经过 EMA 平滑后的值；
- `samples` 在新连接建立后从 1 重新开始，说明滤波状态被复位；
- `distance` 和 `zone` 使用默认 `A=-45 dBm`、`n=2.0` 计算；
- 本阶段没有进行一米标定，距离数值不作为 Server 验收指标。

扫描阶段的 `scan_rssi` 与连接后的周期 `raw` 来自不同阶段和采样时刻，数值不完全相同不构成功能异常。

---

## 13. 本阶段验收结果

### 已完成

- [x] 自研业务 Kconfig 与 `Enable Sample` 解耦；
- [x] 自研组件加入 WS63 CMake 目录；
- [x] 自研组件加入 `ws63-liteos-app` target 组件列表；
- [x] 通过人为语法错误证明 `nearlink_rssi_server.c` 实际参与编译；
- [x] 恢复语法后完成构建和烧录；
- [x] 自研 Server 任务实际启动；
- [x] SLE 使能成功；
- [x] 广播回调注册成功；
- [x] 连接状态回调注册成功；
- [x] 广播参数和数据设置成功；
- [x] 广播启动完成回调成功；
- [x] 官方 Client 成功发现自研 Server；
- [x] 官方 Client 与自研 Server 建立连接；
- [x] 官方 Client 周期读取连接态 RSSI；
- [x] Client 断电后 Server 检测断连；
- [x] Server 断线后恢复广播；
- [x] Client 重新上电后再次连接；
- [x] Client 重连后恢复 RSSI；
- [x] Client 断电恢复重复测试 3/3 通过。

### 已知问题或未确认事项

- [ ] `initialization requests submitted; waiting for announce callback` 日志语义不准确；
- [ ] `reason=0x7` 的当前 SDK 正式定义尚未从公开头文件确认；
- [ ] 尚未执行 Server 断电、官方 Client 自动恢复测试；
- [ ] 尚未进行更长时间和更多轮次的稳定性测试；
- [ ] 尚未验证多连接和多锚点；
- [ ] 尚未进入独立 Client 实现；
- [ ] 本阶段未验证 RSSI 距离模型精度。

---

## 14. 本阶段结论

本阶段已经证明：

> 项目自研最小 SLE Server 已正式进入 WS63 SDK 构建目标，能够在 WS63E 实板上完成 SLE 使能、可连接广播、连接状态通知和断线后恢复广播；本地 SDK 官方 RSSI Client 能发现并连接该 Server，随后周期读取连接态 RSSI。Client 断电恢复流程连续验证 3 次，3 次均恢复广播、重新连接并恢复 RSSI。

当前结果是最小单锚点链路和 Server 断线恢复的实板基线，不代表多锚点架构、距离精度或长期稳定性已经验证完成。

---

## 15. 下一步最小工作

1. 保存当前可运行 Server 代码基线和本验证记录；
2. 可选地修正误导性的初始化成功日志，并重新构建、启动验证一次；
3. 用户不看当前实现，先写出最小 Server 状态流程或伪代码；
4. 根据本地 SDK 接口补齐独立 Server 骨架；
5. 对照当前已验证版本检查回调注册、返回值和断线恢复路径；
6. 暂不进入 GPIO13、NV 校准和距离算法扩展。

