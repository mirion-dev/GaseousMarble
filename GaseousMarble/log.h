#pragma once

#define GM_WARN(...) SPDLOG_LOGGER_WARN(gm::logger(), __VA_ARGS__)
#define GM_ERROR(...) SPDLOG_LOGGER_ERROR(gm::logger(), __VA_ARGS__)

#define GM_WARN_IF_FAILED(result)                                                                                      \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            GM_WARN("Error {:#x}.", static_cast<std::make_unsigned_t<HRESULT>>(value));                                \
        }                                                                                                              \
    } while (false)

#define GM_THROW_IF_FAILED(result)                                                                                     \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            GM_ERROR("Error {:#x}.", static_cast<std::make_unsigned_t<HRESULT>>(value));                               \
            THROW_HR(value);                                                                                           \
        }                                                                                                              \
    } while (false)

#define GM_RETURN_IF_FAILED(result)                                                                                    \
    do {                                                                                                               \
        HRESULT value{ result };                                                                                       \
        if (value < 0) {                                                                                               \
            GM_ERROR("Error {:#x}.", static_cast<std::make_unsigned_t<HRESULT>>(value));                               \
            RETURN_HR(value);                                                                                          \
        }                                                                                                              \
    } while (false)

#define GM_CATCH_RETURN()                                                                                              \
    catch (const wil::ResultException& error) {                                                                        \
        const wil::FailureInfo& info{ error.GetFailureInfo() };                                                        \
        GM_ERROR(                                                                                                      \
            "-> [{}:{}:{}] Error {:#x}.",                                                                              \
            std::filesystem::path{ info.pszFile }.filename().string(),                                                 \
            info.uLineNumber,                                                                                          \
            info.pszFunction,                                                                                          \
            static_cast<std::make_unsigned_t<HRESULT>>(info.hr)                                                        \
        );                                                                                                             \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }                                                                                                                  \
    catch (const std::exception& error) {                                                                              \
        GM_ERROR("{}", error.what());                                                                                  \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        GM_ERROR("Unknown exception.");                                                                                \
        RETURN_CAUGHT_EXCEPTION();                                                                                     \
    }
