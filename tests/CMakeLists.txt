add_executable(barber_core_tests
  test_scheduler.cpp
)

target_link_libraries(barber_core_tests
  PRIVATE
    barber_core
)

target_include_directories(barber_core_tests
  PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
)

add_test(NAME barber_core_tests COMMAND barber_core_tests)
