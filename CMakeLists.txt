cmake_minimum_required(VERSION 3.10) 


project(consoleApp)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_SOURCE_DIR}/bin)
endforeach()

add_executable(${PROJECT_NAME}
        main.cpp
        units.hpp
        utils.hpp
        network.hpp
        network.cpp
        console.hpp
        console.cpp
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8 /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -finput-charset=UTF-8
            -fexec-charset=UTF-8
            -Wall
            -Wextra
            -Wpedantic
    )
endif()