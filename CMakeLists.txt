cmake_minimum_required(VERSION 3.0.0)
project(libstudy VERSION 1.0.2 LANGUAGES C CXX)

include(CTest)
enable_testing()
find_package(Threads)
find_package(CURL)
find_package(Gettext)
find_package(Intl)
find_package(OpenSSL)
find_package(Git)

include(GNUInstallDirs)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(po)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
