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

    constexpr auto LOG_PATH{ "gm2.log" };
    constexpr auto LOGGER_NAME{ "gm2" };
    constexpr auto RATE_LIMIT_INTERVAL{ 1s };
    constexpr usize RATE_LIMIT_COUNT{ 3 };

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
            usize dropped;
        };

        std::shared_ptr<sink> _sink;
        std::unordered_map<Location, Record, Hash> _records;

    public:
        explicit RateLimitSink(const std::shared_ptr<sink>& sink)
            : _sink{ sink } {}

        void log(const spdlog::details::log_msg& message) override {
            auto now{ std::chrono::steady_clock::now() };
            std::string filename{ message.source.filename == nullptr ? "" : gm::filename(message.source.filename) };
            Record& record{ _records.try_emplace({ filename, message.source.line }, now).first->second };
            if (now - record.start >= RATE_LIMIT_INTERVAL) {
                if (record.dropped != 0) {
                    std::string payload{ std::format("Dropped {} messages since the last log.", record.dropped) };
                    auto dropped_message{ message };
                    dropped_message.level = spdlog::level::warn;
                    dropped_message.payload = payload;
                    _sink->log(dropped_message);
                }

                record = { now };
            }

            if (record.count >= RATE_LIMIT_COUNT) {
                ++record.dropped;
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
