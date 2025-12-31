#ifndef IPLATFORMSTRATEGY_H
#define IPLATFORMSTRATEGY_H

#include "models/BluetoothDevice.h"
#include "models/MacAddress.h"
#include <QList>
#include <QString>

class IPlatformStrategy {
public:
    virtual ~IPlatformStrategy() = default;

    virtual QList<BluetoothDevice> discoverDevices(int timeoutSeconds) = 0;
    virtual bool pairDevice(const MacAddress& mac, const QString& pinCode) = 0;
    virtual BluetoothDevice* findDeviceByMac(const MacAddress& mac) = 0;
};

#endif // IPLATFORMSTRATEGY_H
