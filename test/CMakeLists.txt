aux_source_directory(. test_src)

add_executable(test ${test_src})

target_include_directories(
    test 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/inc 
    /home/pzy/software/gtest/install/include
)

target_link_directories(
    test 
    PRIVATE 
    /home/pzy/software/gtest/install/lib
)

target_link_libraries(test gtest gtest_main pthread)

target_compile_options(test PRIVATE -g)