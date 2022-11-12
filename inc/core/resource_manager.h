#pragma once

#include "core/file_common.h"
#include <vector>

/*
 * 文件资源管理器
 * 提供对备份源、备份目标（统称备份节点）的文件和目录操作接口
 * 备份节点上的文件资源以文件url的方式提供。
 * 文件url：/file/path/name，始终以'/'开头
 */
class resource_manager
{
public:
    /*
    读取url代表文件的[pos, pos + count)的内容到buf中
    返回实际读取的字节数，若pos越过末尾，则返回0。
    */
    virtual size_t read_file(const std::string &file_url, void *buf, off_t pos, size_t count) = 0;

    /*
    将[buf, buf + count)写入url代表文件的[pos, pos + count)位置
    返回实际写入的字节数。
    */
    virtual size_t write_file(const std::string &file_url, void *buf, off_t pos, size_t count) = 0;

    // 将文件大小调整为length字节
    virtual void truncate(const std::string &file_url, size_t length) = 0;

    virtual std::vector<file_descriptor> list_dir(const std::string &dir_url) = 0;

    // 说明：由备份软件在备份节点上代理创建文件，新文件权限位不交由用户控制
    virtual void create_file(const std::string &new_url) = 0;
    
    virtual void create_dir(const std::string &new_url) = 0;

    virtual void remove(const std::string &url) = 0;

    virtual file_metadata get_metadata(const std::string &url) = 0;

    virtual ~resource_manager() = default;
};