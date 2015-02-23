project(chat)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}:/usr/local/opt/qt5)
endif()

set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

file(GLOB_RECURSE SOURCES src *.cpp)
qt5_add_resources(RESOURCES res/main.qrc)

add_executable(chat ${SOURCES} ${RESOURCES})

qt5_use_modules(chat Core Widgets Quick Network)