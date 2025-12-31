#ifndef MACADDRESS_H
#define MACADDRESS_H

#include <QString>
#include <QBluetoothAddress>

class MacAddress {
public:
    explicit MacAddress(const QString& address);

    QString toString() const;
    quint64 toUInt64() const;
    QBluetoothAddress toQBluetoothAddress() const;
    bool isValid() const;

    bool operator==(const MacAddress& other) const;
    bool operator!=(const MacAddress& other) const;

private:
    QString m_address;
    quint64 m_rawAddress;
    bool m_isValid;

    static bool validate(const QString& address);
    static quint64 parse(const QString& address);
};

#endif // MACADDRESS_H
