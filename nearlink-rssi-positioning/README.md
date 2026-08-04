# 项目结构：固件与上位机文件结构
准确结构是：

```text
D:\NearLink\workspace
└─ fbb_ws63
   └─ src                         # VS Code / HiSpark 打开这一层
      ├─ ws63_sle_rssi.hiproj
      └─ application
         └─ ws63
            └─ nearlink_rssi_positioning
               ├─ AGENTS.md
               ├─ CMakeLists.txt
               ├─ Kconfig
               ├─ common
               ├─ tag
               └─ anchor
```

最终工作区划分：

| 工程 | 正式代码目录 | VS Code 打开目录 | `AGENTS.md` |
|---|---|---|---|
| WS63 固件 | `D:\NearLink\workspace\fbb_ws63\src\application\ws63\nearlink_rssi_positioning` | `D:\NearLink\workspace\fbb_ws63\src` | 业务专属规则放在固件代码目录内 |
| PC 上位机 | `D:\NearLink\nearlink-rssi-positioning\host` | `D:\NearLink\nearlink-rssi-positioning` | `D:\NearLink\nearlink-rssi-positioning\AGENTS.md` |

另外可在 SDK Git 根目录放一份长期保护规则：

```text
D:\NearLink\workspace\fbb_ws63\AGENTS.md
```

它负责整个 SDK 的共性约束，例如：

- 不直接覆盖官方示例；
- 不手动修改构建产物；
- 修改前检查调用链；
- 编译和烧录前确认目标；
- 保留用户已有配置变更。

而业务模块自己的开发规则放在：

```text
D:\NearLink\workspace\fbb_ws63\src\application\ws63\nearlink_rssi_positioning\AGENTS.md
```

所以结论明确为：

> 固件代码在 `fbb_ws63\src\application\ws63\nearlink_rssi_positioning`，HiSpark 继续打开 `fbb_ws63\src`。`D:\NearLink\nearlink-rssi-positioning\firmware` 不作为正式固件源码目录。

仍然建议固件和上位机使用两个独立 VS Code 窗口，不建立多根工作区。当前没有修改任何文件。