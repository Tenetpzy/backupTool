#pragma once

#include "core/resource_manager.h"
#include "utils/lru_counter.h"
#include <stdexcept>
#include <unordered_set>
#include <string>


// class local_fd_guard
// {
// public:
//     local_fd_guard() = default;
//     local_fd_guard(const local_fd_guard &rhs) = delete;
//     local_fd_guard(local_fd_guard &&rhs) = delete;
//     local_fd_guard& operator=(const local_fd_guard &rhs) = delete;
//     local_fd_guard& operator=(local_fd_guard &&rhs) = delete;
//     ~local_fd_guard();

//     void add_fd(int fd);
//     void remove_fd(int fd);

// private:
//     std::unordered_set<int> fd_set;
// };

class local_resource_manager : public resource_manager
{
public:
    size_t read_file(const std::string &file_url, void *buf, off_t pos, size_t count) override;
    size_t write_file(const std::string &file_url, void *buf, off_t pos, size_t count) override;
    void truncate(const std::string &file_url, size_t length) override;
    std::vector<file_descriptor> list_dir(const std::string &dir_url) override;
    void create_file(const std::string &new_url) override;
    void create_dir(const std::string &new_url) override;
    void remove(const std::string &url) override;
    file_metadata get_metadata(const std::string &url) override;

    local_resource_manager();

    using error_type = resource_manager_error::error_type;

private:

    class fd_cache
    {
    private:
        std::unordered_map<std::string, int> url_fd_map;
        lru_counter<int> fd_lru;
        int _open_flag;
        size_t cache_size, current_size;

    public:
        fd_cache(int open_flag, size_t size) : _open_flag(open_flag), cache_size(size), current_size(0) {} 
        ~fd_cache() noexcept;
        int get_fd(const std::string &url);
    
    } rfd_cache, wfd_cache;
};


class resource_manager_error
{
public:
    enum class error_type{
        open, close, read, write, truncate, open_dir, close_dir, read_dir, get_stat, create, remove
    };

    resource_manager_error(error_type error_t, const std::string &err_msg)
        : err(error_t),  msg(err_msg) {}
    
    resource_manager_error(error_type error_t, std::string &&err_msg)
        : err(error_t),  msg(std::move(err_msg)) {}

    resource_manager_error(error_type error_t)
        : err(error_t) {}

    error_type get_error_type() const { return err; }
    const std::string& get_error_msg() const { return msg; }

private:
    error_type err;
    std::string msg;
};

struct stat;
class local_file_helper
{
public:
    static file_type get_file_type(const struct stat *_stat);
};