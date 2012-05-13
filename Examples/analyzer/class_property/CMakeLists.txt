add_custom_target(
    analyze_class_property
    COMMAND ${SWIG_EXECUTABLE} -I${PROJECT_BINARY_DIR}/Dist/Lib -analyze -c++ 
            functionWrapper +before 
            ${CMAKE_CURRENT_SOURCE_DIR}/example.i
)
