#include "WindowsStrategy.h"
#include "utils/AppException.h"
#include <QTimer>
#include <QTextStream>
#include <QCoreApplication>

WindowsStrategy::WindowsStrategy()
    : m_discoveryAgent(nullptr)
    , m_localDevice(nullptr)
    , m_eventLoop(nullptr)
    , m_pairingSuccess(false)
    , m_pairingFinished(false)
    , m_discoveryFinished(false)
{
    setupDiscoveryAgent();
    setupLocalDevice();
}

WindowsStrategy::~WindowsStrategy()
{
    delete m_discoveryAgent;
    delete m_localDevice;
    delete m_eventLoop;
}

void WindowsStrategy::setupDiscoveryAgent()
{
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_discoveryAgent->setLowEnergyDiscoveryTimeout(10000);
    
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &WindowsStrategy::onDeviceDiscovered);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &WindowsStrategy::onDiscoveryFinished);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
            this, &WindowsStrategy::onDiscoveryError);
}

void WindowsStrategy::setupLocalDevice()
{
    m_localDevice = new QBluetoothLocalDevice(this);
    
    if (!m_localDevice->isValid()) {
        throw AppException("No Bluetooth adapter found");
    }
    
    if (m_localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        m_localDevice->powerOn();
    }
    
    connect(m_localDevice, &QBluetoothLocalDevice::pairingFinished,
            this, &WindowsStrategy::onPairingFinished);
    connect(m_localDevice, &QBluetoothLocalDevice::errorOccurred,
            this, &WindowsStrategy::onPairingError);
}

QList<BluetoothDevice> WindowsStrategy::discoverDevices(int timeoutSeconds)
{
    QTextStream out(stdout);
    out << "Discovering devices for " << timeoutSeconds << " seconds...\n";
    out.flush();

    m_discoveredDevices.clear();
    m_discoveryFinished = false;

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (m_discoveryAgent->isActive()) {
            m_discoveryAgent->stop();
        }
        m_discoveryFinished = true;
    });

    m_discoveryAgent->start();
    timer->start(timeoutSeconds * 1000);

    while (!m_discoveryFinished) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    timer->stop();
    timer->deleteLater();

    return m_discoveredDevices;
}

bool WindowsStrategy::pairDevice(const MacAddress& mac, const QString& pinCode)
{
    QTextStream out(stdout);
    m_pairingSuccess = false;
    m_pairingFinished = false;
    m_currentPinCode = pinCode;

    QBluetoothAddress address = mac.toQBluetoothAddress();
    
    out << "Starting pairing with device: " << mac.toString() << "\n";
    out << "PIN Code: " << pinCode << "\n";
    out.flush();

    // Request pairing
    m_localDevice->requestPairing(address, QBluetoothLocalDevice::Paired);

    // Wait for pairing to complete with timeout
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        QTextStream out(stdout);
        out << "Pairing timeout (30s)\n";
        out.flush();
        m_pairingFinished = true;
    });
    timer->start(30000);

    out << "Waiting for pairing response...\n";
    out.flush();

    while (!m_pairingFinished) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    timer->stop();
    timer->deleteLater();

    return m_pairingSuccess;
}

BluetoothDevice* WindowsStrategy::findDeviceByMac(const MacAddress& mac)
{
    // First, do a quick discovery to find the device
    QList<BluetoothDevice> devices = discoverDevices(5);
    
    for (auto& device : devices) {
        if (device.macAddress() == mac) {
            return new BluetoothDevice(device);
        }
    }
    
    return nullptr;
}

void WindowsStrategy::onDeviceDiscovered(const QBluetoothDeviceInfo& info)
{
    if (info.address().isNull()) {
        return;
    }
    
    try {
        m_discoveredDevices.append(convertToBluetoothDevice(info));
    } catch (const std::exception&) {
        // Skip devices that fail to convert
    }
}

void WindowsStrategy::onDiscoveryFinished()
{
    m_discoveryFinished = true;
}

void WindowsStrategy::onDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    Q_UNUSED(error);
    m_discoveryFinished = true;
}

void WindowsStrategy::onPairingFinished(const QBluetoothAddress& address, 
                                        QBluetoothLocalDevice::Pairing pairing)
{
    QTextStream out(stdout);
    
    out << "Pairing finished for: " << address.toString() << "\n";
    
    if (pairing == QBluetoothLocalDevice::Paired || 
        pairing == QBluetoothLocalDevice::AuthorizedPaired) {
        m_pairingSuccess = true;
        out << "Status: SUCCESS - Device paired!\n";
    } else {
        m_pairingSuccess = false;
        out << "Status: FAILED - Pairing was cancelled or rejected\n";
    }
    out.flush();
    
    m_pairingFinished = true;
}

void WindowsStrategy::onPairingError(QBluetoothLocalDevice::Error error)
{
    QTextStream out(stderr);
    out << "Pairing error occurred!\n";
    out << "Error code: " << static_cast<int>(error) << "\n";
    
    switch (error) {
        case QBluetoothLocalDevice::PairingError:
            out << "Error type: Pairing error\n";
            break;
        case QBluetoothLocalDevice::UnknownError:
            out << "Error type: Unknown error\n";
            break;
        default:
            out << "Error type: Other\n";
            break;
    }
    out.flush();
    
    m_pairingSuccess = false;
    m_pairingFinished = true;
}

BluetoothDevice WindowsStrategy::convertToBluetoothDevice(const QBluetoothDeviceInfo& info)
{
    QString name = info.name();
    MacAddress mac(info.address().toString());
    
    DeviceType type = (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
                      ? DeviceType::LowEnergy
                      : DeviceType::Classic;
    
    ConnectionStatus status = getConnectionStatus(info);
    
    return BluetoothDevice(name, mac, type, status);
}

ConnectionStatus WindowsStrategy::getConnectionStatus(const QBluetoothDeviceInfo& info)
{
    QBluetoothLocalDevice::Pairing pairingStatus = 
        m_localDevice->pairingStatus(info.address());
    
    switch (pairingStatus) {
        case QBluetoothLocalDevice::Unpaired:
            return ConnectionStatus::NotPaired;
        case QBluetoothLocalDevice::Paired:
        case QBluetoothLocalDevice::AuthorizedPaired:
            return ConnectionStatus::Paired;
        default:
            return ConnectionStatus::NotPaired;
    }
}
