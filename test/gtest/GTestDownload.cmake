cmake_minimum_required ( VERSION 3.10 )
project( googletest_download )
set ( CMAKE_CXX_STANDARD 11)
set ( CMAKE_CXX_STANDARD_REQUIRED True )

include ( ExternalProject )

# Download a fixed release instead of the current master,
# so that we know that it works for us.
ExternalProject_Add ( googletest
    URL https://github.com/google/googletest/archive/release-1.8.0.zip
    SOURCE_DIR        "${CMAKE_BINARY_DIR}/googletest-src"
    BINARY_DIR        "${CMAKE_BINARY_DIR}/googletest-build"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    TEST_COMMAND      ""
    DOWNLOAD_NO_PROGRESS TRUE
)
