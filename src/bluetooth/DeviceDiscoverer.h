#ifndef DEVICEDISCOVERER_H
#define DEVICEDISCOVERER_H

#include "platform/IPlatformStrategy.h"
#include "models/DiscoveryTime.h"
#include "models/BluetoothDevice.h"
#include <QList>

class DeviceDiscoverer {
public:
    explicit DeviceDiscoverer(IPlatformStrategy* strategy);

    QList<BluetoothDevice> discoverDevices(const DiscoveryTime& time);

private:
    IPlatformStrategy* m_strategy;
};

#endif // DEVICEDISCOVERER_H
