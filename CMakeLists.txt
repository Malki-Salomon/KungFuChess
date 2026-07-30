# KungFuChess - portable CMake build.
#
# This build exists ALONGSIDE KungFuChess.sln, which remains the canonical
# IDE build. Neither the .sln nor any .vcxproj is read or written by CMake;
# the two build systems are independent views of the same sources, and the
# project boundaries below are a deliberate 1:1 mirror of the ten Visual
# Studio projects. Nothing is merged, moved or re-layered here.
#
# Configure/build/test:
#   cmake --preset default
#   cmake --build build
#   ctest --test-dir build
#
# The preset gives a single-config Ninja tree, which is what makes those
# last two commands work exactly as written.
#
# A bare `cmake -S . -B build` also works (dependencies and compiler are
# found automatically), but ON WINDOWS it selects the Visual Studio
# generator, which is multi-config - so the test step then needs a config:
#   ctest --test-dir build -C Debug
# Add -G Ninja to the bare configure if you would rather not pass -C.
# On Linux the default generator is already single-config, so all three
# commands work verbatim there with no preset.

cmake_minimum_required(VERSION 3.21)

# ---------------------------------------------------------------------------
# vcpkg toolchain discovery
#
# Must run before project(): CMAKE_TOOLCHAIN_FILE is consumed there and
# ignored if set afterwards. This is what lets a bare `cmake -S . -B build`
# work with no flags and no environment setup. An explicitly supplied
# -DCMAKE_TOOLCHAIN_FILE=... always wins; -DKFC_AUTO_VCPKG=OFF disables the
# search entirely for builds that provide dependencies some other way
# (a distro package manager, or a Docker image with them preinstalled).
# ---------------------------------------------------------------------------
option(KFC_AUTO_VCPKG "Auto-detect a vcpkg toolchain file when none was supplied" ON)

if(KFC_AUTO_VCPKG AND NOT DEFINED CMAKE_TOOLCHAIN_FILE AND NOT DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
    foreach(_kfc_root
            "$ENV{VCPKG_ROOT}"
            "$ENV{USERPROFILE}/vcpkg"
            "$ENV{HOME}/vcpkg"
            "C:/vcpkg"
            "/opt/vcpkg")
        # Both the toolchain file AND a ports tree are required. Visual
        # Studio ships a cut-down vcpkg under VC/vcpkg that has the toolchain
        # but no port registry, and a VS developer shell points VCPKG_ROOT at
        # it - accepting that one fails later with a confusing baseline error.
        if(_kfc_root
           AND EXISTS "${_kfc_root}/scripts/buildsystems/vcpkg.cmake"
           AND IS_DIRECTORY "${_kfc_root}/ports")
            set(CMAKE_TOOLCHAIN_FILE "${_kfc_root}/scripts/buildsystems/vcpkg.cmake"
                CACHE FILEPATH "vcpkg toolchain file (auto-detected)")
            message(STATUS "KungFuChess: auto-detected vcpkg at ${_kfc_root}")
            break()
        endif()
    endforeach()
    unset(_kfc_root)
endif()

project(KungFuChess LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Global build shape
# ---------------------------------------------------------------------------

# Single-config generators (Ninja, Make) have no build type by default, which
# on MSVC means no /MDd and therefore no _DEBUG. Multi-config generators
# (Visual Studio, Ninja Multi-Config) pick their config at build time instead.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Executables land together in one directory. The GUI needs this (it loads
# its sprite assets from a path relative to the working directory) and it
# makes the server/shell-client pair easy to launch by hand.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ---------------------------------------------------------------------------
# Settings shared by every target.
#
# Mirrors the .vcxproj files: WarningLevel3 (/W3), ConformanceMode (/permissive-)
# and CharacterSet=Unicode (UNICODE/_UNICODE). The MSBuild-only _LIB/_CONSOLE
# defines are deliberately NOT replicated - nothing in the codebase reads them.
#
# C++17 is applied uniformly. Nine of the ten Visual Studio projects already
# set stdcpp17; KungFuChessCore had no LanguageStandard at all and so built as
# C++14 despite every one of its consumers being C++17.
# ---------------------------------------------------------------------------
function(kfc_apply_common_settings target)
    target_compile_features(${target} PUBLIC cxx_std_17)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W3 /permissive-)
    endif()
    if(WIN32)
        target_compile_definitions(${target} PRIVATE UNICODE _UNICODE)
    endif()
endfunction()

# Winsock and friends, as listed in AdditionalDependencies for the three
# networked executables. Non-Windows platforms get these from libc.
function(kfc_link_platform_sockets target)
    if(WIN32)
        target_link_libraries(${target} PRIVATE ws2_32 mswsock crypt32)
    endif()
endfunction()

# ---------------------------------------------------------------------------
# Optional GUI
#
# KungFuChessGUI is the only target needing OpenCV. It defaults ON when OpenCV
# is present and OFF otherwise, so a server-only environment (a future Docker
# image, or CI without OpenCV) configures cleanly instead of hard-failing.
#
# Note this does NOT gate the test suite: KungFuChessTest compiles the GUI's
# NetworkSnapshotAdapter.cpp into itself exactly as the .vcxproj does, and
# that file has no OpenCV dependency. Tests build and run with the GUI off.
# ---------------------------------------------------------------------------
find_package(OpenCV CONFIG QUIET)

# OpenCV_FOUND is left undefined (not just false) on a failed lookup, and an
# empty expansion would make option() choke on its argument count.
if(OpenCV_FOUND)
    set(_kfc_gui_default ON)
else()
    set(_kfc_gui_default OFF)
endif()
option(KFC_BUILD_GUI "Build the OpenCV-based graphical client" ${_kfc_gui_default})

if(KFC_BUILD_GUI AND NOT OpenCV_FOUND)
    message(FATAL_ERROR
        "KFC_BUILD_GUI is ON but OpenCV was not found. Install it (vcpkg: opencv4) "
        "or reconfigure with -DKFC_BUILD_GUI=OFF.")
endif()

enable_testing()

# ---------------------------------------------------------------------------
# Projects, in dependency order. One directory per Visual Studio project.
# ---------------------------------------------------------------------------
add_subdirectory(KungFuChessLogging)
add_subdirectory(KungFuChessProtocol)
add_subdirectory(KungFuChessCore)
add_subdirectory(KungFuChessAccounts)
add_subdirectory(KungFuChessSessionState)
add_subdirectory(KungFuChessClientCommon)
add_subdirectory(KungFuChessServer)
add_subdirectory(KungFuChessShellClient)
add_subdirectory(KungFuChessTest)

if(KFC_BUILD_GUI)
    add_subdirectory(KungFuChessGUI)
endif()

if(CMAKE_CONFIGURATION_TYPES)
    message(STATUS "KungFuChess: multi-config (${CMAKE_CONFIGURATION_TYPES}), GUI ${KFC_BUILD_GUI}")
else()
    message(STATUS "KungFuChess: build type ${CMAKE_BUILD_TYPE}, GUI ${KFC_BUILD_GUI}")
endif()
