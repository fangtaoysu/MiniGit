/**实现git暂存区的功能 */
#ifndef __INDEX_H__
#define __INDEX_H__
#include <string>
#include "../thrid_party/nlohmann/json.hpp"


using json = nlohmann::json;
namespace fs = std::filesystem;
class Index {
    public:
        Index(const std::string& mgit_path);
        void add() const;
        
    private:
        const std::string& mgit_path_;
        json read_index() const;
        void write_to_index(json file_info) const;
        fs::path index_path_;

};


#endif