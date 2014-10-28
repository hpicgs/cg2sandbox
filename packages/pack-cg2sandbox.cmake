
# CPack configuration

if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")

    set(OPTION_PACK_GENERATOR "ZIP")

    # Initialize

    # Reset CPack configuration
    if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
        set(CPACK_IGNORE_FILES "")
        set(CPACK_INSTALLED_DIRECTORIES "")
        set(CPACK_SOURCE_IGNORE_FILES "")
        set(CPACK_SOURCE_INSTALLED_DIRECTORIES "")
        set(CPACK_STRIP_FILES "")
        set(CPACK_SOURCE_TOPLEVEL_TAG "")
        set(CPACK_SOURCE_PACKAGE_FILE_NAME "")
    endif()

    # Find cpack executable
    get_filename_component(CPACK_PATH ${CMAKE_COMMAND} PATH)
    set(CPACK_COMMAND "${CPACK_PATH}/cpack")


    # Package project

    set(project_name ${META_PROJECT_NAME})   # Name of package project
    set(project_root ${META_PROJECT_NAME})   # Name of root project that is to be installed


    # Package information

    string(TOLOWER ${META_PROJECT_NAME} package_name)          # Package name
    set(package_description     "${META_PROJECT_DESCRIPTION}") # Package description
    set(package_vendor          "${META_AUTHOR_ORGANIZATION}") # Package vendor
    set(package_maintainer      "${META_AUTHOR_MAINTAINER}")   # Package maintainer


    # Package specific options

    set(CMAKE_MODULE_PATH                   ${CMAKE_SOURCE_DIR}/packages/${project_name})


    # Package information

    set(CPACK_PACKAGE_NAME                  "${package_name}")
    set(CPACK_PACKAGE_VENDOR                "${package_vendor}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "${package_description}")
    set(CPACK_PACKAGE_VERSION               "${META_VERSION}")
    set(CPACK_PACKAGE_VERSION_MAJOR         "${META_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR         "${META_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH         "${META_VERSION_PATCH}")
#    set(CPACK_RESOURCE_FILE_LICENSE         "${CMAKE_SOURCE_DIR}/LICENSE")
#    set(CPACK_RESOURCE_FILE_README          "${CMAKE_SOURCE_DIR}/README.md")
#    set(CPACK_RESOURCE_FILE_WELCOME         "${CMAKE_SOURCE_DIR}/README.md")
#    set(CPACK_PACKAGE_DESCRIPTION_FILE      "${CMAKE_SOURCE_DIR}/README.md")
#    set(CPACK_PACKAGE_ICON                  "${CMAKE_SOURCE_DIR}/packages/logo.bmp")
    set(CPACK_PACKAGE_RELOCATABLE           OFF)

    # Package name

    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${package_name}-${CPACK_PACKAGE_VERSION}")

    # Install files

    set(CPACK_INSTALL_CMAKE_PROJECTS        "${CMAKE_BINARY_DIR};${project_root};ALL;/")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY     "${package_name}")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY  "${package_name}")


    # Set generator

    set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${CMAKE_BINARY_DIR}/CPackSourceConfig-${project_name}.cmake")
    set(CPACK_SOURCE_GENERATOR ${OPTION_PACK_GENERATOR})
    set(CPACK_SOURCE_INSTALLED_DIRECTORIES "${CMAKE_SOURCE_DIR}/source;/source;${CMAKE_SOURCE_DIR}/data;/data")


    # CPack

    if(NOT WIN32)
        # Important: Must be set to install files to absolute path (e.g., /etc)
        # -> CPACK_[RPM_]PACKAGE_RELOCATABLE = OFF
        set(CPACK_SET_DESTDIR ON)
    endif()
    set(CPack_CMake_INCLUDED FALSE)
    include(CPack)
endif()


# Package target

add_custom_target(
    pack-${project_name}
    COMMAND ${CPACK_COMMAND} --config ${CMAKE_BINARY_DIR}/CPackSourceConfig-${project_name}.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_target_properties(pack-${project_name} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD 1)


# Dependencies

if(MSVC)
    add_dependencies(pack-${project_name} ALL_BUILD)
endif()
add_dependencies(pack pack-${project_name})
