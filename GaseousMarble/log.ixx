module;

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>

export module gm.log;

import std;

namespace gm {

    static constexpr auto LOG_PATH{ "gm2.log" };

    export spdlog::logger& logger() noexcept {
        static auto value{ [] noexcept -> spdlog::logger {
            try {
                return { "gm2", std::make_shared<spdlog::sinks::basic_file_sink_st>(LOG_PATH) };
            } catch (const std::exception&) {
                return { "gm2", std::make_shared<spdlog::sinks::null_sink_st>() };
            }
        }() };
        return value;
    }

}