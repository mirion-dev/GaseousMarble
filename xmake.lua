set_plat("windows")
set_arch("x86")
set_languages("c++latest")

add_rules("mode.debug", "mode.release")

package("rectpack2d")
do
	on_load(function(package)
		package:set("installdir", "third_party/rectpack2d")
	end)

	on_fetch(function(package)
		return { includedirs = package:installdir("src") }
	end)
end
package_end()

add_requires("rectpack2d")
add_requires(
	"vcpkg::doctest",
	"vcpkg::eigen3",
	"vcpkg::glaze",
	"vcpkg::wil",
	{ debug = is_mode("debug"), configs = { shared = true } }
)
add_requires("vcpkg::spdlog", { debug = is_mode("debug"), configs = { runtimes = "MD" } })

on_config(function(target)
	target:add("sysincludedirs", path.splitenv(target:toolchain("msvc"):runenvs().INCLUDE))
	target:add("sysincludedirs", "third_party/dx8.1b/include")
end)
add_linkdirs("third_party/dx8.1b/lib")

add_defines("_UNICODE", "NOMINMAX", "UNICODE")
add_defines(is_mode("debug") and "_DEBUG" or "NDEBUG")

set_warnings("all", "error")
if is_mode("release") then
	set_symbols("debug")
	set_policy("build.optimization.lto", true)
end

add_cxxflags(
	"/diagnostics:caret",
	"/sdl",
	"/Zc:__cplusplus,checkGwOdr,cmath,enumTypes,inline,preprocessor,templateScope,throwingNew",
	{ tools = "cl" }
)
if is_mode("debug") then
	add_cxxflags("/JMC", "/RTC1", { tools = "cl", force = true })
else
	add_cxxflags("/Ob3", { tools = "cl" })
end

target("gm2_core")
do
	set_kind("static")
	add_files("src/*.ixx", { public = true })

	add_packages("rectpack2d", "vcpkg::eigen3", "vcpkg::glaze", "vcpkg::spdlog", "vcpkg::wil", { public = true })
	add_syslinks("d3d8", "dwrite", "ole32", "oleaut32", { public = true })
end

target("gm2")
do
	set_kind("shared")
	add_files("src/*.cpp")

	add_deps("gm2_core")
	add_defines("GASEOUSMARBLE_EXPORTS")

	set_targetdir(is_mode("debug") and "build/debug" or "build/release")
	on_run(function(target)
		import("private.action.run.runenvs")

		local addenvs, setenvs = runenvs.make(target)
		os.addenv("PATH", path.absolute(target:targetdir()))
		os.execv(
			"projects/test.gm82/test.exe",
			{},
			{ curdir = "projects/test.gm82", addenvs = addenvs, setenvs = setenvs }
		)
	end)
end

target("gm2_test")
do
	set_default(os.getenv("XMAKE_IN_PROJECT_GENERATOR") == "true")
	set_kind("binary")
	add_files("tests/*.cpp")

	add_deps("gm2_core")
	add_packages("vcpkg::doctest")
end
