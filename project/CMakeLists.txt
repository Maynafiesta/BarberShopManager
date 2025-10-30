# UI + executable tarafı

set(BARBER_APP_SOURCES
  main.cpp

  ui/MainWindow.cpp
  ui/MainWindow.h
  ui/MainWindow.ui

  ui/AppointmentDialog.cpp
  ui/AppointmentDialog.h
  ui/AppointmentDialog.ui

  controllers/AppointmentController.cpp
  controllers/AppointmentController.h

  controllers/SalonController.cpp
  controllers/SalonController.h
)

add_executable(BarberShopApp ${BARBER_APP_SOURCES})

target_link_libraries(BarberShopApp
  PRIVATE
    barber_core
    ${QT_PACKAGE}::Widgets
)

target_compile_features(BarberShopApp PRIVATE cxx_std_17)

target_include_directories(BarberShopApp
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/controllers
    ${CMAKE_SOURCE_DIR}/lib/include
)
