#pragma once

#include <any>

// 自定义备份过滤器类型
enum class custom_filter_type
{
    path_filter,
    name_filter,
    type_filter,
    time_filter
};

// 自定义备份过滤器描述
class custom_filter
{
public:
    custom_filter_type get_filter_type() const { return _type; }
    const std::any& get_filter_info() const { return _info; }

    friend class custom_filter_factory;

private:
    custom_filter(custom_filter_type type, const std::any &info)
        : _type(type), _info(info) {}
    custom_filter(custom_filter_type type, std::any &&info)
        : _type(type), _info(std::move(info)) {}

    custom_filter_type _type;
    std::any _info;
};

// 备份配置项类型
// 目前可配置项包括校验，压缩，加密，实时
enum class backup_config_type
{
    check,
    compress,
    encrypt,
    realtime
};

// 备份配置项描述
class backup_config
{
public:
    backup_config_type get_config_type() { return _type; }
    const std::any& get_config_info() const { return _info; }

    friend class backup_config_factory;

private:

    backup_config(backup_config_type type, const std::any &info)
        : _type(type), _info(info) {}
    backup_config(backup_config_type type, std::any &&info)
        : _type(type), _info(std::move(info)) {}

    backup_config_type _type;
    std::any _info;
};