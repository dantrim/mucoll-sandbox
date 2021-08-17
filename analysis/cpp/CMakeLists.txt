cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(mucoll-sandbox)

##
## ROOT is required
##
find_package(ROOT 6.22 CONFIG REQUIRED)
include(${ROOT_USE_FILE})

##
## add our C++ executables
##

# make-histograms
add_executable(make-histograms src/make-histograms.cpp)
target_link_libraries(make-histograms PRIVATE ${ROOT_LIBRARIES} ${ROOT_EXE_LINKER_FLAGS})

# histos-simple
add_executable(make-histograms-loop src/make-histograms-loop.cpp)
target_link_libraries(make-histograms-loop PRIVATE ${ROOT_LIBRARIES} ${ROOT_EXE_LINKER_FLAGS})
