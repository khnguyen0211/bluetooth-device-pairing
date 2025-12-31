#include "DiscoveryTime.h"
#include "utils/AppException.h"
#include <QString>

DiscoveryTime::DiscoveryTime(int seconds)
    : m_seconds(seconds)
{
    if (seconds < MIN_SECONDS || seconds > MAX_SECONDS) {
        throw AppException(QString("Discovery time should be in range [%1; %2] but was %3")
                          .arg(MIN_SECONDS)
                          .arg(MAX_SECONDS)
                          .arg(seconds));
    }
}

int DiscoveryTime::seconds() const
{
    return m_seconds;
}
