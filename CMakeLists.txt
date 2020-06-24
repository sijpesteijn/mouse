cmake_minimum_required(VERSION 3.8)
project(mouse)

set(CMAKE_CXX_STANDARD 11)

if( APPLE )
    include_directories( /usr/local/include/libusb-1.0 )

    link_directories(/usr/local/lib)
    set(LIBS usb-1.0 )
endif()

set(SOURCE_FILES main.cpp)
add_executable(mouse ${SOURCE_FILES})
target_link_libraries( mouse ${LIBS})