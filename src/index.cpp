#include "../include/index.h"
#include "../include/file_system.h"
#include "../include/utils.h"
#include "../include/object_db.h"
#include "../include/thread_pool.h"
#include <fstream>
#include <filesystem>
#include <thread>


namespace fs = std::filesystem;


/**将 .mgit/index 文件读到json中 */
json Index::get_index() {
    const std::string project_path = Utils::get_project_path();
    const std::string index_path = project_path + "/.mgit/index";
    json index = {};
    if (!fs::exists(index_path)) {
        return index;
    }
    std::ifstream(index_path) >> index;
    return index;
}

void Index::reset_index_entries() {
    json index = Index::get_index();
    index["entries"]["counts"] = 0;
    index["entries"]["create_files"].clear();
    for (auto& item : index) {
        if (item.contains("is_commit")) {
            item["is_commit"] = true;
        }
    }
    write_to_index(index);
}

/**将Json文件写入 .mgit/index 中*/
void Index::write_to_index(json file_info) const {
    fs::path index_parent_path = index_path_.parent_path();

    if (!index_parent_path.empty() && !fs::exists(index_parent_path)) {
        fs::create_directories(index_parent_path);
    }
    std::ofstream(index_path_) << file_info.dump(2);
}

void Index::add(const std::vector<fs::path>* files) const {
    ObjectDB db;
    // 1. 读原有的index文件，如果index为空，那么构造一个空字典
    json index_info = Index::get_index();

    // 2. 得到需要add的文件
    std::vector<fs::path> all_files;
    if (!files) {
        all_files = FileSystem::get_all_files(project_path_);
        files = &all_files;
    }
    size_t count = 0;
    std::vector<std::string> create_files;
    ThreadPool pool(std::thread::hardware_concurrency());
    std::mutex mtx;
    
    const std::vector<fs::path> filitered_files = Utils::filter_files(files);
    for (auto & file : filitered_files) {
        // 如果当前文件位于 .mgitignore中，跳过
        pool.enqueue([&, file] {
            std::string hash_value;
            bool is_commit = true;
            
            if (FileSystem::is_text_file(file)) {
                // 如果是文本文件，读取内容并根据内容生成哈希
                std::stringstream contents = FileSystem::read_file(file);
                hash_value = Utils::get_hash(contents.str());
            } else {
                // 否则根据文件记录生成哈希
                hash_value = Utils::get_hash(file.string());
            }

            // 获取文件信息
            db.write(hash_value, file);
            int64_t size =  FileSystem::get_file_size(file);
            int64_t modified = FileSystem::get_file_timestamp(file);
            
            // 给index_info和count加锁
            std::lock_guard<std::mutex> lock(mtx);
            
            // 判断是否是修改的文件，是否是新增的文件
            auto it = index_info.find(file);
            if (it == index_info.end()) { // 如果当前路径不在其中，说明是新建的文件
                create_files.push_back(file);
                ++count;
                is_commit = false;
            } else if (index_info[file]["hash"] != hash_value) {
                ++count; // 如果是修改的的文件
                is_commit = false;
            }

            // 2.3 得到这个文件的其他信息
            index_info[file] = {
                {"hash", hash_value},
                {"size", size},
                {"modified", modified},
                {"is_commit", is_commit}
            };
        });
    }
    pool.wait(); // 等所有线程都执行结束
    index_info["entries"] = {
        {"counts", count}, // 有变化文件的数量
        {"create_files", create_files}
    };
    // 并写回到这个字典
    write_to_index(index_info);
}

/** 根据add的原理，判断当前还有哪些文件没有add
 * 新建文件（untrack） 未add的（not staged） 未commit的（to be committed)
 */
void Index::status() {
    json index = Index::get_index();
    std::vector<fs::path> untrack, not_staged, to_be_commited;
    std::vector<fs::path> all_files = FileSystem::get_all_files(project_path_);
    const std::vector<fs::path> filitered_files = Utils::filter_files(&all_files);
    for (auto& file : filitered_files) {
        int64_t now_modified = FileSystem::get_file_timestamp(file);
        // 对file进行分类
        fs::path relative_path = fs::relative(file, project_path_);
        if (!index.contains(file)) { // 如果为空字符 - untrack
            untrack.push_back(relative_path);
        } else if (index.at(file).at("is_commit")) {
            continue; // 文件已经commit
        } else {
            int64_t index_modified = index.at(file).at("modified");
            if (now_modified != index_modified) { // 不一致 - not staged
                not_staged.push_back(relative_path);
            } else if (now_modified == index_modified) { // 一致 - to be committed
                to_be_commited.push_back(relative_path);
            }
        }
    }

    // 打印
    print_status(to_be_commited, not_staged, untrack);
}

void Index::print_status(const std::vector<fs::path> &to_be_commited, const std::vector<fs::path> &not_staged, const std::vector<fs::path> &untrack) {
    std::vector<std::string> tips{
        "Changes to be committed:\n\t(use \"git restore --staged <file>...\" to unstage)\n",
        "Changes not staged for commit:\n\t(use \"git add <file>...\" to update what will be committed)\n\t(use \"git restore <file>...\" to discard changes in working directory)\n",
        "Untracked files:\n\t(use \"git add <file>...\" to include in what will be committed)\n",
        "nothing to commit, working tree clean\n"
    };
    size_t tips_index = 0;
    
    if (!to_be_commited.empty()) {
        std::cout << tips[tips_index];
    }
    ++tips_index;
    for (auto& file : to_be_commited) {
        std::cout << "\tmodified:\t" << file << std::endl;
    }
    if (!not_staged.empty()) {
        std::cout << tips[tips_index];
    }
    ++tips_index;
    for (auto& file : not_staged) {
        std::cout << "\tmodified:\t" << file << std::endl;
    }
    if (!untrack.empty()) {
        std::cout << tips[tips_index];
    }
    ++tips_index;
    for (auto& file : untrack) {
        std::cout << "\t\t" << file << std::endl;
    }

    if (to_be_commited.empty() && not_staged.empty() && untrack.empty()) {
        std::cout << tips[tips_index];
    }

}