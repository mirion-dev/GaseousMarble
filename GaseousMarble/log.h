#pragma once

#define GM_THROW_IF_FAILED(result)                                                                                     \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            SPDLOG_LOGGER_ERROR(                                                                                       \
                gm::logger(), "{}(): Error {:#x}.", __func__, static_cast<std::make_unsigned_t<HRESULT>>(value)        \
            );                                                                                                         \
            THROW_HR(value);                                                                                           \
        }                                                                                                              \
    } while (false)

#define GM_RETURN_IF_FAILED(result)                                                                                    \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            SPDLOG_LOGGER_ERROR(                                                                                       \
                gm::logger(), "{}(): Error {:#x}.", __func__, static_cast<std::make_unsigned_t<HRESULT>>(value)        \
            );                                                                                                         \
            RETURN_HR(value);                                                                                          \
        }                                                                                                              \
    } while (false)

#define GM_CATCH_RETURN()                                                                                              \
    catch (const wil::ResultException& error) {                                                                        \
        const wil::FailureInfo& info{ error.GetFailureInfo() };                                                        \
        SPDLOG_LOGGER_ERROR(                                                                                           \
            gm::logger(),                                                                                              \
            "{}() -> [{}:{}] {}(): Error {:#x}.",                                                                      \
            __func__,                                                                                                  \
            std::filesystem::path{ info.pszFile }.filename().string(),                                                 \
            info.uLineNumber,                                                                                          \
            info.pszFunction,                                                                                          \
            static_cast<std::make_unsigned_t<HRESULT>>(info.hr)                                                        \
        );                                                                                                             \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }                                                                                                                  \
    catch (const std::exception& error) {                                                                              \
        SPDLOG_LOGGER_ERROR(gm::logger(), "{}", error.what());                                                         \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        SPDLOG_LOGGER_ERROR(gm::logger(), "Unknown exception.");                                                       \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }