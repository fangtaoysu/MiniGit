/**实现git暂存区的功能 */
#ifndef __INDEX_H__
#define __INDEX_H__
#include <string>
#include "utils.h"
#include "../third_party/nlohmann/json.hpp"


using json = nlohmann::json;
namespace fs = std::filesystem;
class Index {
    public:
        Index()=default;
        void add(const std::vector<fs::path>* files) const;
        static json get_index();
        void reset_index_entries();
        void status();
        
    private:
        const std::string project_path_ = Utils::get_project_path();
        void write_to_index(json file_info) const;
        fs::path index_path_ = fs::path(project_path_) / ".mgit/index";
        void print_status(const std::vector<fs::path> &to_be_commited, const std::vector<fs::path> &not_staged, const std::vector<fs::path> &untrack);

};


#endif