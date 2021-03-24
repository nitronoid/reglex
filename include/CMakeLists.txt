add_library(reglex INTERFACE)
target_include_directories(reglex INTERFACE .)
target_link_libraries(
    reglex
    INTERFACE
        project_options
        project_warnings
        CONAN_PKG::magic_enum)
