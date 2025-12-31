#ifndef WINDOWSSTRATEGY_H
#define WINDOWSSTRATEGY_H

#include "IPlatformStrategy.h"
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QObject>
#include <QEventLoop>

class WindowsStrategy : public QObject, public IPlatformStrategy {
    Q_OBJECT

public:
    WindowsStrategy();
    ~WindowsStrategy() override;

    QList<BluetoothDevice> discoverDevices(int timeoutSeconds) override;
    bool pairDevice(const MacAddress& mac, const QString& pinCode) override;
    BluetoothDevice* findDeviceByMac(const MacAddress& mac) override;

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo& info);
    void onDiscoveryFinished();
    void onDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error);
    void onPairingFinished(const QBluetoothAddress& address, QBluetoothLocalDevice::Pairing pairing);
    void onPairingError(QBluetoothLocalDevice::Error error);

private:
    QBluetoothDeviceDiscoveryAgent* m_discoveryAgent;
    QBluetoothLocalDevice* m_localDevice;
    QList<BluetoothDevice> m_discoveredDevices;
    QEventLoop* m_eventLoop;
    bool m_pairingSuccess;
    bool m_pairingFinished;
    bool m_discoveryFinished;
    QString m_currentPinCode;

    void setupDiscoveryAgent();
    void setupLocalDevice();
    BluetoothDevice convertToBluetoothDevice(const QBluetoothDeviceInfo& info);
    ConnectionStatus getConnectionStatus(const QBluetoothDeviceInfo& info);
};

#endif // WINDOWSSTRATEGY_H
