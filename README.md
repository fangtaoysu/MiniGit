# MiniGit

一个基于 DDD 架构并以 TDD 驱动实现的简化版 Git。命令来自控制台输入，采用“编译同款”管线：词法分析 → 语法分析 → 语义分析 → 分发执行，同时严格遵循分层职责与依赖方向。

## 架构总览
- Presentation：负责字符串命令的识别、词法/语法/语义分析与命令分发，是 CLI 适配层，不含业务流程与复杂领域逻辑。
- Application：负责用例编排与业务流程（如 init、add、commit 等的处理管线），调用 Domain 提供的能力，不做复杂算法实现。
- Domain：封装复杂的领域能力与模型（本地/远程仓库、对象存储、差异与合并算法、引用与索引等），是核心业务逻辑所在。
- Infrastructure：提供基础设施能力（日志、存储、数据库、线程池/并发、配置、文件系统、网络），被其他层通过抽象接口依赖。
- 依赖方向：Presentation → Application → Domain；各层对 Infrastructure 仅依赖抽象接口，具体实现向下替换。

## 命令处理管线（编译同款）
- 词法分析（Scanner）：把输入字符串流切分为 Token（命令、子命令、参数、选项）。
- 语法分析（Parser）：根据命令文法构造 AST，保证结构正确（如 git <sub> [args] [flags]）。
- 语义分析（Analyzer）：类型检查与符号表构建（验证命令存在、参数类型与组合合法、引用/分支是否存在等）。
- 分发执行（Dispatcher）：将语义正确的 AST 映射到 Application 的命令处理器（Use Case），执行业务流程。

## 领域建模（示例）
- 限界上下文：
  - 仓库上下文（Repository BC）：本地/远程仓库、对象存储（Blob/Tree/Commit）、引用（Ref/HEAD/Branch/Tag）。
  - 对比与合并上下文（Diff/Merge BC）：差异计算、三方合并、冲突表示与解决策略。
  - 传输上下文（Transport BC）：远程通信协议抽象（fetch/pull/push），认证与网络容错。
- 核心模型：
  - 实体：`Commit`、`Tree`、`Blob`、`Repository`、`Remote`。
  - 值对象：`Oid`（对象 ID）、`PathSpec`、`DiffHunk`、`Author`、`Signature`。
  - 聚合与仓储：`Repository` 为聚合根，提供对象读写与引用维护；`Remote` 聚合封装远端配置与交互。
- 领域服务：
  - 差异计算服务（基于最短编辑序列/Patience/Histogram 等策略可插拔）。
  - 合并服务（递归树合并、三方合并、冲突标注与策略）。
  - 传输服务（包协议/HTTP(s) 抽象、带重试与超时）。

## 应用用例（示例）
- `init`：初始化本地仓库结构与默认引用。
- `add`：根据 PathSpec 更新索引（暂存区）。
- `commit`：生成提交对象，写入对象库，更新 `HEAD`。
- `status`：对比工作区/索引/提交，汇总改动。
- `branch`/`tag`：创建与管理分支/标签引用。
- `merge`/`rebase`：调用领域合并与重排服务，维护历史与引用。
- `remote`/`fetch`/`pull`/`push`：远端配置与数据同步，用例串联传输服务与引用/对象更新。

## TDD 测试策略
- 测试金字塔：
  - 单元测试：Scanner/Parser/Analyzer、领域服务（Diff/Merge/Transport）各自独立可测试。
  - 集成测试：Application 用例编排与 Infrastructure 适配的协同验证（文件系统/网络可使用替身）。
  - 端到端测试（E2E）：CLI 输入到输出的黑盒验证（含错误路径与帮助信息）。
- 测试驱动：先写失败测试定义行为与边界，再最小实现，迭代重构。
- 约定：
  - 命名：`should_<行为>`、`when_<场景>`、`given_<前置>`。
  - 目录：`tests/unit/...`、`tests/integration/...`、`tests/e2e/...`。
  - 替身：使用接口与依赖注入为 Infrastructure 提供 Mock/Fake。

## 目录结构建议
```
src/
  presentation/        # CLI 适配、Scanner/Parser/Analyzer、Dispatcher
  application/         # 用例服务、命令处理器、DTO、事务边界
  domain/              # 聚合、实体/值对象、领域服务、仓储接口
  infrastructure/      # 日志、存储、FS、DB、网络、线程池等实现
tests/
  unit/
  integration/
  e2e/
```

## 命令文法草案（示例）
- 词法：
  - 标识：`git`、子命令（`init`/`add`/`commit`/...）、参数（文件路径、引用名）、选项（`-m`/`--all` 等）。
- 语法：
  - `command := 'git' subcommand args flags`
  - `subcommand := 'init' | 'add' | 'commit' | 'status' | 'branch' | 'merge' | 'rebase' | 'remote' | 'fetch' | 'pull' | 'push'`
  - `args := arg*`，`flags := flag*`
- 语义规则：
  - 选项与子命令合法性（如 `commit` 需要 `-m` 或使用编辑器）。
  - 引用/对象存在性校验，路径可达性与模式匹配。
  - 不同上下文的兼容性（例如工作区/索引/提交三者关系）。

## 错误处理与日志
- 失败即安全：任何失败不破坏仓库一致性；操作具备幂等/可重试设计。
- 错误分级：用户错误（语法/语义）、业务冲突（合并冲突）、系统错误（IO/网络）。
- 日志级别：`TRACE/DEBUG/INFO/WARN/ERROR`，可配置输出与采样。

## 并发与性能
- 线程池：基础设施提供可配置池；应用用例通过接口请求并发执行（如读对象并行）。
- IO 策略：批量读写、流式处理、大文件分块、零拷贝优先。
- 缓存：对象/引用的读缓存，策略与失效与一致性由领域控制。

## 配置与环境
- 仓库目录：`.minigit/objects`、`.minigit/refs`、`HEAD` 等。
- 远端配置：`remotes` 存储远端名称、URL、认证方式。
- 环境变量：日志级别、并发度、网络超时等。

## 路线图
- M0：搭建分层骨架与依赖注入；完成 Scanner/Parser/Analyzer 与 Dispatcher 的最小可用版本。
- M1：实现 `init`/`add`/`commit`/`status` 基础用例与对象存储。
- M2：实现 `branch`/`tag`/`merge`，提供基本差异与三方合并。
- M3：实现 `remote`/`fetch`/`pull`/`push`，抽象传输协议与远端交互。
- M4：健全 E2E 测试、性能优化与错误恢复策略。

## 贡献与规范
- 代码风格：符合Google C++ 风格指南，保持分层边界清晰，接口优先，拒绝跨层耦合。
- 提交信息：`feat(domain): 支持三方合并`、`fix(infra): 修复网络重试` 等约定式前缀。
- 评审要点：用例是否只编排不实现复杂逻辑；复杂算法是否下沉到领域；基础设施是否通过接口替身可测。
