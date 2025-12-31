#include "CommandExecutor.h"
#include "commands/DiscoverCommand.h"
#include "commands/PairCommand.h"
#include "utils/AppException.h"

int CommandExecutor::execute(const QString& command, const QVariantMap& options)
{
    if (command == "discover") {
        int discoveryTime = options.value("discovery-time").toInt();
        DiscoverCommand cmd(discoveryTime);
        return cmd.execute();
    } 
    else if (command == "pair-by-mac") {
        QString mac = options.value("mac").toString();
        QString pin = options.value("pin").toString();
        PairCommand cmd(mac, pin);
        return cmd.execute();
    }
    else {
        throw AppException(QString("Unknown command: %1").arg(command));
    }
}
