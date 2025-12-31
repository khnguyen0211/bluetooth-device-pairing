#include "DeviceFormatter.h"
#include <QTextStream>
#include <algorithm>

void DeviceFormatter::printDeviceTable(const QList<BluetoothDevice>& devices)
{
    QTextStream out(stdout);
    
    // Sort devices by name
    QList<BluetoothDevice> sortedDevices = devices;
    std::sort(sortedDevices.begin(), sortedDevices.end(),
              [](const BluetoothDevice& a, const BluetoothDevice& b) {
                  return a.name().toLower() < b.name().toLower();
              });

    out << QString("-").repeated(73) << "\n";
    
    for (const auto& device : sortedDevices) {
        QString type = formatDeviceType(device.deviceType());
        QString mac = device.macAddress().toString();
        QString status = formatConnectionStatus(device.connectionStatus());
        QString name = formatDeviceName(device.name());
        
        out << QString("|%1|%2|%3|%4|")
               .arg(type, 2)
               .arg(mac, 17)
               .arg(status, -9)
               .arg(name, -40)
            << "\n";
    }
    
    out << QString("-").repeated(73) << "\n";
    out.flush();
}

QString DeviceFormatter::formatDeviceType(DeviceType type)
{
    return (type == DeviceType::LowEnergy) ? "LE" : "";
}

QString DeviceFormatter::formatConnectionStatus(ConnectionStatus status)
{
    switch (status) {
        case ConnectionStatus::NotPaired: return "";
        case ConnectionStatus::Paired: return "Paired";
        case ConnectionStatus::Connected: return "Connected";
    }
    return "";
}

QString DeviceFormatter::formatDeviceName(const QString& name)
{
    return name.isEmpty() ? "<Unknown>" : name;
}
