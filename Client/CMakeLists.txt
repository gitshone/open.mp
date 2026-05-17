# open.mp client — Windows x86 only, opt-in via -DBUILD_CLIENT=ON.
#
# Produces:
#   openmp-client.dll  Injected into gta_sa.exe by the loader.
#   openmp.exe         Loader; replaces the legacy SA-MP launcher pattern.
#   openmp-client-setup.exe  (optional) NSIS installer, -DBUILD_CLIENT_INSTALLER=ON.

if(NOT WIN32)
	message(STATUS "Client: skipped (Windows only)")
	return()
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(FATAL_ERROR "Client must be built as 32-bit to match gta_sa.exe. Re-run cmake with -A Win32.")
endif()

message("Configuring client")

set(OMP_CLIENT_VERSION_MAJOR 0)
set(OMP_CLIENT_VERSION_MINOR 1)
set(OMP_CLIENT_VERSION_PATCH 0)

include(FetchContent)

# MinHook — function detours and vtable hooks.
FetchContent_Declare(minhook
	GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
	GIT_TAG        v1.3.4)
FetchContent_MakeAvailable(minhook)

# Dear ImGui — overlay rendering on D3D9.
FetchContent_Declare(imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG        v1.91.5)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp)
target_include_directories(imgui PUBLIC
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends)

# spdlog — logging.
FetchContent_Declare(spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG        v1.15.0)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# toml++ — config files.
FetchContent_Declare(tomlplusplus
	GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
	GIT_TAG        v3.4.0)
FetchContent_MakeAvailable(tomlplusplus)

add_subdirectory(Source)
add_subdirectory(Loader)

option(BUILD_CLIENT_INSTALLER "Build the NSIS installer for the open.mp client" OFF)
if(BUILD_CLIENT_INSTALLER)
	add_subdirectory(Installer)
endif()
