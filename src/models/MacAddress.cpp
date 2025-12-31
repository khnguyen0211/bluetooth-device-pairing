#include "MacAddress.h"
#include "utils/AppException.h"
#include <QRegularExpression>

MacAddress::MacAddress(const QString& address)
    : m_address(address.toUpper())
    , m_rawAddress(0)
    , m_isValid(false)
{
    if (!validate(address)) {
        throw AppException(QString("MAC address '%1' is not a valid mac address").arg(address));
    }
    m_isValid = true;
    m_rawAddress = parse(m_address);
}

QString MacAddress::toString() const
{
    return m_address;
}

quint64 MacAddress::toUInt64() const
{
    return m_rawAddress;
}

QBluetoothAddress MacAddress::toQBluetoothAddress() const
{
    return QBluetoothAddress(m_rawAddress);
}

bool MacAddress::isValid() const
{
    return m_isValid;
}

bool MacAddress::operator==(const MacAddress& other) const
{
    return m_rawAddress == other.m_rawAddress;
}

bool MacAddress::operator!=(const MacAddress& other) const
{
    return !(*this == other);
}

bool MacAddress::validate(const QString& address)
{
    static QRegularExpression regex("^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$");
    return regex.match(address).hasMatch();
}

quint64 MacAddress::parse(const QString& address)
{
    QString cleanAddress = address;
    cleanAddress.remove(':');
    bool ok;
    quint64 result = cleanAddress.toULongLong(&ok, 16);
    return result;
}
