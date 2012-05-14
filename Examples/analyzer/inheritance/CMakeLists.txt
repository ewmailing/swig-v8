add_custom_target(
    analyze_inheritance
    COMMAND ${SWIG_EXECUTABLE} -I${PROJECT_BINARY_DIR}/Dist/Lib -analyze -c++ 
            functionWrapper +before
            classHandler +before +after
            ${CMAKE_CURRENT_SOURCE_DIR}/example.i
)
