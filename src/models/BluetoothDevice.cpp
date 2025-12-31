#include "BluetoothDevice.h"

BluetoothDevice::BluetoothDevice(const QString& name,
                                 const MacAddress& mac,
                                 DeviceType type,
                                 ConnectionStatus status)
    : m_name(name)
    , m_macAddress(mac)
    , m_deviceType(type)
    , m_connectionStatus(status)
{
}

QString BluetoothDevice::name() const
{
    return m_name;
}

MacAddress BluetoothDevice::macAddress() const
{
    return m_macAddress;
}

DeviceType BluetoothDevice::deviceType() const
{
    return m_deviceType;
}

ConnectionStatus BluetoothDevice::connectionStatus() const
{
    return m_connectionStatus;
}

QString BluetoothDevice::toString() const
{
    QString typeStr = (m_deviceType == DeviceType::LowEnergy) ? "BLE" : "Classic";
    QString statusStr;
    switch (m_connectionStatus) {
        case ConnectionStatus::NotPaired: statusStr = "NotPaired"; break;
        case ConnectionStatus::Paired: statusStr = "Paired"; break;
        case ConnectionStatus::Connected: statusStr = "Connected"; break;
    }
    return QString("name:'%1' mac:'%2' type:'%3' status:'%4'")
           .arg(m_name)
           .arg(m_macAddress.toString())
           .arg(typeStr)
           .arg(statusStr);
}
