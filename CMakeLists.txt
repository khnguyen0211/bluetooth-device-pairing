cmake_minimum_required(VERSION 3.16)

project(bluetooth-device-pairing LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Bluetooth)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Bluetooth)

# Source files
set(SOURCES
    src/main.cpp
    src/utils/AppException.cpp
    src/models/MacAddress.cpp
    src/models/DiscoveryTime.cpp
    src/models/BluetoothDevice.cpp
    src/models/DeviceFormatter.cpp
    src/platform/WindowsStrategy.cpp
    src/bluetooth/BluetoothManager.cpp
    src/bluetooth/DeviceDiscoverer.cpp
    src/bluetooth/DevicePairer.cpp
    src/commands/DiscoverCommand.cpp
    src/commands/PairCommand.cpp
    src/application/CommandLineParser.cpp
    src/application/CommandExecutor.cpp
)

# Header files
set(HEADERS
    src/utils/AppException.h
    src/models/MacAddress.h
    src/models/DiscoveryTime.h
    src/models/BluetoothDevice.h
    src/models/DeviceFormatter.h
    src/platform/IPlatformStrategy.h
    src/platform/WindowsStrategy.h
    src/bluetooth/BluetoothManager.h
    src/bluetooth/DeviceDiscoverer.h
    src/bluetooth/DevicePairer.h
    src/commands/ICommand.h
    src/commands/DiscoverCommand.h
    src/commands/PairCommand.h
    src/application/CommandLineParser.h
    src/application/CommandExecutor.h
)

add_executable(bluetooth-device-pairing
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(bluetooth-device-pairing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(bluetooth-device-pairing PRIVATE 
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Bluetooth
)

include(GNUInstallDirs)
install(TARGETS bluetooth-device-pairing
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
