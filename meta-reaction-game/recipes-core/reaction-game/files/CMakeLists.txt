cmake_minimum_required(VERSION 3.16)
project(reaction_game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Find libgpiod
find_path(GPIOD_INCLUDE_DIR gpiod.h)
find_library(GPIOD_LIBRARY gpiod)

if(NOT GPIOD_INCLUDE_DIR OR NOT GPIOD_LIBRARY)
    message(WARNING "libgpiod not found, GPIO functionality will be disabled")
    set(GPIOD_FOUND FALSE)
else()
    set(GPIOD_FOUND TRUE)
    message(STATUS "Found libgpiod: ${GPIOD_LIBRARY}")
endif()

# Configure GPIO support
if(GPIOD_FOUND)
    add_definitions(-DGPIO_SUPPORT_ENABLED)
endif()

qt6_standard_project_setup()

set(SOURCES
    reaction_game.cpp
)

set(HEADERS
    reaction_game.h
)

qt6_add_executable(reaction_game ${SOURCES} ${HEADERS})

target_link_libraries(reaction_game Qt6::Core Qt6::Widgets)

if(GPIOD_FOUND)
    target_include_directories(reaction_game PRIVATE ${GPIOD_INCLUDE_DIR})
    target_link_libraries(reaction_game ${GPIOD_LIBRARY})
endif()

install(TARGETS reaction_game DESTINATION bin)
