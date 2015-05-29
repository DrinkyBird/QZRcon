    #######################
    # QZRCon CMake script #
    #######################

cmake_minimum_required (VERSION 2.8.11)

project (QZRCon)

# Find includes in corresponding build directories
set (CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set (CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package (Qt5Gui)
find_package (Qt5Network)
find_package (Qt5Core)

# Find all C++ files, add them to executable
file (GLOB SOURCEFILES src/*.cpp)
add_executable (${PROJECT_NAME} WIN32 ${SOURCEFILES})

# Link Qt5 libraries
target_link_libraries (${PROJECT_NAME} Qt5::Gui)
target_link_libraries (${PROJECT_NAME} Qt5::Network)
target_link_libraries (${PROJECT_NAME} Qt5::Core)