#include <QCoreApplication>
#include <QTextStream>
#include "bluetooth/BluetoothManager.h"
#include "bluetooth/DeviceDiscoverer.h"
#include "bluetooth/DevicePairer.h"
#include "models/DiscoveryTime.h"
#include "models/MacAddress.h"
#include "models/DeviceFormatter.h"
#include "utils/AppException.h"

// ============================================================
// CONFIGURATION
// ============================================================

// Discovery settings
const int DISCOVERY_TIME_SECONDS = 10;

// Pairing settings - Change TARGET_MAC_ADDRESS to your device MAC
// const QString TARGET_MAC_ADDRESS = "E6:4C:AB:B5:F6:50";
const QString TARGET_MAC_ADDRESS = "E7:77:AF:A4:81:CC";
const QString PIN_CODE = "0000";

// Set to true to run pairing after discovery
const bool RUN_PAIR_AFTER_DISCOVER = true;

// ============================================================

int runDiscover()
{
    QTextStream out(stdout);

    DiscoveryTime time(DISCOVERY_TIME_SECONDS);
    IPlatformStrategy* strategy = BluetoothManager::instance().platformStrategy();
    DeviceDiscoverer discoverer(strategy);

    QList<BluetoothDevice> devices = discoverer.discoverDevices(time);
    
    out << "\nFound " << devices.size() << " device(s):\n";
    out.flush();
    
    DeviceFormatter::printDeviceTable(devices);
    
    return 0;
}

int runPair()
{
    QTextStream out(stdout);

    out << "\n--- Starting Pairing ---\n";
    out.flush();

    MacAddress mac(TARGET_MAC_ADDRESS);
    IPlatformStrategy* strategy = BluetoothManager::instance().platformStrategy();
    DevicePairer pairer(strategy);

    bool success = pairer.pairDevice(mac, PIN_CODE);
    
    out << "\n--- Pairing Result ---\n";
    if (success) {
        out << "SUCCESS: Device paired!\n";
    } else {
        out << "FAILED: Pairing failed!\n";
    }
    out.flush();
    
    return success ? 0 : 1;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("bluetooth-device-pairing");
    QCoreApplication::setApplicationVersion("1.0.0");

    QTextStream err(stderr);

    try {
        BluetoothManager::instance().initialize();

        runDiscover();

        runPair();

        return 0;

    } catch (const AppException& e) {
        err << "Error: " << e.message() << "\n";
        err.flush();
        return 1;
    } catch (const std::exception& e) {
        err << "Unexpected error: " << e.what() << "\n";
        err.flush();
        return 1;
    }
}
