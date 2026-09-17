# CSV Refresh

**AI 或其他程序修改 CSV 后，按 Ctrl+R 重新读取文件，查看最新内容。**

CSV Refresh 是基于 [Tablecruncher](https://github.com/Tablecruncher/tablecruncher) 的实验性修改版，保留原项目的桌面表格编辑功能，新增从磁盘刷新的操作。适合让 AI、脚本或外部编辑器修改本地 CSV，再回到表格窗口检查结果的工作流程。

当前功能位于 [`feature/ctrl-r-refresh`](https://github.com/qeawasd/tablecruncher/tree/feature/ctrl-r-refresh) 分支。`main` 分支仍为上游代码。项目采用 **C++17 + FLTK + CMake**，无需部署网页服务，也无需配置 AI API。

> 当前状态：Windows x64 Release 已编译通过，解析回归测试已通过；GUI 端到端验收尚未完成。此版本暂未发布 GitHub Release，仓库中不包含预编译安装包。应用窗口和构建产物目前仍沿用上游名称。

## 为什么做这个工具

CSV 在磁盘上已经被 AI 修改，但编辑器仍显示打开时的内容，就很难确认修改结果。这个分支增加一个明确的刷新入口：重新读取当前文件，并在替换表格之前处理未保存修改和读取失败。

```text
打开 CSV → AI / 脚本修改同一个文件并保存 → 回到窗口按 Ctrl+R → 查看最新内容
```

AI 可以使用你现有的工具。CSV Refresh 本身不调用模型；双方只需要操作同一个本地文件。

## 新增功能

| 功能 | 当前行为 |
| --- | --- |
| 快捷键刷新 | Windows / Linux 使用 Ctrl+R；macOS 使用 Cmd+R |
| 菜单入口 | File → Refresh from Disk |
| 复用 CSV 格式 | 沿用当前分隔符、编码等设置，重新检查 BOM，不弹出格式选择框 |
| 未保存修改保护 | 刷新前提示 Cancel 或 Discard and Refresh；正在编辑的单元格也纳入检查 |
| 读取失败保护 | 先解析到临时表格，读取成功后才替换当前内容 |
| 异常文件检查 | 文件不可读、空文件、未闭合引号或读取期间检测到大小 / 修改时间变化时，取消替换 |
| 视图位置 | 尽量保留选区和滚动位置；文件缩小时将位置限制到有效范围 |

本项目同时继承上游的 CSV 编辑、查找、排序、JavaScript 宏、多种编码及主题等功能。完整的上游介绍见 [README_UPSTREAM.md](README_UPSTREAM.md)。

## 使用方法

1. 启动编译后的程序，通过 **File → Open** 打开本地 CSV。
2. 使用 AI、脚本或其他编辑器修改**同一路径**的文件，并等待写入完成。
3. 回到 CSV 窗口，按 **Ctrl+R**，或选择 **File → Refresh from Disk**。
4. 如果窗口里有未保存修改，根据提示选择：
   - **Cancel**：保留当前编辑，取消刷新。
   - **Discard and Refresh**：放弃当前未保存修改，读取磁盘上的内容。

刷新不会把当前表格写回磁盘。成功刷新后，旧撤销记录和行标记会被清除；取消刷新或读取失败时，当前表格保留。新建且尚未保存的文档需要先保存，才能从磁盘刷新。

如果文件的分隔符或编码发生变化，请使用原有的 **File → Reopen ...** 重新选择格式。

## 界面参考

下图来自上游项目，用于展示原有表格界面，未展示本分支新增的刷新菜单。

![上游 Tablecruncher 的表格编辑界面](assets/artwork/screenshot.png)

## 从源码构建

### 获取修改分支

```sh
git clone --branch feature/ctrl-r-refresh https://github.com/qeawasd/tablecruncher.git csv-refresh
cd csv-refresh
```

### Windows

需要 Visual Studio 的 C++ 桌面开发工具、Windows SDK、Git 和 CMake。以下命令在 **x64 Native Tools Command Prompt for VS** 中运行，起始目录为上面克隆的 `csv-refresh` 文件夹。

先在相邻目录构建 FLTK：

```bat
git clone --depth 1 --branch release-1.4.3 https://github.com/fltk/fltk.git ../fltk
cmake -S ../fltk -B ../fltk-build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DFLTK_MSVC_RUNTIME_DLL=ON -DFLTK_BUILD_TEST=OFF -DFLTK_BUILD_EXAMPLES=OFF -DFLTK_BUILD_FLUID=OFF -DFLTK_BUILD_FLTK_OPTIONS=OFF
cmake --build ../fltk-build
```

再构建应用：

```bat
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DFLTKINCDIR="%CD%/../fltk" -DFLTKLIBDIR="%CD%/../fltk-build"
cmake --build build
```

构建产物为 `build/dist/Tablecruncher.exe`。这里使用动态 MSVC 运行库；运行机器需要相应的 Visual C++ 运行库。

已通过的构建环境：Windows x64、MSVC 14.51、CMake 3.31.6、FLTK 1.4.3。上游 Windows 构建说明中的运行库选项与这里不同，复现本分支构建时请保持应用与 FLTK 的运行库设置一致。

### macOS / Linux

参考上游 [BUILD.md](BUILD.md) 准备编译环境。刷新代码使用跨平台接口，但本分支尚未在 macOS 或 Linux 上完成构建及运行验证。

## 验证状态

| 检查 | 结果 |
| --- | --- |
| Windows x64 Release 编译 | 通过 |
| 多行字段与前导零解析 | 通过 |
| 未闭合引号检测 | 通过 |
| 指定分隔符及空文件解析 | 通过 |
| 快捷键、弹窗、多窗口等 GUI 验收 | 待完成 |
| macOS / Linux 构建及运行 | 待验证 |

解析测试源码见 [tests/refresh_parser.cpp](tests/refresh_parser.cpp)。本次通过链接应用构建对象和 FLTK 库运行测试，尚未接入 CTest。手动验收步骤和实现记录见 [REFRESH.md](REFRESH.md)。

## 已知限制

- **当前只有手动刷新**，尚未加入文件监听或自动刷新。
- 刷新在界面线程执行，大文件可能导致窗口暂时无响应；临时表格还会增加内存占用。
- 修改时间和文件大小检查不能完全排除并发写入，建议等待 AI 或脚本完成写入后再刷新。
- 暂不支持合并本地未保存编辑与外部修改，需要选择保留编辑或放弃后刷新。
- 空文件会被视为刷新失败，保留现有表格。

后续可考虑文件变化监听、切回窗口时检查更新、后台读取及更完整的冲突处理；这些功能目前尚未实现。

## 来源与许可证

本项目 fork 自 [Tablecruncher/tablecruncher](https://github.com/Tablecruncher/tablecruncher)，刷新功能最初基于上游提交 `c6fec63` 开发。感谢 Stefan Fischerländer 及上游贡献者。

源码沿用 [GPL-3.0-or-later](LICENSE)。UI 图标版权归 Stefan Fischerländer 所有，可按 GPL-3.0-or-later 或 CC BY 4.0 使用；上游应用标志按 GPL-3.0-or-later 授权。发布修改版时请遵循上游关于使用不同名称和标志的要求。

第三方库及原始署名保留在 [上游 README](README_UPSTREAM.md#third-party-libraries-used-in-tablecruncher) 和 [external](external/) 目录中。
