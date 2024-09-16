include(FetchContent)


find_program(PATCH
NAMES patch
HINTS ${GIT_DIR}
PATH_SUFFIXES usr/bin
)

if(NOT PATCH)
  message(FATAL_ERROR "Did not find GNU Patch")
endif()

FetchContent_Declare(
    asio
    URL      https://github.com/chriskohlhoff/asio/archive/asio-1-16-0.zip
    URL_HASH SHA1=6BDD33522D5B95B36445ABB2072A481F7CE15402
)

FetchContent_GetProperties(asio)

set(in_file1
${CMAKE_SOURCE_DIR}/_deps/asio-src/asio/include/asio/impl/serial_port_base.ipp
)

set(in_file2
${CMAKE_SOURCE_DIR}/_deps/asio-src/asio/include/asio/serial_port_base.hpp
)

set(patch_file1
${CMAKE_SOURCE_DIR}/deps/serial_port_base.ipp.patch
)

set(patch_file2
${CMAKE_SOURCE_DIR}/deps/serial_port_base.hpp.patch
)

if(NOT asio_POPULATED)
    FetchContent_Populate(asio)

    find_package(Threads)

    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)

    execute_process(COMMAND echo pathisthis ${CMAKE_SOURCE_DIR}
    TIMEOUT 15
    COMMAND_ECHO STDOUT
    RESULT_VARIABLE ret
    )

    execute_process(COMMAND ${PATCH} ${in_file1} ${patch_file1}
    TIMEOUT 15
    COMMAND_ECHO STDOUT
    RESULT_VARIABLE ret
    )

    execute_process(COMMAND ${PATCH} ${in_file2} ${patch_file2}
    TIMEOUT 15
    COMMAND_ECHO STDOUT
    RESULT_VARIABLE ret
    )

    if(NOT ret EQUAL 0)
        message(FATAL_ERROR "Failed to patch asio")
    endif()

    target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
    target_compile_features(asio INTERFACE cxx_std_11)
    target_link_libraries(asio INTERFACE Threads::Threads)

    if(WIN32)
        target_link_libraries(asio INTERFACE ws2_32 wsock32) # Link to Winsock
        target_compile_definitions(asio INTERFACE _WIN32_WINNT=0x0601) # Windows 7 and up
    endif()
endif()
