add_rules("mode.debug", "mode.release")

set_languages("c++latest")
set_arch("x86")
set_plat("windows")
set_warnings("all", "error")

add_requires("vcpkg::glaze", "vcpkg::spdlog", "vcpkg::wil", {debug = is_mode("debug"), configs = {shared = false, runtimes = "MD"}})
add_linkdirs("third_party/dx81/lib")
on_config(function (target)
    target:add("sysincludedirs", path.splitenv(target:toolchain("msvc"):runenvs().INCLUDE))
    target:add("sysincludedirs", "third_party/dx81/include", "third_party/rectpack2d/src")
end)

add_defines("NOMINMAX", "UNICODE", "_UNICODE", "GASEOUSMARBLE_EXPORTS")
add_syslinks("d3d8", "dwrite", "ole32", "oleaut32")

add_cxxflags("/JMC", "/sdl", "/permissive-", "/Zc:preprocessor", {tools = "cl", force = true})
add_shflags("/SUBSYSTEM:WINDOWS", "/SAFESEH:NO", {tools = "link"})

if is_mode("release") then
    set_symbols("debug")
    set_policy("build.optimization.lto", true)
    add_cxxflags("/Oi", "/Gy", {tools = "cl"})
end

target("gm2")
    set_kind("shared")
    add_files("src/*.cpp", "src/*.ixx")
    add_packages("vcpkg::glaze", "vcpkg::spdlog", "vcpkg::wil")

    set_targetdir(is_mode("debug") and "build/debug" or "build/release")
    on_run(function (target)
        os.execv("projects/test.gm82/test.exe", {path.absolute(target:targetfile())}, {curdir = "projects/test.gm82"})
    end)
