add_custom_target(
    analyze_static_var
    COMMAND ${SWIG_EXECUTABLE} -I${PROJECT_BINARY_DIR}/Dist/Lib -analyze -c++ -all ${CMAKE_CURRENT_SOURCE_DIR}/example.i
)
