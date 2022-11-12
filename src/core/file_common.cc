#include "core/file_common.h"

const std::any& file_metadata::get_metadata_entry(const std::string &entry_name) const
{
    auto res = _metadata.find(entry_name);
    if (res == _metadata.end())
        throw no_metadata_error(entry_name);
    return *res;
}

inline void file_metadata::set_metadata_entry(const std::string &entry_name, const std::any &value)
{
    _metadata[entry_name] = value;
}

