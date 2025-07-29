#include "../include/utils.h"
#include <fstream>
#include <openssl/sha.h> // sha-1 哈希
#include <sstream>
#include <iomanip>
#include <chrono>
#include <stdexcept>
#include <regex>



/** 根据commit的消息生成哈希值(40位) */
const std::string Utils::get_hash(const std::string& hash_source) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    // std::string hash_source = msg_ + get_current_timestamp();
    SHA1(reinterpret_cast<const unsigned char*>(hash_source.c_str()), hash_source.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<int>(hash[i]);
    }
    return ss.str();
}

/**
 * 获取当前时间戳(10位)
 */
const std::string Utils::get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::string timestamp_str = std::to_string(timestamp);
    // 长度不够则补前导0
    if (timestamp_str.length() < 10) {
        timestamp_str = std::string(10 - timestamp_str.length(), '0') + timestamp_str;
    }
    return timestamp_str;
}

/** 分词器 */
std::vector<std::string> Utils::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;
    bool in_quote = false;
    char quote_char = '\0';

    for (size_t i = 0; i < input.size(); ++i) {
        const char c = input[i];
        
        // 先收集范围最大的（引号）
        if ((c == '\'' || c == '"') && (!in_quote || c == quote_char)) {
            in_quote = !in_quote;
            quote_char = in_quote ? c : '\0';
            continue;
        }

        // 在引号内保留所有字符（包括空格和换行）
        if (in_quote) {
            current += c;
            continue;
        }

        // 非引号下的分隔符
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            continue;
        }

        // 普通字符
        current += c;
    }

    // 添加最后一个token
    if (!current.empty()) {
        tokens.push_back(current);
    }

    // 验证引号闭合
    if (in_quote) {
        throw std::runtime_error("Unclosed quote in input");
    }

    return tokens;
}

fs::path Utils::generate_obj_path(const std::string& project_path, const std::string& hash) {
    fs::path obj_dir = project_path + "/.mgit/objects/" + hash.substr(0, 2);
    fs::create_directories(obj_dir);
    if (hash.size() != 40) {
        throw std::invalid_argument("hash must 40 characters long, hash:" + hash);
    }
    return obj_dir / hash.substr(2);
}


bool Utils::is_subpath(const fs::path& parent, const fs::path& child) {
    try {
        auto norm_parent = fs::canonical(parent);
        auto norm_child = fs::canonical(child);

        // 额外检查：确保 child 的路径长度 >= parent
        if (norm_child.string().length() < norm_parent.string().length()) {
            return false;
        }
        
        // mismatch找到第一个不匹配的元素，如果全部匹配，则返回end()
        auto [mismatch, _] = std::mismatch(
            norm_parent.begin(), norm_parent.end(),
            norm_child.begin()
        );

        return mismatch == norm_parent.end();
    } catch (const fs::filesystem_error&) {
        return false;
    }
}


/** 获取项目路径 */
std::string Utils::get_project_path() {
    // 读json文件
    fs::path config_path = fs::current_path() / ".mgit" / "config";
    std::ifstream config_file(config_path);
    json config;
    config_file >> config;

    // 读 project path
    std::string project_path = "";
    if (config.contains("core") && config["core"].contains("worktree")) {
        project_path = config["core"]["worktree"];
    }
    return project_path;
}


#include <iostream> // 添加iostream头文件用于日志输出

std::vector<fs::path> Utils::filter_files(const std::vector<fs::path>* files) {
    std::cout << "\n[DEBUG] Starting filter_files\n";
    std::vector<fs::path> res;
    
    if (!files || files->empty()) {
        std::cout << "[DEBUG] No files to filter or null input\n";
        return res;
    }

    fs::path project_path = get_project_path();
    fs::path ignore_path = project_path / ".mgitignore";
    std::cout << "[DEBUG] Project path: " << project_path << "\n";
    std::cout << "[DEBUG] Ignore file path: " << ignore_path << "\n";

    // 如果没有忽略文件，返回所有文件
    if (!fs::exists(ignore_path)) {
        std::cout << "[DEBUG] No .mgitignore file found, returning all files\n";
        return std::vector<fs::path>(*files);
    }

    // 读取忽略规则
    std::cout << "[DEBUG] Reading ignore patterns from .mgitignore\n";
    std::ifstream ignore_file(ignore_path);
    std::vector<std::string> ignore_patterns;
    std::string line;
    while (getline(ignore_file, line)) {
        if (line.empty() || line[0] == '#') {
            std::cout << "[DEBUG] Skipping comment/empty line: " << line << "\n";
            continue;
        }
        if (line.back() == '/') {
            line.pop_back();
            std::cout << "[DEBUG] Removed trailing / from pattern: " << line << "\n";
        }
        ignore_patterns.push_back(line);
        std::cout << "[DEBUG] Added pattern: " << line << "\n";
    }

    // 过滤文件
    std::cout << "[DEBUG] Starting file filtering\n";
    std::cout << "[DEBUG] Total files to process: " << files->size() << "\n";
    
    for (const auto& file : *files) {
        std::cout << "\n[DEBUG] Processing file: " << file << "\n";
        bool should_include = true;
        fs::path relative_path = fs::relative(file, project_path);
        std::cout << "[DEBUG] Relative path: " << relative_path << "\n";
        
        for (const auto& pattern : ignore_patterns) {
            std::cout << "[DEBUG] Checking against pattern: " << pattern << "\n";
            
            // 处理通配符模式（完整路径匹配）
            if (pattern.find('*') != std::string::npos) {
                std::cout << "[DEBUG] Pattern contains wildcard\n";
                std::string regex_str = std::regex_replace(
                    pattern,
                    std::regex("\\*"),
                    ".*"
                );
                std::cout << "[DEBUG] Converted to regex pattern: " << regex_str << "\n";
                
                std::regex pattern_regex(regex_str);
                if (std::regex_match(relative_path.string(), pattern_regex)) {
                    std::cout << "[DEBUG] Full path matches wildcard pattern - excluding\n";
                    should_include = false;
                    break;
                }
                std::cout << "[DEBUG] No full path match with wildcard pattern\n";
            }
            
            // 处理目录匹配
            fs::path pattern_path(pattern);
            std::cout << "[DEBUG] Checking directory pattern: " << pattern_path << "\n";
            
            if (is_subpath(pattern_path, relative_path)) {
                std::cout << "[DEBUG] File is under pattern directory - excluding\n";
                should_include = false;
                break;
            }
            std::cout << "[DEBUG] File is not under pattern directory\n";
            
            // 精确匹配
            if (relative_path == pattern_path) {
                std::cout << "[DEBUG] Exact path match - excluding\n";
                should_include = false;
                break;
            }
            std::cout << "[DEBUG] No exact path match\n";
        }
        
        if (should_include) {
            std::cout << "[DEBUG] Including file: " << file << "\n";
            res.push_back(file);
        } else {
            std::cout << "[DEBUG] Excluding file: " << file << "\n";
        }
    }

    std::cout << "[DEBUG] Filtering complete. " << res.size() << " files included\n";
    return res;
}