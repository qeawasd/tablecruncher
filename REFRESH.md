# Ctrl+R 本地修改版

基于 https://github.com/Tablecruncher/tablecruncher ，分支 `feature/ctrl-r-refresh`。
远程 fork：https://github.com/qeawasd/tablecruncher 。Ctrl+R 修改保存在 `feature/ctrl-r-refresh` 分支。

## 使用

打开 CSV 后，按 Windows/Linux 的 Ctrl+R（macOS 为 Cmd+R），或选 File > Refresh from Disk。
刷新沿用当前分隔符和编码，重新检查 BOM，不弹出格式选择框。
未保存的修改会先询问是否放弃。成功刷新会清除旧撤销记录和行标记，并尽量保留选择与滚动位置。
读取失败、空文件、未闭合引号以及读取期间检测到文件大小/修改时间变化时，保留原表格。
自动刷新尚未实现。大文件刷新在界面线程进行，期间窗口可能暂时无响应。
文件时间和大小检查不能完全避免并发写入竞态；建议等 AI 写入完成再刷新。

## 手动验收

1. 打开 CSV，用外部编辑器修改并保存，按 Ctrl+R，确认显示新内容。
2. 修改一个单元格但不保存，刷新时选择 Cancel，确认修改仍然存在。
3. 再次刷新并选择 Discard and Refresh，确认使用磁盘内容且清除未保存标记。
4. 在单元格编辑状态按 Ctrl+R，确认输入也纳入未保存修改提示。
5. 缩短 CSV 行列数量后刷新，确认选区和滚动位置没有越界。
6. 临时移走文件、清空文件、写入未闭合引号，再刷新，确认旧表格保留。
7. 使用 UTF-8 BOM、中文路径、分号、制表符和单元格内换行的文件复测。
8. 打开多个文件窗口，确认只刷新当前窗口。

## 构建与许可证

构建沿用 BUILD.md 的 C++17、FLTK 1.4 和 CMake 流程。
源码沿用项目 GPL-3.0-or-later 许可证。上游 README 请求发布修改版时使用不同名称和标志。

## 本次验证（2026-09-17）

- 上游基线：c6fec63。
- Windows x64 Release 编译通过（MSVC 14.51、CMake 3.31.6、FLTK 1.4.3）。
- tests/refresh_parser.cpp 链接本次构建的应用对象及 FLTK 库运行通过：多行字段与前导零、未闭合引号、指定分隔符、空文件。
- git diff --check 通过。
- 尚未进行 GUI 端到端验收；上述手动验收清单仍待执行。
- 可执行文件位于本项目父目录 Tablecruncher-refresh.exe，为本地试用构建。
