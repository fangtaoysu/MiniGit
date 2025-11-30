# Git存储实现详解：MySQL配合机制

## 一、Git存储的核心原理

### 1.1 内容寻址存储（Content-Addressable Storage）

Git采用**内容寻址**的方式存储对象，核心思想是：
- **对象ID = SHA-1(对象内容)**
- 相同内容的对象具有相同的SHA-1哈希值
- 通过SHA-1可以直接定位到对象数据

### 1.2 三种核心对象

#### Blob对象（文件内容）
```cpp
// 存储格式：header + content
// header = "blob " + size + "\0"
// SHA-1 = Hash(header + content)
```

**序列化过程**（参考 `src/domain/core/blob.cc`）：
1. 读取文件内容到 `std::array<uint8_t, 20>`
2. 构建header：`"blob " + std::to_string(size) + '\0'`
3. 计算SHA-1：`HashUtil::Sha1Hex(header, data)`
4. 序列化：返回 `std::span<const uint8_t>` 指向原始数据

#### Tree对象（目录结构）
```cpp
// 存储格式：每个条目为 "mode obj_type sha1\tname\n"
// 例如："100644 blob abc123...\tREADME.md\n"
```

**序列化过程**（参考 `src/domain/core/tree.cc`）：
1. 遍历所有条目（TreeEntry）
2. 格式化为字符串：`mode + " " + obj_type + " " + sha1 + "\t" + name + "\n"`
3. 计算SHA-1：对整个字符串计算哈希
4. 序列化：转换为 `std::vector<uint8_t>`

#### Commit对象（提交记录）
```cpp
// 存储格式：
// tree <tree_sha1>
// parent <parent_sha1>  (可能有多个)
// author <author_info>
// committer <committer_info>
// 
// <commit_message>
```

**序列化过程**（参考 `src/domain/core/commit.cc`）：
1. 构建文本格式的提交数据
2. 包含tree引用、parent引用、作者信息、提交信息
3. 计算SHA-1：对整个序列化数据计算哈希
4. 序列化：转换为 `std::vector<uint8_t>`

## 二、架构分层设计

### 2.1 领域层（Domain Layer）

**核心对象**（`src/domain/core/`）：
- `Blob`：文件内容对象
- `Tree`：目录结构对象
- `Commit`：提交对象

**存储抽象接口**（`src/domain/storage/repositories/`）：
```cpp
class ObjectRepository {
public:
    // 存储接口
    virtual bool StoreBlob(const Blob& blob) = 0;
    virtual bool StoreCommit(const Commit& commit) = 0;
    virtual bool StoreTree(const Tree& tree) = 0;
    
    // 加载接口
    virtual std::unique_ptr<Blob> LoadBlob(const std::string& sha1) = 0;
    virtual std::unique_ptr<Commit> LoadCommit(const std::string& sha1) = 0;
    virtual std::unique_ptr<Tree> LoadTree(const std::string& sha1) = 0;
    
    // 查询接口
    virtual bool Exists(const std::string& sha1) = 0;
    virtual std::vector<std::string> GetAllObjectHashes() = 0;
};
```

**关键点**：
- 领域层**不关心**具体存储方式（文件系统、MySQL、Redis等）
- 只定义**接口契约**，由基础设施层实现

### 2.2 应用层（Application Layer）

**业务服务**（`src/application/`）：
- `AddService`：添加文件到暂存区
- `CommitService`：创建提交
- `InitService`：初始化仓库

**工作流程示例**（Add操作）：
```cpp
// 1. 读取文件，创建Blob对象
Blob blob = Blob::CreateFromFile(file_path);

// 2. 计算SHA-1（Blob对象内部完成）
std::string sha1 = blob.CalculateSha1();

// 3. 存储Blob到仓库（通过接口，不关心具体实现）
object_repository_->StoreBlob(blob);

// 4. 添加到暂存区
index_repository_->StageFile(file_path, sha1);
```

### 2.3 基础设施层（Infrastructure Layer）

**MySQL实现**（`src/infrastructure/database/`）：
- `MySQLObjectRepository`：实现 `ObjectRepository` 接口
- `MySQLConnectionPool`：管理数据库连接池
- `DatabaseManager`：数据库初始化和管理

## 三、MySQL配合机制详解

### 3.1 数据库表结构

#### git_objects表（核心存储表）
```sql
CREATE TABLE git_objects (
    sha1 CHAR(40) PRIMARY KEY,        -- SHA-1哈希值作为主键（内容寻址）
    type ENUM('blob', 'commit', 'tree') NOT NULL,  -- 对象类型
    data LONGBLOB NOT NULL,            -- 序列化后的对象数据（二进制）
    size INT NOT NULL,                 -- 数据大小
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_type (type),
    INDEX idx_created_at (created_at)
);
```

**设计要点**：
- **SHA-1作为主键**：实现O(1)的查找性能
- **LONGBLOB存储**：可以存储最大4GB的二进制数据
- **类型字段**：区分blob、commit、tree三种对象

#### references表（引用管理）
```sql
CREATE TABLE references (
    name VARCHAR(255) PRIMARY KEY,    -- "HEAD", "refs/heads/main"
    target VARCHAR(40) NOT NULL,      -- 指向的commit SHA1
    type ENUM('head', 'branch', 'tag') NOT NULL
);
```

#### staging_index表（暂存区）
```sql
CREATE TABLE staging_index (
    file_path VARCHAR(1024) PRIMARY KEY,
    blob_sha1 CHAR(40) NOT NULL,      -- 指向git_objects表中的blob
    mode VARCHAR(6) NOT NULL DEFAULT '100644'
);
```

### 3.2 MySQLObjectRepository实现流程

#### 存储流程（StoreBlob示例）

```cpp
bool MySQLObjectRepository::StoreBlob(const Blob& blob) {
    // 1. 获取Blob的SHA-1（已在Blob对象中计算好）
    std::string sha1 = blob.CalculateSha1();
    
    // 2. 检查是否已存在（避免重复存储）
    if (Exists(sha1)) {
        return true;  // 内容寻址：相同内容=相同SHA-1，已存在则无需重复存储
    }
    
    // 3. 序列化Blob对象
    auto serialized_data = blob.Serialize();  // 返回 std::span<const uint8_t>
    std::vector<uint8_t> data(serialized_data.begin(), serialized_data.end());
    
    // 4. 调用通用存储方法
    return StoreObject(sha1, "blob", data);
}

bool MySQLObjectRepository::StoreObject(
    const std::string& sha1, 
    const std::string& type, 
    const std::vector<uint8_t>& data) {
    
    // 1. 从连接池获取连接
    auto conn = connection_pool_.GetConnection();
    
    // 2. 准备SQL语句
    std::string sql = "INSERT INTO git_objects (sha1, type, data, size) "
                      "VALUES (?, ?, ?, ?) "
                      "ON DUPLICATE KEY UPDATE sha1=sha1";  // 幂等性保证
    
    auto stmt = conn->prepareStatement(sql);
    stmt->setString(1, sha1);
    stmt->setString(2, type);
    stmt->setBlob(3, data.data(), data.size());  // 二进制数据
    stmt->setInt(4, data.size());
    
    // 3. 执行插入
    bool success = stmt->executeUpdate() > 0;
    
    // 4. 归还连接到池
    connection_pool_.ReturnConnection(conn);
    
    return success;
}
```

#### 加载流程（LoadBlob示例）

```cpp
std::unique_ptr<Blob> MySQLObjectRepository::LoadBlob(const std::string& sha1) {
    // 1. 从数据库加载原始数据
    std::vector<uint8_t> data = LoadObjectData(sha1);
    if (data.empty()) {
        return nullptr;
    }
    
    // 2. 反序列化为Blob对象
    std::span<const uint8_t> data_span(data);
    Blob blob = Blob::Deserialize(data_span);
    
    return std::make_unique<Blob>(blob);
}

std::vector<uint8_t> MySQLObjectRepository::LoadObjectData(const std::string& sha1) {
    // 1. 从连接池获取连接
    auto conn = connection_pool_.GetConnection();
    
    // 2. 查询SQL
    std::string sql = "SELECT data FROM git_objects WHERE sha1 = ?";
    auto stmt = conn->prepareStatement(sql);
    stmt->setString(1, sha1);
    
    // 3. 执行查询
    auto result = stmt->executeQuery();
    if (!result->next()) {
        connection_pool_.ReturnConnection(conn);
        return {};
    }
    
    // 4. 读取LONGBLOB数据
    std::istream* blob_stream = result->getBlob(1);
    std::vector<uint8_t> data;
    char buffer[4096];
    while (blob_stream->read(buffer, sizeof(buffer))) {
        data.insert(data.end(), buffer, buffer + blob_stream->gcount());
    }
    data.insert(data.end(), buffer, buffer + blob_stream->gcount());
    
    // 5. 归还连接
    connection_pool_.ReturnConnection(conn);
    
    return data;
}
```

### 3.3 连接池机制

**为什么需要连接池**：
- 数据库连接创建/销毁开销大
- 高并发场景下频繁创建连接会成为瓶颈
- 连接池复用连接，提高性能

**MySQLConnectionPool工作流程**：
```cpp
class MySQLConnectionPool {
    std::queue<std::shared_ptr<sql::Connection>> connections_;
    std::mutex mutex_;
    std::condition_variable condition_;
    
    std::shared_ptr<sql::Connection> GetConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        // 等待直到有可用连接
        condition_.wait(lock, [this] { return !connections_.empty(); });
        
        auto conn = connections_.front();
        connections_.pop();
        return conn;
    }
    
    void ReturnConnection(std::shared_ptr<sql::Connection> conn) {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.push(conn);
        condition_.notify_one();
    }
};
```

## 四、完整数据流转示例

### 4.1 添加文件（git add）流程

```
用户执行: git add README.md
    ↓
1. AddService::AddFile("README.md")
    ↓
2. Blob::CreateFromFile("README.md")
   - 读取文件内容
   - 创建Blob对象
   - 计算SHA-1: "abc123..."
    ↓
3. object_repository_->StoreBlob(blob)
   - MySQLObjectRepository::StoreBlob()
   - 序列化Blob数据
   - INSERT INTO git_objects (sha1='abc123', type='blob', data=..., size=...)
    ↓
4. index_repository_->StageFile("README.md", "abc123")
   - INSERT INTO staging_index (file_path='README.md', blob_sha1='abc123')
```

### 4.2 创建提交（git commit）流程

```
用户执行: git commit -m "Initial commit"
    ↓
1. CommitService::CreateCommit(message, author, committer)
    ↓
2. 从暂存区构建Tree对象
   - index_repository_->GetStagedFiles()
   - 递归构建Tree（目录结构）
   - 为每个Tree计算SHA-1
   - StoreTree() 存储所有Tree对象
    ↓
3. 创建Commit对象
   - Commit commit(tree_sha1, parent_commits, author, committer, message)
   - 计算Commit的SHA-1: "def456..."
    ↓
4. object_repository_->StoreCommit(commit)
   - INSERT INTO git_objects (sha1='def456', type='commit', data=..., size=...)
    ↓
5. reference_repository_->UpdateHead("def456")
   - UPDATE references SET target='def456' WHERE name='HEAD'
    ↓
6. index_repository_->ClearStaged()
   - DELETE FROM staging_index
```

### 4.3 读取对象（git checkout）流程

```
用户执行: git checkout <commit_sha1>
    ↓
1. LoadCommit(commit_sha1)
   - SELECT data FROM git_objects WHERE sha1 = ?
   - Commit::Deserialize(data)
   - 获取tree_sha1
    ↓
2. LoadTree(tree_sha1)
   - SELECT data FROM git_objects WHERE sha1 = ?
   - Tree::Deserialize(data)
   - 获取所有TreeEntry（包含子对象SHA-1）
    ↓
3. 递归加载所有Tree和Blob
   - 对每个TreeEntry，如果是Tree则递归LoadTree
   - 如果是Blob则LoadBlob
    ↓
4. 重建工作目录
   - 根据Tree结构创建目录
   - 将Blob内容写入文件
```

## 五、MySQL vs 传统文件系统存储

### 5.1 传统Git存储（文件系统）

```
.git/objects/
├── ab/
│   └── c123...  (压缩后的blob数据)
├── cd/
│   └── e456...  (压缩后的tree数据)
└── ef/
    └── 789...   (压缩后的commit数据)
```

**特点**：
- 使用文件系统目录结构
- 对象数据zlib压缩
- SHA-1前2位作为目录名，后38位作为文件名

### 5.2 MySQL存储（本项目）

```
git_objects表
┌──────────┬──────┬──────────┬──────┐
│ sha1     │ type │ data     │ size │
├──────────┼──────┼──────────┼──────┤
│ abc123...│ blob │ [二进制] │ 1024 │
│ cde456...│ tree │ [二进制] │ 256  │
│ efg789...│commit│ [二进制] │ 512  │
└──────────┴──────┴──────────┴──────┘
```

**优势**：
1. **集中管理**：所有对象在一个表中，便于备份和迁移
2. **事务支持**：可以保证操作的原子性
3. **查询优化**：可以利用MySQL的索引和查询优化器
4. **并发控制**：MySQL的锁机制保证并发安全
5. **扩展性**：可以轻松添加元数据字段（如created_at）

**劣势**：
1. **性能开销**：相比直接文件系统访问，有网络和SQL解析开销
2. **存储效率**：LONGBLOB可能不如文件系统压缩效率高
3. **依赖数据库**：需要MySQL服务运行

## 六、关键设计模式

### 6.1 依赖倒置原则（DIP）

```
应用层 (AddService)
    ↓ 依赖接口
领域层 (ObjectRepository接口)
    ↑ 实现接口
基础设施层 (MySQLObjectRepository)
```

**好处**：
- 应用层不依赖具体实现
- 可以轻松切换存储后端（文件系统、MySQL、Redis等）
- 便于单元测试（可以mock接口）

### 6.2 仓储模式（Repository Pattern）

**ObjectRepository** 封装了数据访问逻辑：
- 应用层不需要知道数据存在哪里（MySQL、文件系统等）
- 统一的接口：Store/Load/Exists
- 隐藏了数据库细节（SQL、连接管理等）

### 6.3 内容寻址的优势

1. **去重**：相同内容的对象只存储一份
2. **完整性校验**：通过SHA-1可以验证数据完整性
3. **快速查找**：SHA-1作为主键，O(1)查找
4. **不可变性**：内容改变，SHA-1就改变，天然支持版本控制

## 七、总结

这个MiniGit项目通过**分层架构**和**依赖注入**，将Git的对象存储抽象化，然后用MySQL作为具体实现：

1. **领域层**定义核心对象（Blob/Tree/Commit）和存储接口
2. **应用层**使用接口完成业务逻辑（add/commit等）
3. **基础设施层**用MySQL实现存储接口

**MySQL的作用**：
- 作为持久化存储后端
- 通过`git_objects`表存储所有Git对象
- 使用SHA-1作为主键实现内容寻址
- 通过连接池管理数据库连接，提高性能

这种设计使得系统**可扩展**（可以轻松切换存储后端）、**可测试**（可以mock接口）、**可维护**（职责清晰，分层明确）。

