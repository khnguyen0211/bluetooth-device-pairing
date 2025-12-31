#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QString>
#include <QStringList>
#include <QVariantMap>

class CommandLineParser {
public:
    struct ParseResult {
        QString command;
        QVariantMap options;
        bool isValid;
        QString errorMessage;
    };

    static ParseResult parse(const QStringList& arguments);
    static void printUsage();

private:
    static ParseResult parseDiscoverCommand(const QStringList& args);
    static ParseResult parsePairByMacCommand(const QStringList& args);
};

#endif // COMMANDLINEPARSER_H
