# MiniGit
complish git by cpp

MiniGit - 一个简化版的版本控制系统，模仿Git的基本功能，包括：

- 初始化仓库
- 添加/删除文件跟踪
- 提交更改
- 查看提交历史
- 分支管理
- 常用的打印

```shell
# 包含以下可用命令
# 一阶段 - 实现最常用的本地命令
git init
git config --local user.name "xxx"
git config --local user.email "xxx"
git add xxx
git commit -m "xxx"
git status
# 二阶段 - 实现分布式
git pull
git push
git reset --soft xxx
git branch
# 三阶段 - 补全漏掉的小功能
git log
git diff
# 考虑加密二进制存储
git cat-file -p xxx
```

## 文件存储思路

```shell
|
|- config # 保存仓库的配置信息
|- logs/
    |- HEAD # 保存所有当前所指分支的所有commit对象（一阶段）
    |- refs/ # 保存本地所有分支，文件名就是分支名（二阶段）
|- index # 保存暂存区中所有文件的信息（路径、哈希值、大小、修改时间）（一阶段）
|- obejcts/ # 保存加入的暂存区和本地仓库的文件大小及文件内容，按照 哈希值 2/38 的路径存储（一阶段）
```

## 对象数据库

> 将所有内容抽象为4中不可变对象
 - blob：文件内容
 - tree：目录结构
 - commit：一次提交，包含提交信息、作者、时间、指向tree的哈希
 - tag：标签，指向某个commit或者对象的标签

 这些对象都被存储在 .git/objects/ 目录中，按照它们的哈希值命名;目录名是前两位，文件名是剩下的 38 位。


## 暂存区(Index/Stage)

文件位于：`.git/index`
主要功能：
 - ​​选择性提交​​：允许你选择哪些修改要包含在下一次提交中
​​ - 准备提交内容​​：暂存修改后的文件，为创建提交做准备
​​ - **文件状态跟踪**​​：记录工作目录中哪些文件已被修改并准备提交

## 命令解析

### add 命令

> 当前目录（.）及其子目录下的**所有修改过的文件（包括新建和修改）**添加到 Git 的 索引区（Index），为下一次提交做准备。

实现：把`.git/index`的内容（路径、哈希值、大小、修改时间）保存在字典中，然后把当前所有文件的暂存区所需信息（路径、哈希值、大小、修改时间）追加到这个字典，再根据哈希把文件保存在对应的 `.git/objects/`路径下

### commit 命令

实现：将本条commit对象保存在`.git/logs/HEAD`中

关于找上次提交的所有文件修改记录问题：
1. `.git/refs/heads/main`
该路径文件保存当前分支（假设是main分支）的引用文件，内容是最新commit的哈希

2. 找到commit对象
通过哈希 `.git/objects/前2位/后38位`的规则找到commit对象，内容中包含tree对象

3. 读tree对象
通过哈希 `.git/objects/前2位/后38位`的规则找到该文件，这个文件存储本次commit中所有新增和修改文件的哈希值（用于查找blob对象）

4. 查看每个blob对象
通过哈希 `.git/objects/前2位/后38位`的规则找到该文件，这个文件中存储着文件大小，文件内容。

# 差缺补漏

## git

```shell
# 查看git对象数据库的内容
git cat-file -p 哈希
# 查看git指定commit的整个目录结构
git ls-tree -r 哈希 # -r 递归
git log --oneline # git记录一行显示
```

## CMake

1. 执行`cmake ..`：
    - 生成构建系统：读CMakeLists.txt文件，生成MakeFile
    - 处理依赖关系：解析`find_package()`指令；检查编译器是否支持指定的cpp标准；验证头文件路径和库路径是否有效
    - 缓存变量：生成`CMakeCache.txt`文件，保存：编译器路径、项目配置选项和依赖库路径
2. 执行`make`（期间会生成关键信息）：
    - 读取`MakeFile`文件
    - 检查文件变更：对比源文件（.cpp）和目标文件（.o）的时间戳，如果不一致则重新编译，否则跳过编译
    - 编译和链接生成可执行文件

调试：
 - cmake -DCMAKE_BUILD_TYPE=Debug -B build
 - make ..

测试：`ctest`

想输出更详细的信息（如日志）:`ctest --verbose`

## 调试

```shell

gdb ./my_program
(gdb) b main.cpp:45 # 打断点
(gdb) run # 运行
# 单步执行
(gdb) s # 进入函数
(gdb) n # 不进入函数
(gdb) bt # 查看调用栈
(gdb) p cmd # 打印变量cmd
```

## linux命令

```shell
find .git/objects -type f | wc -l # 查看文件数
```

## 状态机

> 预先定义若干个有限的解析状态（如：起始状态、命令状态、参数状态、选项键状态、选项值状态等）。

处理过程：
1. 分词处理：对输入字符串进行词法分析，得到一系列的 token（词元）。
2. 状态转移：从初始状态出发，按顺序遍历每个 token：
    - 当前状态决定如何处理当前 token；
    - 处理后决定转移到哪一个下一个状态；
    - 若出现非法 token 或不符合语法的状态转移，则视为解析失败或抛出异常。
3. 最终状态检查：解析结束后，判断当前状态是否为合法的终止状态，以验证输入是否合法。

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


# TODO

1. 添加配置文件
2. 完成.mgitignore
3. 完成push功能