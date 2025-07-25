/**实现git暂存区的功能 */
#ifndef __INDEX_H__
#define __INDEX_H__
#include <string>
#include "../thrid_party/nlohmann/json.hpp"


using json = nlohmann::json;
namespace fs = std::filesystem;
class Index {
    public:
        Index(const std::string& project_path);
        void add(const std::vector<fs::path>* files) const;
        json get_index() const;
        void reset_index_entries();
        void status();
        
    private:
        const std::string project_path_;
        void write_to_index(json file_info) const;
        fs::path index_path_;
        void print_status(const std::vector<fs::path> &to_be_commited, const std::vector<fs::path> &not_staged, const std::vector<fs::path> &untrack);

};


#endif