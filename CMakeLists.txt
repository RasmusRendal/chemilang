cmake_minimum_required(VERSION 3.10)

# Comment out when not debugging
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -fPIC")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set the project name
project(Chemilang VERSION 0.1)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

BISON_TARGET(parser src/parser.yy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/parser.cpp)
FLEX_TARGET(scanner src/scanner.l ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(scanner parser)

include_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
include_directories(src)

file(GLOB SOURCES "src/*.cpp")
message("${SOURCES}")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
file(GLOB TEST_SOURCES "tests/*.cpp")

find_package(Boost 1.56 REQUIRED COMPONENTS iostreams system)

# add the executable
add_executable(chemilang
	${SOURCES}
	"src/main.cpp"
	${BISON_parser_OUTPUTS}
	${FLEX_scanner_OUTPUTS}
)

TARGET_LINK_LIBRARIES(chemilang LINK_PUBLIC Boost::iostreams Boost::system)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(tests
	${TEST_SOURCES}
	${SOURCES}
	${BISON_parser_OUTPUTS}
	${FLEX_scanner_OUTPUTS}
)
target_link_libraries(tests ${GTEST_BOTH_LIBRARIES} Boost::iostreams Boost::system)

install(TARGETS chemilang DESTINATION /usr/local/bin/)
install(TARGETS chemlib DESTINATION /usr/local/share/)
