set_languages("c++20")

add_rules("mode.debug", "mode.release", "mode.coverage")
add_requires("doctest")

add_includedirs("include", "../lds-gen-cpp/include")

if is_mode("coverage") then
    add_cxflags("-ftest-coverage", "-fprofile-arcs", {force = true})
end

if is_plat("linux") then
    set_warnings("all", "error")
    add_cxflags("-Wconversion", {force = true})
    -- Termux/Android ships its libc++ headers under $PREFIX
    local prefix = os.getenv("PREFIX")
    if prefix then
        add_cxflags("-Wno-unused-command-line-argument", {force = true})
        add_sysincludedirs(prefix .. "/include/c++/v1", prefix .. "/include", {public = true})
    end
elseif is_plat("windows") then
    add_cxflags("/EHsc /utf-8 /W4 /WX /wd4702", {force = true})
end

target("SphereN")
    set_kind("static")
    add_files("source/*.cpp")
    -- sphere_n lives in the sibling lds-gen-cpp repo; compile its source here to
    -- provide the ldsgen::Sphere3/SphereN definitions without a local copy
    add_files("../lds-gen-cpp/source/sphere_n.cpp")
    add_packages("doctest")

target("test_sphere_n")
    add_deps("SphereN")
    add_files("test/source/*.cpp")
    add_packages("doctest")
    add_tests("default")
