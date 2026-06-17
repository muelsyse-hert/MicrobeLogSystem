# MicrobeLogSystem

## 项目结构

- `include/app.h`：主入口调用的函数声明
- `include/daily_log_list.h`：每日培养日志模块的结构体和函数声明
- `include/strain_archive_bst.h`：菌株档案 BST 模块的结构体和函数声明
- `include/culture_status.h`：培养状态与快照模块的结构体和函数声明
- `src/daily_log_list_demo.c`：每日培养日志模块实现
- `src/strain_archive_bst_demo.c`：菌株档案 BST 模块实现
- `src/culture_status_demo.c`：培养状态与快照模块实现
- `main.c`：统一入口

## 编译方式

建议使用：

```bash
gcc -Iinclude main.c src/*.c -o MicrobeLogSystem.exe
```

这样 `main.c` 只需要包含 `app.h`，各模块 `.c` 文件各自包含自己的头文件，能避免隐式声明和重复定义问题。
