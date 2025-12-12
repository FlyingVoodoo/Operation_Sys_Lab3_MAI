cmake_minimum_required(VERSION 3.16)
project(Operation_Sys_Lab3_MAI CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(COMMON_SOURCES
	${CMAKE_SOURCE_DIR}/src/os.cpp
)

add_executable(parent
	${CMAKE_SOURCE_DIR}/src/parent.cpp
	${COMMON_SOURCES}
)

add_executable(child1
	${CMAKE_SOURCE_DIR}/src/child1.cpp
	${COMMON_SOURCES}
)

add_executable(child2
	${CMAKE_SOURCE_DIR}/src/child2.cpp
	${COMMON_SOURCES}
)

find_package(Threads REQUIRED)
target_link_libraries(parent PRIVATE Threads::Threads)
target_link_libraries(child1 PRIVATE Threads::Threads)
target_link_libraries(child2 PRIVATE Threads::Threads)

