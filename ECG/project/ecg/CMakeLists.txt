
# helloapp target instructions

#####################################
# Define Target Information
#####################################
set(TARGET_NAME ecg)
set(TARGET_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TARGET_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/ecg.cpp)

#####################################
# Support IDE Visualization
#####################################
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_SOURCES})

#####################################
# Define Target according to options
#####################################
add_executable(${TARGET_NAME} ${TARGET_SOURCES})

#####################################
# Define Target Properties and Optional OpenSSL Dependency
#####################################

# add target properties such as debug postfix and MSVC runtime
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "ecg" DEBUG_POSTFIX "-d" )

# set target compile options as defined in the cmake/compiler_options.cmake Module
target_compile_options(${TARGET_NAME} PRIVATE ${compiler_options})

# set target link options as defined in the cmake/compiler_options.cmake Module
target_link_options(${TARGET_NAME} PRIVATE ${linker_flags})

# include all the necessary directories
target_include_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/exports>
  PRIVATE 
    ${TARGET_INCLUDE_FOLDER}
    ${Boost_INCLUDE_DIRS}
    ${BOOST_LOCKFREE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../ads1115/include
    # Any other extra include required
)

target_link_libraries(${TARGET_NAME} PUBLIC ${PIGPIO_LIBRARY} ads1115)

add_executable(
  ecg_test
  test/ecg_test.cpp
)
target_link_libraries(
  ecg_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ecg_test)