##################################### libLtl.a #####################################
add_library(Ltl STATIC
        ltl/ltl.cpp
        utils/reader.cpp
)

target_include_directories(Ltl PUBLIC ${PROJECT_SOURCE_DIR}/include)
