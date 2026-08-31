cmake_minimum_required(VERSION 3.16)
project(jumpyball C)

# The port is ANSI C (C89).  Extensions stay ON because snprintf is C99 and is
# only declared by the system headers in gnu89 mode.
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

option(JUMPYBALL_WARNINGS "Enable the full warning set" ON)
option(JUMPYBALL_BUILD_TOOLS "Build tools/modtest" OFF)
option(JUMPYBALL_COPY_ASSETS "Copy BITMAP/Sounds/Musics next to the binary" ON)

set(JUMPYBALL_SOURCES
    jb_appassets.c
    jb_assets.c
    jb_audio.c
    jb_ball.c
    jb_bmp.c
    jb_gfx.c
    jb_gfx_tile.c
    jb_keyconfig.c
    jb_level.c
    jb_main.c
    jb_menu.c
    jb_mod.c
    jb_mod_effects.c
    jb_platform_sdl2.c
    jb_player.c
    jb_stage.c
    jb_text.c
    jb_track.c
    jb_trackrow_forest.c
    jb_trackrow_grass.c
    jb_trackrow_ice_alt.c
    jb_trackrow_sky.c
    jb_trackrow_tiled.c)

find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
    find_package(SDL2 QUIET)
endif()
if(NOT SDL2_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2 IMPORTED_TARGET sdl2)
    endif()
endif()
if(NOT SDL2_FOUND)
    message(FATAL_ERROR
            "SDL2 not found.  Install libsdl2-dev / sdl2 / vcpkg's sdl2, or pass "
            "-DCMAKE_PREFIX_PATH=<sdl2 prefix>.")
endif()

add_executable(jumpyball WIN32 ${JUMPYBALL_SOURCES})

if(TARGET SDL2::SDL2)
    target_link_libraries(jumpyball PRIVATE SDL2::SDL2)
    if(TARGET SDL2::SDL2main)
        target_link_libraries(jumpyball PRIVATE SDL2::SDL2main)
    endif()
elseif(TARGET PkgConfig::SDL2)
    target_link_libraries(jumpyball PRIVATE PkgConfig::SDL2)
else()
    target_include_directories(jumpyball PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(jumpyball PRIVATE ${SDL2_LIBRARIES})
endif()

if(WIN32)
    target_link_libraries(jumpyball PRIVATE shell32)
endif()

if(MSVC)
    target_compile_definitions(jumpyball PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(NOT WIN32)
    include(CheckLibraryExists)
    check_library_exists(m sin "" JUMPYBALL_NEED_LIBM)
    if(JUMPYBALL_NEED_LIBM)
        target_link_libraries(jumpyball PRIVATE m)
    endif()
endif()

if(JUMPYBALL_WARNINGS)
    if(MSVC)
        target_compile_options(jumpyball PRIVATE /W4)
    else()
        target_compile_options(jumpyball PRIVATE -Wall -Wextra)
    endif()
endif()

if(JUMPYBALL_COPY_ASSETS)
    foreach(dir BITMAP Sounds Musics)
        add_custom_command(TARGET jumpyball POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_directory
                                   ${CMAKE_CURRENT_SOURCE_DIR}/${dir}
                                   $<TARGET_FILE_DIR:jumpyball>/${dir})
    endforeach()
endif()

if(JUMPYBALL_BUILD_TOOLS)
    add_executable(modtest tools/modtest.c jb_mod.c jb_mod_effects.c)
    target_include_directories(modtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(modtest PROPERTIES C_STANDARD 90 C_EXTENSIONS ON)
endif()

install(TARGETS jumpyball RUNTIME DESTINATION bin)
install(DIRECTORY BITMAP Sounds Musics DESTINATION bin)
