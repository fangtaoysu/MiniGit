/** 程序的入口位置
 * 初始化仓库，加载配置文件 */
#ifndef __REPOSITORY_H__
#define __REPOSITORY_H__
#include <unordered_map>
#include <string>
#include "remote.h"


class Repository {
    public:
        Repository();
        void init_repository();
        inline const std::string& get_project_path() {
            return project_path_;
        }
        void config(const std::unordered_map<std::string, std::string>& info);

    private:
        std::string project_path_;
};

#endif