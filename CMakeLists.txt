cmake_minimum_required(VERSION 3.14)

project(maty_client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Sql Network WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Sql Network WebSockets)
find_package(Threads)

# Sources
include_directories("src")
file(GLOB_RECURSE SOURCES "src/*.h" "src/*.cpp" "src/*.cc")
source_group("src" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES} "res/qml.qrc")
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick
	Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::WebSockets Threads::Threads)
