cmake_minimum_required(VERSION 3.10)
project(dungeon_crawler VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(USE_NIX "Use Nix-provided dependencies instead of FetchContent" OFF)

if(USE_NIX)
    find_package(raylib 5.5 REQUIRED)
else()
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5.0
        GIT_SHALLOW TRUE
    )
    FetchContent_Declare(
        raytmx
        GIT_REPOSITORY https://github.com/luphi/raytmx.git
        GIT_TAG d4e09bc
    )
    FetchContent_Declare(
        hoxml
        GIT_REPOSITORY https://github.com/luphi/hoxml.git
        GIT_TAG 12938da
    )

    set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(RAYLIB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(RAYLIB_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(HOXML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(raylib raytmx hoxml)
endif()

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
list(FILTER HEADERS EXCLUDE REGEX "raylib|raymath|rlgl|raytmx|hoxml")

add_executable(dungeon_game ${SOURCES} ${HEADERS})

target_include_directories(dungeon_game PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(USE_NIX)
    target_link_libraries(dungeon_game PRIVATE raylib m)
else()
    target_include_directories(dungeon_game PRIVATE
        ${raylib_SOURCE_DIR}/src
        ${raytmx_SOURCE_DIR}
        ${hoxml_SOURCE_DIR}/external
    )
    target_link_libraries(dungeon_game PRIVATE raylib hoxml raytmx m)
endif()

if(WIN32)
    target_link_libraries(dungeon_game PRIVATE gdi32 winmm)
elseif(APPLE)
    target_link_libraries(dungeon_game PRIVATE "-framework Cocoa")
endif()

# Copy assets
add_custom_command(TARGET dungeon_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:dungeon_game>/assets
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(dungeon_game PRIVATE -Wall -Wextra -O2)
endif()

install(TARGETS dungeon_game DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION bin/assets)
