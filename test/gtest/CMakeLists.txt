cmake_minimum_required(VERSION 3.10)
project( trie_tests )
set ( CMAKE_CXX_STANDARD 11)
set ( CMAKE_CXX_STANDARD_REQUIRED True )

# ------------------------------------------------------------------------------
#   Extra Warnings
# ------------------------------------------------------------------------------

# Apple osx clang >= 9.3, some other versions of clang, as well as new gcc versions >= 9
# spam a lot of warnings in googletest about implicitly-declared types being deprecated.
# Let's silence them, as we are not interested in warnings about googletest.
# That's something they have to fix!
# Also, we have to use the double cmake version comparison here, as old osx cmake versions
# do not yet have VERSION_GREATER_EQUAL, and crash otherwise...
SET( GTEST_NO_WARN_DEPRECATED_COPY OFF )
IF(
	(
		#APPLE AND
		(CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND
		((CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "9.3") OR
		(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "9.3"))
	) OR (
		(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND
		((CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "9") OR
		(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "9"))
	) OR (
		CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND
		NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13"
	)
)
	message (STATUS "Deactivate -Wdeprecated-copy for building gtest.")
	SET( GTEST_NO_WARN_DEPRECATED_COPY ON )
ENDIF()
# message (STATUS "GTEST_NO_WARN_DEPRECATED_COPY: ${GTEST_NO_WARN_DEPRECATED_COPY}")

# Without any optimization, and with recent gcc, gtest causes some errors:
# error: 'dummy' may be used uninitialized [-Werror=maybe-uninitialized]
# Let's stop that from messing with our build - not our problem to fix!
SET( GTEST_NO_WARN_MAYBE_UNINITIALIZED OFF )
if(
	"${CMAKE_CXX_FLAGS}" MATCHES "-O0" OR
	"${CMAKE_CXX_FLAGS_DEBUG}" MATCHES "-O0" OR
	(
		(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND
		((CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "9") OR
		(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "9"))
	)
)
	message (STATUS "Deactivate -Wmaybe-uninitialized for building gtest.")
	SET( GTEST_NO_WARN_MAYBE_UNINITIALIZED ON )
endif()

# ------------------------------------------------------------------------------
#   Setup GTest
# ------------------------------------------------------------------------------

# Find GTest!
message (STATUS "Looking for GTest")
find_package (GTest)

if(NOT GTEST_FOUND)
	message (STATUS "GTest not found")
	message (STATUS "Will now download GTest")

	# If GTest was not found, we download and unpack it (at configure time). This roughly follows
	# https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project

	configure_file(
		./GTestDownload.cmake
		${PROJECT_SOURCE_DIR}/gtest/googletest-download/CMakeLists.txt
	)

	execute_process( COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/gtest/googletest-download/
	)

	if(result)
		message (STATUS "Cannot configure GTest: ${result}")
		message (STATUS "${ColorRed}Cannot build tests${ColorEnd}")
		return()
	endif()

	execute_process( COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/gtest/googletest-download/
	)

	if(result)
		message (STATUS "Cannot build GTest: ${result}")
		message (STATUS "${ColorRed}Cannot build tests${ColorEnd}")
		return()
	endif()

	# Prevent overriding the parent project's compiler/linker settings on Windows.
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

	# Add googletest directly to our build. This defines the gtest and gtest_main targets.
	# We only want test, not mock.
	set(BUILD_GTEST ON  CACHE BOOL "Builds the googletest subproject")
	set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject")
	add_subdirectory( ${PROJECT_SOURCE_DIR}/gtest/googletest-src  ${PROJECT_SOURCE_DIR}/gtest/googletest-build )

	# The gtest/gtest_main targets carry header search path dependencies automatically when using
	# CMake 2.8.11 or later. Otherwise we have to add them here ourselves.
	if (${CMAKE_VERSION} VERSION_LESS 2.8.11)
		include_directories( SYSTEM "${gtest_SOURCE_DIR}/include")
	endif()

	# Set targets to what they would be if we already had GTest. This way, we can use the same
	# target_link_libraries command.
	SET( GTEST_BOTH_LIBRARIES gtest_main gtest )
	SET( GTEST_LIBRARIES gtest )

	if(GTEST_NO_WARN_DEPRECATED_COPY)
		target_compile_options(gtest      PRIVATE "-Wno-deprecated-copy")
		target_compile_options(gtest_main PRIVATE "-Wno-deprecated-copy")
		# target_compile_options(gmock      PRIVATE "-Wno-deprecated-copy")
		# target_compile_options(gmock_main PRIVATE "-Wno-deprecated-copy")
	endif()
	if(GTEST_NO_WARN_MAYBE_UNINITIALIZED)
		target_compile_options(gtest      PRIVATE "-Wno-uninitialized")
		target_compile_options(gtest_main PRIVATE "-Wno-uninitialized")
	endif()

	message (STATUS "Finished downloading GTest")
endif()

message (STATUS "Found GTest: ${GTEST_LIBRARIES}")
message (STATUS "${ColorBlue}Building tests${ColorEnd}")

# We include the GTest dirs as SYSTEM, as they are currently producing ugly warnings that spam
# our build output unnecessarily.
include_directories( SYSTEM ${GTEST_INCLUDE_DIRS} )

add_executable(trie_tests ./src/string.cpp)

target_link_libraries(trie_tests PUBLIC gtest_main trie)

add_test(NAME trie_tests COMMAND trie_tests)
