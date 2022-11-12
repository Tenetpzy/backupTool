#pragma once

#include <string>
#include <unordered_map>
#include <any>
#include <stdexcept>

enum class file_type
{
    dir_t,
    generic_file_t,
    fifo_t,
    symlink_t,
    unsupport_t
};

class file_descriptor
{
public:
    file_descriptor(file_type type, const std::string &name)
        : _type(type), _name(name) {}
    file_descriptor(file_type type, std::string &&name)
        : _type(type), _name(name) {}

    file_type get_file_type() const noexcept
    {
        return _type;
    }

    const std::string& get_file_name() const noexcept
    {
        return _name;
    }

private:
    file_type _type;
    std::string _name;
};

class no_metadata_error : public std::logic_error
{
public:
    explicit no_metadata_error(const std::string &msg) : std::logic_error(msg) {}
    // const char* what() const noexcept override;
};

class file_metadata
{
public:
    const std::any& get_metadata_entry(const std::string &entry_name) const;
    void set_metadata_entry(const std::string &entry_name, const std::any &value);

private:
    std::unordered_map<std::string, std::any> _metadata;
};