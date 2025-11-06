cmake_minimum_required(VERSION 3.15)

project(BarberShopManager LANGUAGES CXX)

# default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/W4)
endif()

# Qt auto-magic (etki alanı: tüm alt dizinler)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt'yi bul: önce Qt6, yoksa Qt5 (Ubuntu varsayılan yolu ipucu olarak eklenir)
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
  set(QT_PACKAGE Qt6)
else()
  if(NOT DEFINED Qt5_DIR)
    # Ubuntu 20.04/22.04 tipik konum:
    set(Qt5_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt5" CACHE PATH "Path to Qt5Config.cmake")
  endif()
  find_package(Qt5 REQUIRED COMPONENTS Widgets)
  set(QT_PACKAGE Qt5)
endif()

add_subdirectory(lib)
add_subdirectory(project)

