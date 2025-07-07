/** 程序的入口位置
 * 初始化仓库，加载配置文件 */
#ifndef __REPOSITORY_H__
#define __REPOSITORY_H__

#include <string>


class Repository {
    public:
        Repository();
        void init_repository(const std::string& project_path);
        inline const std::string& get_project_path() {
            return project_path_;
        }

    private:
        std::string project_path_;
};

#endif