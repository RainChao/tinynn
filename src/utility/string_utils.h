#pragma once
#include <string>
#include <type_traits>
#include <fcntl.h>
#include <sys/syscall.h>

namespace tinynn {
class StringUtils {
public:
    static bool ends_with(const ::std::string& str,
                          const ::std::string& suffix) {
        return str.size() >= suffix.size() and
               strncmp(str.c_str() + (str.size() - suffix.size()), suffix.c_str(), suffix.size()) == 0;
    }

    template <typename ...Args>
    static ::std::string string_format(const ::std::string& format,
                                       Args&&... args) {
        ::std::string format_str;
        int len = snprintf(nullptr, 0, format.c_str(), ::std::forward<Args>(args)...);
        format_str.resize(len+1);
        int newsize = snprintf(&format_str[0], len+1, format.c_str(), ::std::forward<Args>(args)...);
        format_str.resize(newsize);
        return format_str;
    }
};
}
