# project/CMakeLists.txt

# Qt otomasyonları (moc, uic, rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt5 Widgets'i bul
# Eğer Qt6 kullanıyorsan: find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Uygulama kaynakları
# Buraya proje tarafındaki kodlarını ekle (UI, controller, main.cpp vs)
set(BARBER_APP_SOURCES
    main.cpp

    # örnekler:
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

# Bu executable barber_core kütüphanesine linkleniyor + Qt Widgets'e linkleniyor
target_link_libraries(BarberShopApp
    PRIVATE
        barber_core
        Qt5::Widgets
)

# Qt tarafında da C++17 kullan
target_compile_features(BarberShopApp PRIVATE cxx_std_17)

# Include path (ui/ ve controllers/ gibi relative headerlar için)
target_include_directories(BarberShopApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/controllers
        ${CMAKE_SOURCE_DIR}/lib/include
)

# Linux'ta rpath ayarı vs gerekirse ileride eklenebilir.
