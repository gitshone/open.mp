set(CLIENT_SOURCES
	pch.hpp
	dll_main.cpp
	client_core.hpp
	client_core.cpp
	config.hpp
	config.cpp

	Hooks/d3d9_device_hook.hpp
	Hooks/d3d9_device_hook.cpp
	Hooks/wnd_proc_hook.hpp
	Hooks/wnd_proc_hook.cpp
	Hooks/game_hooks.hpp
	Hooks/game_hooks.cpp

	Render/overlay.hpp
	Render/overlay.cpp

	UI/chat_window.hpp
	UI/chat_window.cpp
	UI/scoreboard.hpp
	UI/scoreboard.cpp
	UI/dialog.hpp
	UI/dialog.cpp
	UI/loading_screen.hpp
	UI/loading_screen.cpp
	UI/text_draw.hpp
	UI/text_draw.cpp

	Network/net_client.hpp
	Network/net_client.cpp
	Network/rpc_dispatcher.hpp
	Network/rpc_dispatcher.cpp
	Network/packet_handlers.hpp
	Network/packet_handlers.cpp

	Game/game_sa.hpp
	Game/game_sa.cpp)

add_library(Client SHARED ${CLIENT_SOURCES})
GroupSourcesByFolder(Client)

set_target_properties(Client PROPERTIES
	OUTPUT_NAME "openmp-client"
	PREFIX ""
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON)

target_include_directories(Client PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/Shared/NetCode)

target_compile_definitions(Client PRIVATE
	OMP_CLIENT
	OMP_CLIENT_VERSION_MAJOR=${OMP_CLIENT_VERSION_MAJOR}
	OMP_CLIENT_VERSION_MINOR=${OMP_CLIENT_VERSION_MINOR}
	OMP_CLIENT_VERSION_PATCH=${OMP_CLIENT_VERSION_PATCH}
	NOMINMAX
	WIN32_LEAN_AND_MEAN)

find_library(D3D9_LIB d3d9 REQUIRED)

target_link_libraries(Client PRIVATE
	${D3D9_LIB}
	user32
	gdi32
	winmm
	ws2_32
	minhook
	imgui
	spdlog::spdlog
	tomlplusplus::tomlplusplus)

if(TARGET RakNet-LibStatic)
	target_link_libraries(Client PRIVATE RakNet-LibStatic)
endif()

if(MSVC)
	target_compile_options(Client PRIVATE /W4 /permissive-)
endif()
