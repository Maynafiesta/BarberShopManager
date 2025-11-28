cmake_minimum_required(VERSION 3.15)

project(BarberShopManager LANGUAGES CXX)

option(BARBER_BUILD_GUI "Build the Qt GUI application" ON)
option(BARBER_BUILD_TESTS "Build core library tests" ON)

# default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# warnings + debug flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-g3 -O0 -Wall -Wextra -Wpedantic -Werror=return-type)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/W4 /Zi)
endif()

# Qt GUI isteğe bağlı: sadece istenirse bul ve ekle
if(BARBER_BUILD_GUI)
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
endif()

add_subdirectory(lib)

if(BARBER_BUILD_GUI)
  add_subdirectory(project)
else()
  message(STATUS "BARBER_BUILD_GUI=OFF: Qt GUI skipped")
endif()

if(BARBER_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

