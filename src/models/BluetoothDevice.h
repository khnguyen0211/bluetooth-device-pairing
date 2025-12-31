#ifndef BLUETOOTHDEVICE_H
#define BLUETOOTHDEVICE_H

#include "MacAddress.h"
#include <QString>

enum class DeviceType {
    Classic,
    LowEnergy
};

enum class ConnectionStatus {
    NotPaired,
    Paired,
    Connected
};

class BluetoothDevice {
public:
    BluetoothDevice(const QString& name,
                    const MacAddress& mac,
                    DeviceType type,
                    ConnectionStatus status);

    QString name() const;
    MacAddress macAddress() const;
    DeviceType deviceType() const;
    ConnectionStatus connectionStatus() const;

    QString toString() const;

private:
    QString m_name;
    MacAddress m_macAddress;
    DeviceType m_deviceType;
    ConnectionStatus m_connectionStatus;
};

#endif // BLUETOOTHDEVICE_H
