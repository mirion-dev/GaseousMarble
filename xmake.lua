add_rules("mode.debug", "mode.release")

set_languages("c++latest")
set_arch("x86")
set_plat("windows")
set_warnings("all", "error")

add_requires("vcpkg::doctest", {debug = is_mode("debug"), configs = {shared = true}})
add_requires("vcpkg::eigen3", "vcpkg::glaze", "vcpkg::spdlog", "vcpkg::wil", {debug = is_mode("debug"), configs = {runtimes = "MD"}})
add_linkdirs("third_party/dx8.1b/lib")
on_config(function (target)
    target:add("sysincludedirs", path.splitenv(target:toolchain("msvc"):runenvs().INCLUDE))
    target:add("sysincludedirs", "third_party/dx8.1b/include", "third_party/rectpack2d/src")
end)

add_defines("GASEOUSMARBLE_EXPORTS", "_UNICODE", "NOMINMAX", "UNICODE")
add_defines(is_mode("debug") and "_DEBUG" or "NDEBUG")
add_syslinks("d3d8", "dwrite", "ole32", "oleaut32")

add_cxxflags("/diagnostics:caret", "/sdl", "/Zc:__cplusplus,checkGwOdr,enumTypes,inline,preprocessor,templateScope,throwingNew", {tools = "cl"})

if is_mode("debug") then
    add_cxxflags("/JMC", "/RTC1", {tools = "cl", force = true})
end

if is_mode("release") then
    add_cxxflags("/Ob3", {tools = "cl"})
    set_symbols("debug")
    set_policy("build.optimization.lto", true)
end

target("gm2_core")
    set_kind("static")
    add_files("src/*.ixx", {public = true})
    add_packages("vcpkg::eigen3", "vcpkg::glaze", "vcpkg::spdlog", "vcpkg::wil", {public = true})

target("gm2")
    set_kind("shared")
    add_files("src/*.cpp")
    add_deps("gm2_core")

    set_targetdir(is_mode("debug") and "build/debug" or "build/release")
    on_run(function (target)
        os.execv("projects/test.gm82/test.exe", {path.absolute(target:targetfile())}, {curdir = "projects/test.gm82"})
    end)

target("gm2_test")
    set_kind("binary")
    add_files("tests/*.cpp")
    add_deps("gm2_core")
    add_packages("vcpkg::doctest")
