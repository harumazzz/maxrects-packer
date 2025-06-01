add_library(maxrects_packer
    rectangle.cpp
    abstract_bin.cpp
    maxrects_bin.cpp
    maxrects_packer.cpp
    oversized_element_bin.cpp
    rectangle.h
    abstract_bin.h
    maxrects_bin.h
    maxrects_packer.h
    oversized_element_bin.h
)

target_include_directories(maxrects_packer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(maxrects_packer PUBLIC cxx_std_20)
