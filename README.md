# This is a simple shell

## 项目结构
```
SimpleShell
-bin/           可执行文件，SimpleShell_main在这个文件夹下
|   |-commands/  存放外部命令
|   |-SimpleShell_main 主程序
|
-commands/      存放外部命令的源文件
-include/       头文件
-src/           shell程序的源文件
-CMakeLists.txt
```

## 已经实现的功能
#### 交互
1. 输入命令
2. 移动光标
3. ctrl+C停止正在执行的命令，ctrl+D退出终端

#### 解析
1. echo exit cd pwd
2. 管道(可以多重管道)

#### 外部命令
1. 程序内部有一个COMMAND_PATH，日后将实现往COMMAND_PATH中添加路径
    ls(-l -a \[PATH\])


