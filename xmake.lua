add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

set_toolchains("msvc")
add_requires("spdlog 1.14.1", {configs = {shared = false}})
add_requires("argparse", "fmt 10.2.1", "cpp-ipc", {configs = {shared = true}})
add_requires("serial",  {configs = {shared = true}})

set_license("GPL")

target("avs-operator")
    set_kind("binary")
    set_languages("c++17")
    add_files("src/*.cpp")
    add_linkdirs("src/lib")
    add_links("avaspecx64")
    
    add_cxflags("/utf-8", "/wd4819")
    add_packages("argparse", "spdlog", "fmt", "cpp-ipc", "serial")
