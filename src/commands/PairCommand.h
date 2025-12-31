#ifndef PAIRCOMMAND_H
#define PAIRCOMMAND_H

#include "ICommand.h"
#include <QString>

class PairCommand : public ICommand {
public:
    explicit PairCommand(const QString& macAddress, const QString& pinCode);
    int execute() override;

private:
    QString m_macAddress;
    QString m_pinCode;
};

#endif // PAIRCOMMAND_H
