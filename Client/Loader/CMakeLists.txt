set(LOADER_SOURCES
	main.cpp
	inject.hpp
	inject.cpp
	game_detect.hpp
	game_detect.cpp)

add_executable(Loader WIN32 ${LOADER_SOURCES})
GroupSourcesByFolder(Loader)

set_target_properties(Loader PROPERTIES
	OUTPUT_NAME "openmp"
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON)

target_compile_definitions(Loader PRIVATE
	OMP_CLIENT_LOADER
	NOMINMAX
	WIN32_LEAN_AND_MEAN)

target_link_libraries(Loader PRIVATE
	user32
	shell32
	advapi32
	spdlog::spdlog)

add_dependencies(Loader Client)
