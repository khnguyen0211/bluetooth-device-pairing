#include "DevicePairer.h"
#include "utils/AppException.h"
#include <QTextStream>
#include <memory>

DevicePairer::DevicePairer(IPlatformStrategy* strategy)
    : m_strategy(strategy)
{
}

bool DevicePairer::pairDevice(const MacAddress& mac, const QString& pinCode)
{
    QTextStream out(stdout);
    
    out << QString("Request to pair device [%1]\n").arg(mac.toString());
    out.flush();

    // Find device first
    std::unique_ptr<BluetoothDevice> device(m_strategy->findDeviceByMac(mac));
    
    if (!device) {
        throw AppException(QString("Device with MAC address %1 not found").arg(mac.toString()));
    }

    out << QString("Found device: %1\n").arg(device->toString());
    out.flush();

    // Check connection status
    if (device->connectionStatus() == ConnectionStatus::Connected) {
        throw AppException("Device is already connected, no need to pair");
    }

    if (device->connectionStatus() == ConnectionStatus::Paired) {
        throw AppException("Device is already paired");
    }

    // Perform pairing
    return m_strategy->pairDevice(mac, pinCode);
}
