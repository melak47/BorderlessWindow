cmake_minimum_required(VERSION 3.15)

project(BorderlessWindow)

# WIN32 for a /subsystem:windows program...
add_executable(BorderlessWindow WIN32
    src/main.cpp
    src/BorderlessWindow.cpp
)

# but with 'main' entry point
target_link_options(BorderlessWindow PRIVATE /entry:mainCRTStartup)
target_link_libraries(BorderlessWindow PRIVATE dwmapi)
target_compile_options(BorderlessWindow PRIVATE /diagnostics:caret /permissive- /W4)
target_compile_definitions(BorderlessWindow PRIVATE UNICODE _UNICODE NOMINMAX)
