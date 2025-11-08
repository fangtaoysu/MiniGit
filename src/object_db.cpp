#include "../include/object_db.h"
#include <fstream>
#include <iostream>



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


json ObjectDB::read_json(const fs::path& file_path) {
    if (!fs::exists(file_path)) {
        throw std::runtime_error("File not found: " + file_path.string());
    }
    json res;
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_path.string());
    }
    try {
        file >> res;
        return res;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
}


std::vector<std::string> ObjectDB::read_lines(const fs::path& file_path) {
    if (!fs::exists(file_path)) {
        throw std::runtime_error("File not found: " + file_path.string());
    }    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_path.string());
    }
    std::vector<std::string> res;
    std::string line;

    while (getline(file, line)) {
        res.push_back(line);
    }
    return res;
}