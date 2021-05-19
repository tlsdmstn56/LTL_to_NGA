##################################### libLtl.so #####################################
add_library(Ltl SHARED
        ltl/ltl.cpp
        ltl/closure.cpp
        utils/reader.cpp
        utils/dot_representation.cpp)

target_include_directories(Ltl PUBLIC ${PROJECT_SOURCE_DIR}/include)
