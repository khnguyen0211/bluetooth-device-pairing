#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include "platform/IPlatformStrategy.h"
#include <memory>

class BluetoothManager {
public:
    static BluetoothManager& instance();

    void initialize();
    IPlatformStrategy* platformStrategy() const;

private:
    BluetoothManager();
    ~BluetoothManager();
    BluetoothManager(const BluetoothManager&) = delete;
    BluetoothManager& operator=(const BluetoothManager&) = delete;

    std::unique_ptr<IPlatformStrategy> m_platformStrategy;

    IPlatformStrategy* createPlatformStrategy();
};

#endif // BLUETOOTHMANAGER_H
