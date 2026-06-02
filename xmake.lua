add_requires("raylib 5.5", {debug = is_mode("debug")})
add_requires("entt v3.15.0")
add_requires("glm 1.0.1")
add_requires("spdlog v1.16.0")
add_requires("fmt 12.1.0")

includes("../EngineSquared/xmake.lua")

set_project("Aot")
set_languages("c++20")

add_rules("mode.debug", "mode.release")

-- add /W4 for windows
if is_plat("windows") then
    add_cxflags("/W4")
end

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
target("Aot")
    set_kind("binary")

    add_deps("EngineSquared")

    add_files("src/**.cpp")

    add_includedirs("$(projectdir)/src/", {public = true})

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng", "rmlui", "miniaudio", "raylib")

    set_rundir("$(projectdir)")
    set_pcxxheader("src/Raylib.pch.hpp")

    if is_mode("debug") then
        add_defines("ES_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    end

    if is_mode("release") then
        set_optimize("fastest")
    end
target_end()

target("Aot_Tests")
    for _, file in ipairs(os.files("tests/**.cpp")) do
        local name = path.basename(file)
        if name == "main" then
            goto continue
        end
        target(name)
            set_group(TEST_GROUP_NAME)
            set_default(false)
            set_kind("binary")
            if is_plat("linux") then
                add_cxxflags("--coverage", "-fprofile-arcs", "-ftest-coverage", {force = true})
                add_ldflags("--coverage")
            end
            set_languages("cxx20")
            add_packages("gtest", "entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng", "rmlui", "miniaudio", "raylib")

            add_links("gtest")
            add_tests("default")

            add_deps("Aot")

            add_files(file)
            add_files("tests/main.cpp")
            if is_mode("debug") then
                add_defines("DEBUG")
            end
        ::continue::
    end
target_end()