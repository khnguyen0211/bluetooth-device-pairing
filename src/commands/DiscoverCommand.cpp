#include "DiscoverCommand.h"
#include "bluetooth/BluetoothManager.h"
#include "bluetooth/DeviceDiscoverer.h"
#include "models/DiscoveryTime.h"
#include "models/DeviceFormatter.h"

DiscoverCommand::DiscoverCommand(int discoveryTimeSeconds)
    : m_discoveryTimeSeconds(discoveryTimeSeconds)
{
}

int DiscoverCommand::execute()
{
    DiscoveryTime time(m_discoveryTimeSeconds);
    
    IPlatformStrategy* strategy = BluetoothManager::instance().platformStrategy();
    DeviceDiscoverer discoverer(strategy);
    
    QList<BluetoothDevice> devices = discoverer.discoverDevices(time);
    DeviceFormatter::printDeviceTable(devices);
    
    return 0;
}
