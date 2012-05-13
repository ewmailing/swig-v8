add_custom_target(
    analyze_class
    COMMAND ${SWIG_EXECUTABLE} -I${PROJECT_BINARY_DIR}/Dist/Lib -analyze -c++ 
            constructorHandler +before 
            destructorHandler +before 
            functionWrapper +before 
            ${CMAKE_CURRENT_SOURCE_DIR}/example.i
)
