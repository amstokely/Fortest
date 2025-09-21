configure_file(
        FortestConfig.h.in
        ${CMAKE_BINARY_DIR}/FortestConfig.h
        @ONLY
)


# Configure the header from the template
configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/FortestConfig.h.in
        ${CMAKE_BINARY_DIR}/FortestConfig.h
        @ONLY
)

# Install the generated header so downstream tools (like fortest-config) can use it
install(FILES ${CMAKE_BINARY_DIR}/FortestConfig.h
        DESTINATION include/fortest)

# Install the executable into <prefix>/bin
install(TARGETS fortest-config
        RUNTIME DESTINATION bin
        COMPONENT runtime)



include(CMakePackageConfigHelpers)

# Export targets
install(EXPORT FortestTargets
        FILE FortestTargets.cmake
        NAMESPACE Fortest::
        DESTINATION lib/cmake/Fortest
)

# Create the config file that includes the exported targets
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/FortestConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/FortestConfig.cmake"
        INSTALL_DESTINATION lib/cmake/Fortest
)

# Version file (already fixed)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/FortestConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

# Install config + version files
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/FortestConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/FortestConfigVersion.cmake"
        DESTINATION lib/cmake/Fortest
)


# Generate version file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/FortestConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
)

# Install version + config file
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/FortestConfigVersion.cmake"
        DESTINATION lib/cmake/Fortest
)
