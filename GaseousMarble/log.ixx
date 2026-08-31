module;

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>

export module gm.log;

import std;

namespace gm {

    static constexpr auto LOG_PATH{ "gm2.log" };
    static constexpr auto LOGGER_NAME{ "gm2" };

    export spdlog::logger* logger() noexcept {
        static auto value{ [] noexcept -> spdlog::logger {
            try {
                spdlog::logger result{ LOGGER_NAME, std::make_shared<spdlog::sinks::basic_file_sink_st>(LOG_PATH) };
                result.set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
                return result;
            } catch (const std::exception&) {
                return { LOGGER_NAME, std::make_shared<spdlog::sinks::null_sink_st>() };
            }
        }() };
        return &value;
    }

}