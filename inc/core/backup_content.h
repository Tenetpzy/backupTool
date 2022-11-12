#pragma once

#include "core/file_common.h"
#include "core/backup_config.h"
#include <vector>
#include <unordered_map>

class resource_manager;
class backup_content;

class backup_entry
{
public:
    backup_entry(const file_descriptor &fd) : _fd(fd) {}

    const file_descriptor& get_file_descriptor() const noexcept
    {
        return _fd;
    }

    const std::vector<backup_entry>& get_sub_entrys() const noexcept
    {
        return sub_entrys;
    }


private:
    file_descriptor _fd;
    std::vector<backup_entry> sub_entrys;


    void add_sub_entry(const backup_entry &entry)
    {
        sub_entrys.push_back(entry);
    }

    void add_sub_entry(backup_entry &&entry)
    {
        sub_entrys.push_back(std::move(entry));
    }

    friend class backup_content;
};

/*
功能：维护一个备份目录项（一个目录或文件）的信息，包括url，自定义配置，备份选项
*/
class backup_content
{
public:
    backup_content(const std::string &url)
        : _url(url) {}
    backup_content(std::string &&url)
        : _url(std::move(url)) {}

    const std::string& get_url() const { return _url; }


private:
    std::string _url;
    std::unordered_map<backup_config_type, backup_config> backup_configs;
    std::unordered_map<custom_filter_type, custom_filter> custom_filters;
};