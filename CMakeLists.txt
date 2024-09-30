cmake_minimum_required(VERSION 3.0)
project(TIIGR)

# Ensure C++11 or later is enabled
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Geant4 package and include necessary directories and definitions
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
include_directories(SYSTEM ${Geant4_INCLUDE_DIRS})

# Find ROOT package and include necessary directories and definitions
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

# Find OpenGL and Qt packages and include necessary directories and definitions
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

ADD_DEFINITIONS("-Wall -ansi -pedantic -std=c++17")

# Include the Qt5 and OpenGL directories
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})


include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_library(MyLib ${sources} ${headers})

# Create executable
file(GLOB APP_SOURCES ${PROJECT_SOURCE_DIR}/exec/*.cxx)

foreach(testsourcefile ${APP_SOURCES})
    get_filename_component(testname ${testsourcefile} NAME_WE)
    add_executable(${testname} ${testsourcefile})
    target_link_libraries(${testname} MyLib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES} Qt5::Widgets Qt5::OpenGL)
    install(TARGETS ${testname} RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endforeach(testsourcefile ${APP_SOURCES})

# Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Make sure CMake knows about Qt's UI files, if any
set(CMAKE_AUTOMOC ON)  # Ensures that Qt meta-object code is generated
set(CMAKE_AUTOUIC ON)  # Ensures that UI files are processed
set(CMAKE_AUTORCC ON)  # Ensures that Qt resource files are handled

