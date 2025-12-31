#include "PairCommand.h"
#include "bluetooth/BluetoothManager.h"
#include "bluetooth/DevicePairer.h"
#include "models/MacAddress.h"
#include <QTextStream>

PairCommand::PairCommand(const QString& macAddress, const QString& pinCode)
    : m_macAddress(macAddress)
    , m_pinCode(pinCode)
{
}

int PairCommand::execute()
{
    QTextStream out(stdout);
    
    MacAddress mac(m_macAddress);
    
    IPlatformStrategy* strategy = BluetoothManager::instance().platformStrategy();
    DevicePairer pairer(strategy);
    
    bool success = pairer.pairDevice(mac, m_pinCode);
    
    if (success) {
        out << "Pairing completed successfully\n";
        out.flush();
        return 0;
    } else {
        out << "Pairing failed\n";
        out.flush();
        return 1;
    }
}
