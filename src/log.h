#pragma once

#define GM_INFO(...) SPDLOG_LOGGER_INFO(gm::logger(), __VA_ARGS__)
#define GM_WARN(...) SPDLOG_LOGGER_WARN(gm::logger(), __VA_ARGS__)
#define GM_ERROR(...) SPDLOG_LOGGER_ERROR(gm::logger(), __VA_ARGS__)
#define GM_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(gm::logger(), __VA_ARGS__)

#define GM_WARN_IF_FAILED(result)                                                                                      \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            GM_WARN("Error {:#x}.", static_cast<u32>(value));                                                          \
        }                                                                                                              \
    } while (false)

#define GM_RETURN_IF_FAILED(result)                                                                                    \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            GM_ERROR("Error {:#x}.", static_cast<u32>(value));                                                         \
            RETURN_HR(value);                                                                                          \
        }                                                                                                              \
    } while (false)

#define GM_THROW_IF_FAILED(result)                                                                                     \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            GM_ERROR("Error {:#x}.", static_cast<u32>(value));                                                         \
            THROW_HR(value);                                                                                           \
        }                                                                                                              \
    } while (false)

#define GM_ABORT_IF_FAILED(result)                                                                                     \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            GM_CRITICAL("Error {:#x}.", static_cast<u32>(value));                                                      \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (false)

#define GM_WARN_IF_FAILED_WIN32_BOOL(result) GM_WARN_IF_FAILED(gm::win32_bool_to_hr(result))
#define GM_RETURN_IF_FAILED_WIN32_BOOL(result) GM_RETURN_IF_FAILED(gm::win32_bool_to_hr(result))
#define GM_THROW_IF_FAILED_WIN32_BOOL(result) GM_THROW_IF_FAILED(gm::win32_bool_to_hr(result))
#define GM_ABORT_IF_FAILED_WIN32_BOOL(result) GM_ABORT_IF_FAILED(gm::win32_bool_to_hr(result))

#define GM_CATCH_RETURN()                                                                                              \
    catch (const wil::ResultException& error) {                                                                        \
        const wil::FailureInfo& info{ error.GetFailureInfo() };                                                        \
        GM_ERROR("-> [{}:{}] Error {:#x}.", gm::filename(info.pszFile), info.uLineNumber, static_cast<u32>(info.hr));  \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }                                                                                                                  \
    catch (const std::exception& error) {                                                                              \
        GM_ERROR("{}.", error.what());                                                                                 \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        GM_ERROR("Unknown exception.");                                                                                \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }
