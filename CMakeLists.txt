cmake_minimum_required(VERSION 3.16)

# Proje ismi
project(BarberShopManager LANGUAGES CXX)

# Varsayılan derleme tipi (Debug / Release). Eğer cmake .. çağırırken belirtmezsen Release yapar.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# C++ standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Uyarılar (sadece GCC / Clang için, Windows MSVC ise farklı flag veririz)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4)
endif()

# Alt projeleri ekle
add_subdirectory(lib)
add_subdirectory(project)
