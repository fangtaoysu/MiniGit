# MiniGit
complish git by cpp

MiniGit - 一个简化版的版本控制系统，模仿Git的基本功能，包括：

- 初始化仓库
- 添加/删除文件跟踪
- 提交更改
- 查看提交历史
- 分支管理
- 简单的合并功能

# 差缺补漏

## CMake

1. 执行`cmake ..`：
    - 生成构建系统：读CMakeLists.txt文件，生成MakeFile
    - 处理依赖关系：解析`find_package()`指令；检查编译器是否支持指定的cpp标准；验证头文件路径和库路径是否有效
    - 缓存变量：生成`CMakeCache.txt`文件，保存：编译器路径、项目配置选项和依赖库路径
2. 执行`make`（期间会生成关键信息）：
    - 读取`MakeFile`文件
    - 检查文件变更：对比源文件（.cpp）和目标文件（.o）的时间戳，如果不一致则重新编译，否则跳过编译
    - 编译和链接生成可执行文件

测试：`ctest`

想输出更详细的信息（如日志）:`ctest --verbose`

# 遇到的问题

## issue1:undefined reference to 'main'

> Anaconda 安装的 GTest 与系统 GTest 版本不兼容，导致符号解析失败（typeinfo for testing::Test）
解决方案：显式指定了系统 GTest 路径
`set(GTest_DIR "/usr/lib/x86_64-linux-gnu/cmake/GTest")`

## commit的一条信息如果用流存储，那么使用什么作为分隔符呢

> commit的一条信息包括父节点的哈希值、当前节点的哈希值、当前时间戳和commit信息，由于commit信息中可以存在空格或回车，因此使用传统的空格和回车作为流的分隔符是不合理的
解决方案：采用结构序列化的方式
长度前缀序列化：将下一个字符串的长度和空格存入其中（序列化），读取的时候先读下一个字符的长度，就能根据长度读取指定的字符串；
本项目采用了json序列化的方式