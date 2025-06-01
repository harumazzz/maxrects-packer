add_executable(maxrects_tests
    simple_test.h
    test_rectangle.cpp
    test_maxrects_packer.cpp
    test_maxrects_bin.cpp
    test_oversized_element_bin.cpp
    test_main.cpp
)

target_link_libraries(maxrects_tests
    maxrects_packer
)
