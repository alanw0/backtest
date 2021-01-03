#ifndef bt_cmdline_parser_hpp
#define bt_cmdline_parser_hpp

#include <string>

namespace bt {

std::string concatenate(char** argv, int argc);

std::string get_param(const std::string& argstr,
                      const std::string& flag,
                      const std::string& defaultValue);
float get_param(const std::string& argstr,
                const std::string& flag,
                float defaultValue);
unsigned get_param(const std::string& argstr,
                const std::string& flag,
                unsigned defaultValue);
}// namespace bt

#endif
