#ifndef DEVICEFORMATTER_H
#define DEVICEFORMATTER_H

#include "BluetoothDevice.h"
#include <QList>

class DeviceFormatter {
public:
    static void printDeviceTable(const QList<BluetoothDevice>& devices);

private:
    static QString formatDeviceType(DeviceType type);
    static QString formatConnectionStatus(ConnectionStatus status);
    static QString formatDeviceName(const QString& name);
};

#endif // DEVICEFORMATTER_H
