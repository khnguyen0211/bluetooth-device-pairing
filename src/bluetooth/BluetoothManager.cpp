#include "BluetoothManager.h"
#include "platform/WindowsStrategy.h"
#include "utils/AppException.h"

BluetoothManager& BluetoothManager::instance()
{
    static BluetoothManager instance;
    return instance;
}

BluetoothManager::BluetoothManager()
    : m_platformStrategy(nullptr)
{
}

BluetoothManager::~BluetoothManager() = default;

void BluetoothManager::initialize()
{
    if (!m_platformStrategy) {
        m_platformStrategy.reset(createPlatformStrategy());
    }
}

IPlatformStrategy* BluetoothManager::platformStrategy() const
{
    return m_platformStrategy.get();
}

IPlatformStrategy* BluetoothManager::createPlatformStrategy()
{
#ifdef Q_OS_WIN
    return new WindowsStrategy();
#elif defined(Q_OS_LINUX)
    throw AppException("Linux platform is not yet supported");
#elif defined(Q_OS_MACOS)
    throw AppException("macOS platform is not yet supported");
#else
    throw AppException("Unsupported platform");
#endif
}
