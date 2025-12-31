#ifndef DEVICEPAIRER_H
#define DEVICEPAIRER_H

#include "platform/IPlatformStrategy.h"
#include "models/MacAddress.h"
#include <QString>

class DevicePairer {
public:
    explicit DevicePairer(IPlatformStrategy* strategy);

    bool pairDevice(const MacAddress& mac, const QString& pinCode);

private:
    IPlatformStrategy* m_strategy;
};

#endif // DEVICEPAIRER_H
