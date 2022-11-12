#include "core/local_resource_manager.h"
#include "utils/log_recorder.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <dirent.h>

int local_resource_manager::fd_cache::get_fd(const std::string &url)
{
    int fd;
    auto res = url_fd_map.find(url);
    if (res == url_fd_map.end())
    {
        fd = open(url.c_str(), _open_flag);
        if (fd == -1)
            throw resource_manager_error(error_type::open, std::string("error opening ") + url);

        if (current_size == cache_size)
        {
            int victim_fd = fd_lru.remove_less_use_entry();
            if (close(victim_fd) == -1)
                throw resource_manager_error(error_type::close, strerror(errno));
        }
        else
            ++current_size;
        
        fd_lru.add_element(fd);
    }
    else
        fd = res->second;
    
    fd_lru.add_element_count(fd);
    return fd;
}

local_resource_manager::fd_cache::~fd_cache() noexcept
{
    for (auto &[url, fd] : url_fd_map)
        if (close(fd) == -1)
            log_helper::print_log_to_cerr(strerror(errno));
}

local_resource_manager::local_resource_manager()
    : rfd_cache(O_RDONLY, 5), wfd_cache(O_WRONLY, 5) {}

size_t local_resource_manager::read_file(const std::string &file_url, void *buf, off_t pos, size_t count)
{
    int fd = rfd_cache.get_fd(file_url);

    // 注：程序应忽略作业控制信号，防止信号中断系统调用执行
    int res = pread(fd, buf, count, pos);
    if (res == -1)
        throw resource_manager_error(error_type::read, std::string("error reading ") + file_url);
    return res;
}

size_t local_resource_manager::write_file(const std::string &file_url, void *buf, off_t pos, size_t count)
{
    int fd = wfd_cache.get_fd(file_url);
    int res = pwrite(fd, buf, count, pos);
    if (res == -1)
        throw resource_manager_error(error_type::write, std::string("error writing ") + file_url);
    return res;
}

void local_resource_manager::truncate(const std::string &file_url, size_t length)
{
    if (::truncate(file_url.c_str(), (off_t)length) == -1)
        throw resource_manager_error(error_type::truncate, std::string("error truncating ") + file_url);
}


class dirp_guard
{
public:
    dirp_guard(DIR *dirp, const std::string &url)
        : _dirp(dirp), dir_url(url) {}

    ~dirp_guard() noexcept
    {
        if (_dirp != NULL)
        {
            if (closedir(_dirp) == -1)
                log_helper::print_log_to_cerr(std::string("error truncating ") + dir_url);
        }
    }

    operator DIR*() const
    {
        return _dirp;
    }

private:
    DIR *_dirp;
    std::string dir_url;
};

std::vector<file_descriptor> local_resource_manager::list_dir(const std::string &dir_url)
{
    dirp_guard dirp(opendir(dir_url.c_str()), dir_url);
    if (dirp == NULL)
        throw resource_manager_error(error_type::open_dir, std::string("error opening dir ") + dir_url);
    std::vector<file_descriptor> res;
    for (;;)
    {
        errno = 0;  // 从readdir返回NULL时，分辨出不是到达end of dir的错误
        struct dirent *dp = readdir(dirp);
        if (dp == NULL)
        {
            if (!errno)
                throw resource_manager_error(error_type::read_dir, std::string("error reading dir ") + dir_url); 
            else
                break;
        }

        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        struct stat stat_buf;
        std::string dentry_full_url = dir_url + '/' + std::string(dp->d_name);

        // 对符号链接，获取它自身的信息而非指向的文件信息
        if (lstat(dentry_full_url.c_str(), &stat_buf) == -1)
            throw resource_manager_error(error_type::get_stat, std::string("error getting stat of ") + dentry_full_url); 

        file_type type = local_file_helper::get_file_type(&stat_buf);
        if (type == file_type::unsupport_t)
            throw resource_manager_error(error_type::get_stat, std::string("unsupport file type: ") + dentry_full_url); 

        res.emplace_back(type, dp->d_name);
    }

    return res;
}

void local_resource_manager::create_file(const std::string &new_url)
{     
    auto throw_error = [&new_url]()
    {
        std::string errstr = std::string("error creating ") + new_url + '\n' + strerror(errno);
        throw resource_manager_error(error_type::create, std::move(errstr));
    };
    int fd = open(new_url.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH);

    if (fd == -1)
        throw_error();
    else
    {
        if (close(fd) == -1)
            throw_error();
    }
}

void local_resource_manager::create_dir(const std::string &new_url)
{
    auto throw_error = [&new_url]()
    {
        std::string errstr = std::string("error creating ") + new_url + '\n' + strerror(errno);
        throw resource_manager_error(error_type::create, std::move(errstr));
    };
    int fd = mkdir(new_url.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH);
    if (fd == -1)
        throw_error();
    else
    {
        if (close(fd) == -1)
            throw_error();
    }
}

void local_resource_manager::remove(const std::string &url)
{
    if (::remove(url.c_str()) == -1)
        throw resource_manager_error(error_type::remove, std::string("error removing ") + url + '\n' + strerror(errno));
}

file_metadata local_resource_manager::get_metadata(const std::string &url)
{
    file_metadata meta;
    struct stat stat_buf;

    if (lstat(url.c_str(), &stat_buf) == -1)
        throw resource_manager_error(error_type::get_stat, std::string("error getting stat of ") + url); 

    
}

file_type local_file_helper::get_file_type(const struct stat *_stat)
{
    file_type res;
    switch (_stat->st_mode & S_IFMT)
    {
    case S_IFREG:
        res = file_type::generic_file_t;
        break;
    
    case S_IFDIR:
        res = file_type::dir_t;
        break;
    
    case S_IFIFO:
        res = file_type::fifo_t;
        break;
    
    case S_IFLNK:
        res = file_type::symlink_t;
        break;

    default:
        res = file_type::unsupport_t;
        break;
    }
    return res;
}