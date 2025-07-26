#include "../include/object_db.h"
#include <fstream>
#include <iostream>


ObjectDB::ObjectDB(const std::string& project_path)
    : project_path_(project_path) {

}

// std::vector<std::string> ObjectDB::read(const std::string& hash) {

// }


bool ObjectDB::write_path(const fs::path& src_path, const fs::path& des_path) {
    try {
        fs::copy_file(src_path, des_path, fs::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

bool ObjectDB::write_json(const json& write_content, const fs::path& dst_path) {
    std::ofstream out(dst_path);
    if (!out) {
        return false;
    }
    out << write_content.dump(4);
    return true;
}

bool ObjectDB::write_lines(const std::vector<std::string>& write_contents, const fs::path& dst_path) {
    std::ofstream out(dst_path);
    if (!out) {
        return false;
    }

    for (const std::string& line : write_contents) {
        out << line << "\n";
    }
    return true; 
}