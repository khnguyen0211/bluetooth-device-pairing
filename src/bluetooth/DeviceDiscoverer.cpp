#include "DeviceDiscoverer.h"

DeviceDiscoverer::DeviceDiscoverer(IPlatformStrategy* strategy)
    : m_strategy(strategy)
{
}

QList<BluetoothDevice> DeviceDiscoverer::discoverDevices(const DiscoveryTime& time)
{
    return m_strategy->discoverDevices(time.seconds());
}
