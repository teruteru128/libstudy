cmake_minimum_required(VERSION 3.0.0)
project(libstudy VERSION 1.0.2 LANGUAGES C CXX)

#include(CTest)
enable_testing()
find_package(Threads)
if(Threads_FOUND)

endif(Threads_FOUND)

find_package(CURL)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
endif(CURL_FOUND)

find_package(Gettext)
if(GETTEXT_FOUND)
endif(GETTEXT_FOUND)

find_package(Intl)
if(Intl_FOUND)
        include_directories(${Intl_INCLUDE_DIRS})
        set(LIBINTL_LIBRARIES ${Intl_LIBRARIES})
    else()
        set(LIBINTL_LIBRARIES)
endif(Intl_FOUND)

find_package(OpenSSL)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif(OPENSSL_FOUND)

find_package(GnuTLS REQUIRED)
if(GNUTLS_FOUND)
#    include_directories(${GNUTLS_INCLUDE_DIR})
endif(GNUTLS_FOUND)

find_package(Git)

find_package(Iconv)

include(GNUInstallDirs)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(po)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
