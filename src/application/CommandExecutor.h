#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include <QString>
#include <QVariantMap>

class CommandExecutor {
public:
    static int execute(const QString& command, const QVariantMap& options);
};

#endif // COMMANDEXECUTOR_H
