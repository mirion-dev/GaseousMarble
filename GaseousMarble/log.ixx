module;

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>

export module gm.log;

import std;
import gm.types;
import gm.utils;

using namespace std::literals;

namespace gm {

    static constexpr auto LOG_PATH{ "gm2.log" };
    static constexpr auto LOGGER_NAME{ "gm2" };
    static constexpr auto RATE_LIMIT_INTERVAL{ 1s };
    static constexpr usize RATE_LIMIT_COUNT{ 10 };

    class RateLimitSink : public spdlog::sinks::sink {
        struct Location {
            std::string filename;
            int line;

            bool operator==(const Location&) const noexcept = default;
        };

        struct Hash {
            usize operator()(const Location& location) const noexcept {
                return hash_combine(gm::Hash{}, location.filename, location.line);
            }
        };

        struct Record {
            std::chrono::steady_clock::time_point start;
            usize count;
        };

        std::shared_ptr<sink> _sink;
        std::unordered_map<Location, Record, Hash> _records;

    public:
        explicit RateLimitSink(std::shared_ptr<sink> sink)
            : _sink{ sink } {}

        void log(const spdlog::details::log_msg& message) override {
            auto now{ std::chrono::steady_clock::now() };
            std::string filename{ message.source.filename == nullptr ? "" : gm::filename(message.source.filename) };
            Record& record{ _records.try_emplace({ filename, message.source.line }, now, 0).first->second };
            if (now - record.start >= RATE_LIMIT_INTERVAL) {
                record = { now, 0 };
            }

            if (record.count >= RATE_LIMIT_COUNT) {
                return;
            }

            ++record.count;
            _sink->log(message);
        }

        void flush() override {
            _sink->flush();
        }

        void set_pattern(const std::string& pattern) override {
            _sink->set_pattern(pattern);
        }

        void set_formatter(std::unique_ptr<spdlog::formatter> formatter) override {
            _sink->set_formatter(std::move(formatter));
        }
    };

    export spdlog::logger* logger() noexcept {
        static auto value{ [] noexcept -> spdlog::logger {
            try {
                spdlog::logger result{
                    LOGGER_NAME,
                    std::make_shared<RateLimitSink>(std::make_shared<spdlog::sinks::basic_file_sink_st>(LOG_PATH, true))
                };
                result.set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#:%!] %v");
                return result;
            } catch (const std::exception&) {
                return { LOGGER_NAME, std::make_shared<spdlog::sinks::null_sink_st>() };
            }
        }() };
        return &value;
    }

}