##################################### libLtl.a #####################################
add_library(Ltl STATIC
        ltl/ltl.cpp
        ltl/closure.cpp
        utils/reader.cpp
        ../include/utils/dot_representation.hpp)

target_include_directories(Ltl PUBLIC ${PROJECT_SOURCE_DIR}/include)
