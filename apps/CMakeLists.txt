set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LTL_BINARY_DIR})

file(INSTALL
        test.txt
        run.sh
    DESTINATION
        ${LTL_BINARY_DIR}
)

##################################### emptiness_dfs #####################################
add_executable(ltl_converter ltl_converter.cpp)
target_link_libraries(ltl_converter PRIVATE Ltl)
