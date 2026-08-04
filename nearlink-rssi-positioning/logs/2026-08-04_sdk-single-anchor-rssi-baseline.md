
# WS63 SLE 单锚点 RSSI 链路验证日志

## 1. 基本信息

- 日期：2026-08-04
- 项目：星闪多锚点 RSSI 室内定位
- 当前阶段：SDK 官方示例单锚点链路验证
- 开发工具：HiSpark Studio
- SDK 路径：`D:\NearLink\workspace\fbb_ws63`
- 工程路径：`D:\NearLink\workspace\fbb_ws63\src`
- 使用示例：`application\samples\bt\sle\sle_rssi_ranging`

### 节点角色

| 节点 | 角色 | 串口 |
|---|---|---|
| T1 | SLE RSSI Ranging Server | COM10 |
| A1 | SLE RSSI Ranging Client | COM8 |

### 串口日志参数

- 波特率：115200
- 数据位：8
- 停止位：1
- 校验位：无
- 流控：无

---

## 2. 本阶段目标

基于 WS63 SDK 官方 `sle_rssi_ranging` 示例，完成以下最小链路闭环：

1. T1 运行 SLE Server；
2. A1 运行 SLE Client；
3. A1 扫描并识别 T1；
4. A1 与 T1 建立 SLE 连接；
5. A1 周期读取 T1 的连接态 RSSI；
6. 通过靠近、远离实验验证 RSSI 的总体变化趋势。

本阶段暂不包含：

- A2、A3、A4；
- 多锚点连接调度；
- 四路 RSSI 汇总；
- 上位机采集；
- 位置标签；
- 四区域分类模型；
- 精确距离测量。

---

## 3. 工程建立

在 HiSpark Studio 中创建 WS63 示例工程。

配置如下：

- 芯片：WS63
- 开发板：ws63
- 工程类型：示例工程
- 工程名称：`ws63_sle_rssi`
- 工程路径：`D:\NearLink\workspace\fbb_ws63\src`
- 软件包路径：`D:\NearLink\workspace\fbb_ws63\src`

工程创建成功后，在以下目录找到官方示例：

```text
application
└─ samples
   └─ bt
      └─ sle
         └─ sle_rssi_ranging
```


## 4. T1 Server 配置、编译与烧录

### 4.1 系统配置

在 HiSpark Studio 系统配置中选择：

```text
Support SLE RSSI Ranging Server Sample
```

保存配置后执行重编译。

### 4.2 烧录

烧录目标：

```text
T1 / COM10
```

烧录结果：

```text
烧录进度: 100/100
All images burnt successfully.

reset device 1
Reset device...
update_reset_count 0x1
```

### 4.3 结果

T1 的 SLE RSSI Ranging Server 固件烧录成功。

---

## 5. A1 Client 配置、编译与烧录

### 5.1 系统配置

在系统配置中切换为：

```text
Support SLE RSSI Ranging Client Sample
```

Client 使用以下默认参数：

```text
Calibrated RSSI at 1 metre = -45 dBm
Path loss exponent multiplied by 10 = 20
```

对应：

```text
1 米参考 RSSI A = -45 dBm
路径损耗指数 n = 2.0
```

保存配置后执行重编译。

### 5.2 烧录

烧录目标：

```text
A1 / COM8
```

实际烧录命令：

```text
d:\HiSpark\toolchain\tools\cfbb\BurnTool\BurnTool.exe
-com:8
-bin:d:\NearLink\workspace\fbb_ws63\src\output\ws63\fwpkg\ws63-liteos-app\ws63-liteos-app_all.fwpkg
-signalbaud:921600
-reset
-beforereset
-switchafterloader
```

烧录结果：

```text
Connecting...Reset the device...
All images burnt successfully.

reset device 1
Reset device...
update_reset_count 0x1
```

### 5.3 结果

A1 的 SLE RSSI Ranging Client 固件烧录成功。

---

## 6. A1 启动与连接日志

A1 复位后输出：

```text
xo update temp:3,diff:0,xo:0x3083c
[sle rssi cal] no valid NV calibration, use default A=-45 dBm
[sle rssi cal] ready: hold GPIO13 for 2000 ms at 100 cm; LED GPIO5 blue=recording, green=saved
[ACore] sle enable cbk in, result:0
[sle rssi client] SLE enabled, status=0x0
[sle rssi client] start seek
[sle rssi client] seek enabled, status=0x0
sle enable
[adv_report] event_type: 0x03, addr_type: 0x0000, addr: 44:**:**:**:01:00
[adv_report] event_type: 0x0b, addr_type: 0x0000, addr: 44:**:**:**:01:00
[sle rssi client] found sle_rssi_server, scan_rssi=-66 dBm, stop seek
[sle rssi client] seek disabled, status=0x0
[sle rssi client] connect request sent, status=0x0
[Connected]
addr:41:**:**:**:45:46, conn_id:00
[sle rssi client] connected, conn_id=0x00, calibration=-45 dBm@1m, path_loss=2.0
[sle rssi cal] stale LED state cleared after SLE connection
```

### 连接结果

1. A1 成功启用 SLE；
2. A1 成功开始扫描；
3. A1 成功发现 `sle_rssi_server`；
4. 扫描阶段 RSSI 为 `-66 dBm`；
5. A1 成功向 T1 发起连接；
6. A1 与 T1 成功建立连接；
7. 当前连接 ID 为 `conn_id=0x00`；
8. Client 使用默认标定参数 `A=-45 dBm`、`n=2.0`。

---

## 7. RSSI 实验过程

实验操作：

1. 复位 A1；
2. 保持 T1 与 A1 连接；
3. 将 T1 从较远位置逐渐靠近 A1；
4. T1 靠近 A1 后短暂停留；
5. 再将 T1逐渐远离 A1；
6. 记录 A1 串口日志。

---

## 8. 靠近过程 RSSI 结果

靠近初期日志：

```text
[sle rssi client] range: raw=-63 dBm, median=-63 dBm, filtered=-63.0 dBm, samples=1, distance=794 cm, zone=far
[sle rssi client] range: raw=-63 dBm, median=-63 dBm, filtered=-63.0 dBm, samples=2, distance=794 cm, zone=far
[sle rssi client] range: raw=-61 dBm, median=-63 dBm, filtered=-63.0 dBm, samples=3, distance=794 cm, zone=far
[sle rssi client] range: raw=-59 dBm, median=-61 dBm, filtered=-62.5 dBm, samples=4, distance=750 cm, zone=far
[sle rssi client] range: raw=-56 dBm, median=-61 dBm, filtered=-62.1 dBm, samples=5, distance=718 cm, zone=far
[sle rssi client] range: raw=-55 dBm, median=-59 dBm, filtered=-61.3 dBm, samples=6, distance=656 cm, zone=far
[sle rssi client] range: raw=-54 dBm, median=-59 dBm, filtered=-60.7 dBm, samples=7, distance=614 cm, zone=far
```

继续靠近：

```text
[sle rssi client] range: raw=-37 dBm, median=-56 dBm, filtered=-59.5 dBm, samples=7, distance=535 cm, zone=far
[sle rssi client] range: raw=-41 dBm, median=-55 dBm, filtered=-58.4 dBm, samples=7, distance=469 cm, zone=middle
[sle rssi client] range: raw=-43 dBm, median=-54 dBm, filtered=-57.3 dBm, samples=7, distance=413 cm, zone=middle
[sle rssi client] range: raw=-45 dBm, median=-45 dBm, filtered=-54.2 dBm, samples=7, distance=290 cm, zone=middle
[sle rssi client] range: raw=-37 dBm, median=-43 dBm, filtered=-51.4 dBm, samples=7, distance=210 cm, zone=middle
[sle rssi client] range: raw=-34 dBm, median=-41 dBm, filtered=-48.8 dBm, samples=7, distance=155 cm, zone=middle
[sle rssi client] range: raw=-35 dBm, median=-37 dBm, filtered=-45.8 dBm, samples=7, distance=110 cm, zone=near
[sle rssi client] range: raw=-36 dBm, median=-37 dBm, filtered=-43.6 dBm, samples=7, distance=86 cm, zone=near
```

近距离停留：

```text
[sle rssi client] range: raw=-36 dBm, median=-36 dBm, filtered=-41.7 dBm, samples=7, distance=69 cm, zone=near
[sle rssi client] range: raw=-37 dBm, median=-36 dBm, filtered=-40.2 dBm, samples=7, distance=58 cm, zone=near
[sle rssi client] range: raw=-34 dBm, median=-36 dBm, filtered=-39.2 dBm, samples=7, distance=51 cm, zone=near
[sle rssi client] range: raw=-31 dBm, median=-35 dBm, filtered=-38.1 dBm, samples=7, distance=46 cm, zone=near
[sle rssi client] range: raw=-30 dBm, median=-35 dBm, filtered=-37.3 dBm, samples=7, distance=42 cm, zone=near
[sle rssi client] range: raw=-31 dBm, median=-34 dBm, filtered=-36.5 dBm, samples=7, distance=38 cm, zone=near
[sle rssi client] range: raw=-30 dBm, median=-31 dBm, filtered=-35.1 dBm, samples=7, distance=32 cm, zone=near
[sle rssi client] range: raw=-30 dBm, median=-31 dBm, filtered=-34.1 dBm, samples=7, distance=29 cm, zone=near
[sle rssi client] range: raw=-32 dBm, median=-31 dBm, filtered=-33.3 dBm, samples=7, distance=26 cm, zone=near
[sle rssi client] range: raw=-30 dBm, median=-30 dBm, filtered=-32.4 dBm, samples=7, distance=24 cm, zone=near
```

### 靠近过程结论

原始 RSSI 总体从：

```text
约 -63 dBm
```

增强到：

```text
约 -30 dBm ～ -36 dBm
```

说明 T1 靠近 A1 时，A1 测得的 RSSI 总体增强，符合预期。

---

## 9. 远离过程 RSSI 结果

开始远离：

```text
[sle rssi client] range: raw=-52 dBm, median=-30 dBm, filtered=-31.8 dBm, samples=7, distance=22 cm, zone=near
[sle rssi client] range: raw=-63 dBm, median=-31 dBm, filtered=-31.6 dBm, samples=7, distance=22 cm, zone=near
[sle rssi client] range: raw=-56 dBm, median=-32 dBm, filtered=-31.7 dBm, samples=7, distance=22 cm, zone=near
[sle rssi client] range: raw=-74 dBm, median=-52 dBm, filtered=-36.8 dBm, samples=7, distance=39 cm, zone=near
[sle rssi client] range: raw=-70 dBm, median=-56 dBm, filtered=-41.5 dBm, samples=7, distance=68 cm, zone=near
[sle rssi client] range: raw=-69 dBm, median=-63 dBm, filtered=-46.9 dBm, samples=7, distance=125 cm, zone=near
[sle rssi client] range: raw=-70 dBm, median=-69 dBm, filtered=-52.4 dBm, samples=7, distance=236 cm, zone=middle
[sle rssi client] range: raw=-73 dBm, median=-70 dBm, filtered=-56.8 dBm, samples=7, distance=391 cm, zone=middle
[sle rssi client] range: raw=-69 dBm, median=-70 dBm, filtered=-60.1 dBm, samples=7, distance=571 cm, zone=far
[sle rssi client] range: raw=-68 dBm, median=-70 dBm, filtered=-62.5 dBm, samples=7, distance=758 cm, zone=far
[sle rssi client] range: raw=-69 dBm, median=-69 dBm, filtered=-64.1 dBm, samples=7, distance=912 cm, zone=far
[sle rssi client] range: raw=-71 dBm, median=-69 dBm, filtered=-65.3 dBm, samples=7, distance=1046 cm, zone=far
```

### 远离过程结论

T1 远离后，原始 RSSI 总体下降到：

```text
约 -68 dBm ～ -74 dBm
```

说明 T1 远离 A1 时，A1 测得的 RSSI 总体减弱，符合预期。

---

## 10. 现象分析

### 10.1 RSSI 不呈严格单调变化

实验中，RSSI 的总体趋势与距离变化一致：

```text
靠近 → RSSI 更接近 0
远离 → RSSI 更加负
```

但局部存在反向跳变。

例如靠近过程中出现：

```text
-37 → -41 → -43 → -45 → -37
```

可能原因包括：

- 室内多径传播；
- 墙面、桌面和地面的信号反射；
- 手和人体对天线的遮挡；
- T1 在移动过程中的天线方向变化；
- 无线环境干扰；
- 芯片 RSSI 测量本身的波动。

结论：

> 单次 RSSI 不适合作为稳定的位置或距离判断依据，应使用时间窗口和统计特征。

---

### 10.2 distance 与实际物理距离明显不一致

启动日志显示：

```text
[sle rssi cal] no valid NV calibration, use default A=-45 dBm
```

说明当前没有有效的实际标定数据，程序使用默认参数：

```text
A = -45 dBm
n = 2.0
```

例如：

```text
filtered=-63 dBm
distance=794 cm
```

该数值是根据默认对数距离模型计算得到的，但与实际目测距离明显不一致。

判断：

- 默认的 1 米参考 RSSI 不一定适合当前板卡；
- 默认路径损耗指数不一定适合当前室内环境；
- 天线方向、人体遮挡和多径传播都会影响距离估算。

结论：

> 当前 `distance` 只是官方示例根据默认参数生成的派生结果，暂时不能作为真实物理距离，也不能作为项目的位置真值。

---

### 10.3 median 和 filtered 存在响应滞后

日志中包含：

```text
raw
median
filtered
samples
distance
zone
```

其含义为：

- `raw`：最新一次读取到的 RSSI；
- `median`：最近最多 7 个 raw 样本的中位数；
- `filtered`：对 median 进一步进行平滑后的结果；
- `distance`：根据 filtered 和传播模型计算的距离；
- `zone`：根据估算距离划分的 near、middle 或 far。

根据日志变化可推断，`filtered` 使用了近似如下的 EMA：

```text
filtered_new
= 0.25 × median_current
+ 0.75 × filtered_previous
```

例如：

```text
上一次 filtered = -63.0
当前 median = -61
```

计算：

```text
0.25 × (-61) + 0.75 × (-63)
= -62.5
```

与日志中的：

```text
filtered=-62.5 dBm
```

一致。

由于存在 7 点中位数窗口和 EMA 平滑：

- T1 刚靠近时，历史远距离样本仍会影响输出；
- T1 刚远离时，历史近距离样本仍会影响输出；
- `filtered` 和 `distance` 不代表打印时刻的即时物理位置。

结论：

> 当前动态实验应优先观察 `raw` 的变化趋势；后续正式采样应保留原始 RSSI，在电脑端统一进行窗口统计和滤波。

---

## 11. 系统运行状态

串口中周期输出：

```text
APP|[SYS INFO] mem: used:102312, free:256076; log: drop/all[0/0], at_recv 0.
```

实验期间：

- 未观察到日志丢失；
- 未观察到异常断线；
- 未观察到系统重启；
- A1 能持续输出 RSSI；
- T1 与 A1 的连接保持正常。

---

## 12. 本阶段验收结果

### 已完成

- [x] HiSpark Studio 示例工程成功建立；
- [x] 成功定位官方 `sle_rssi_ranging` 示例；
- [x] T1 Server 成功配置；
- [x] T1 Server 成功编译；
- [x] T1 Server 成功烧录；
- [x] A1 Client 成功配置；
- [x] A1 Client 成功编译；
- [x] A1 Client 成功烧录；
- [x] A1 成功扫描到 T1；
- [x] A1 成功连接 T1；
- [x] A1 成功获得有效 `conn_id`；
- [x] A1 成功连续读取 T1 的连接态 RSSI；
- [x] T1 靠近时 RSSI 总体增强；
- [x] T1 远离时 RSSI 总体减弱；
- [x] 单锚点 SLE RSSI 最小闭环成功跑通。

### 尚未完成

- [ ] 官方示例代码结构审查；
- [ ] RSSI API 和回调链路分析；
- [ ] Server 多连接能力确认；
- [ ] 两个锚点同时连接 T1；
- [ ] 四个锚点同时采集；
- [ ] 锚点身份标识；
- [ ] 多路 RSSI 时间对齐；
- [ ] 上位机串口汇总；
- [ ] 固定采样点规划；
- [ ] 四区域数据集采集；
- [ ] 四区域分类模型。

---

## 13. 本阶段结论

本阶段已经证明：

> 基于 WS63 SDK 官方 SLE RSSI Ranging 示例，T1 与 A1 能够成功建立 SLE 连接，A1 能够周期性读取 T1 的真实连接态 RSSI。RSSI 在 T1 靠近和远离 A1 时呈现正确的总体变化趋势，说明单锚点 RSSI 数据链路已经跑通。

同时确认：

1. RSSI 单次测量存在明显波动；
2. RSSI 与距离只具有总体相关性，不是严格单调关系；
3. 默认距离模型未经过现场标定，距离结果暂不可信；
4. median 和 filtered 会提高稳定性，但会引入明显的动态响应滞后；
5. 后续定位数据采集应保留原始 RSSI，由上位机统一进行统计和滤波。

---

## 14. 下一阶段


1. 审查官方 Server 和 Client 示例代码；
2. 确认连接状态、`conn_id` 和 RSSI 回调的管理方式；
3. 检查当前代码是否按单连接设计；
4. 使用 T1、A1、A2 进行双锚点验证；
5. 判断连接态多锚点方案是否可行；
6. 再决定扩展到四连接，或改用广播扫描方式；
7. 形成固定采样点与人工标注方案；
8. 与老师确认测试区域和第一阶段验收口径。
