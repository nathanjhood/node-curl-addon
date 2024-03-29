cmake_minimum_required(VERSION 3.15)

project(node_curl_addon VERSION 1.0.0)

if(NOT DEFINED CMAKE_JS_VERSION)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/node_modules/cmake-js/share/cmake")
endif()

# Get CURL::libcurl
find_package(CURL REQUIRED)

if(${PROJECT_NAME}_IS_INSTALLABLE)
  include(GNUInstallDirs)
  # cURL relocate...
  if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/include/curl")
    # Set up cURL for transport
    file(COPY "${CURL_INCLUDE_DIR}/curl" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include")
  endif()
  set (CURL_INCLUDE_DIR
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/curl>
    $<INSTALL_INTERFACE:include/curl>
    CACHE PATH "Path to curl.h" FORCE
  )
  install(FILES ${CURL_INCLUDE_DIR} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()

# Get cmake-js::cmake-js via 'cmakejs_create_napi_addon()'
include(CMakeJS)

cmakejs_create_napi_addon(
  addon
  "src/node-curl-addon/addon.cpp"
  "src/node-curl-addon/get.cpp"
  "src/node-curl-addon/post.cpp"
  NAMESPACE ${PROJECT_NAME}
  EXCEPTIONS YES
)
# Move project headers...
configure_file(
  "include/node-curl-addon/addon.h" # from SOURCE_DIR...
  "include/node-curl-addon/addon.h" # ..to BINARY_DIR
)
target_include_directories(addon PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/node-curl-addon>
  $<INSTALL_INTERFACE:include/node-curl-addon>
)

# Link Addon to cURl
target_link_libraries(addon PRIVATE CURL::libcurl)
target_include_directories(addon PRIVATE ${CURL_INCLUDE_DIR})

# TESTING
if(NOT DEFINED BUILD_TESTS AND ${PROJECT_NAME}_IS_TOP_LEVEL)
  set(BUILD_TESTS ON)
endif()
if(BUILD_TESTS AND ${PROJECT_NAME}_IS_TOP_LEVEL)
  # include(CTest) # uncomment this if you want to see the Test Dashboard targets (lots...)
  enable_testing()
  include("${CMAKE_CURRENT_LIST_DIR}/tests/node-curl-addon/tests.cmake")
endif()

# PACKING
if(${PROJECT_NAME}_IS_INSTALLABLE AND ${PROJECT_NAME}_IS_TOP_LEVEL)
  # CPack prep stuff...
  include (CMakePackageConfigHelpers)

  file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/node-curl-addon-config.cmake.in" [==[
@PACKAGE_INIT@

include (${CMAKE_CURRENT_LIST_DIR}/node-curl-addon-targets.cmake)

check_required_components (node-curl-addon)

]==])

  # create cmake config file
  configure_package_config_file (
      "${CMAKE_CURRENT_BINARY_DIR}/node-curl-addon-config.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/node-curl-addon-config.cmake"
    INSTALL_DESTINATION
      "${CMAKE_INSTALL_LIBDIR}/cmake/node-curl-addon"
  )
  # generate the version file for the cmake config file
  write_basic_package_version_file (
    "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/node-curl-addon-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
  )
  # pass our build script along
  file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/share/cmake/node-curl-addon.cmake" "${CMAKE_CURRENT_LIST_FILE}")
  export (
    TARGETS addon
    FILE share/cmake/node-curl-addon-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
  )

  install(TARGETS addon
    EXPORT node-curl-addon-targets
    LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION  ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(
    EXPORT node-curl-addon-targets
    FILE node-curl-addon-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/node-curl-addon
  )

  # set(CPACK_PACKAGE_CHECKSUM "${PROJECT_VERSION_TWEAK}") # git rev-parse is a good idea for this...
  set(CPACK_PACKAGE_VENDOR "node-curl-addon")
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-node_${NODE_VERSION}")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-node_${NODE_VERSION}")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME  "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-Source")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_VERSION_PATCH})
  set(CPACK_PACKAGE_VERSION_TWEAK ${PROJECT_VERSION_VERSION_TWEAK})
  set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
  set(CPACK_RESOURCE_FILE_README  ${PROJECT_SOURCE_DIR}/README.md)
  set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
  set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)
  set(CPACK_SOURCE_GENERATOR "TGZ;ZIP") # Check out CPack's 'NSIS' installer for Win32, and the others!
  set(CPACK_SOURCE_IGNORE_FILES
    _CPack_Packages
    /*.zip
    /*.tar
    /*.tar.*
    /.env*
    /.git/*
    /.cmake
    /.github
    /.vs
    /.vscode
    /.cache
    /.config
    /.local
    /dist
    /doc
    /docs
    #/bin
    #/lib
    /usr
    /out
    #/build
    /Release
    /Debug
    /MinSizeRel
    /RelWithDebInfo
    /downloads
    /installed
    /install
    /node_modules
    /vcpkg
    /.*build.*
    /package-lock.json
    /yarn.lock
    /\\\\.DS_Store
  )
  include(CPack)
endif()
