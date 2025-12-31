#include "CommandLineParser.h"
#include "models/DiscoveryTime.h"
#include <QTextStream>

CommandLineParser::ParseResult CommandLineParser::parse(const QStringList& arguments)
{
    ParseResult result;
    result.isValid = false;

    // Skip program name (first argument)
    if (arguments.size() < 2) {
        result.errorMessage = "No command specified";
        return result;
    }

    QString command = arguments[1];
    QStringList args = arguments.mid(2);

    if (command == "discover") {
        return parseDiscoverCommand(args);
    } else if (command == "pair-by-mac") {
        return parsePairByMacCommand(args);
    } else {
        result.errorMessage = QString("Unknown command: %1").arg(command);
        return result;
    }
}

CommandLineParser::ParseResult CommandLineParser::parseDiscoverCommand(const QStringList& args)
{
    ParseResult result;
    result.command = "discover";
    result.isValid = true;
    result.options["discovery-time"] = DiscoveryTime::DEFAULT_SECONDS;

    for (int i = 0; i < args.size(); ++i) {
        if (args[i] == "--discovery-time" && i + 1 < args.size()) {
            bool ok;
            int time = args[i + 1].toInt(&ok);
            if (!ok) {
                result.isValid = false;
                result.errorMessage = QString("Invalid discovery time: %1").arg(args[i + 1]);
                return result;
            }
            result.options["discovery-time"] = time;
            ++i;
        }
    }

    return result;
}

CommandLineParser::ParseResult CommandLineParser::parsePairByMacCommand(const QStringList& args)
{
    ParseResult result;
    result.command = "pair-by-mac";
    result.isValid = false;
    result.options["pin"] = QString("0000");

    bool hasMac = false;

    for (int i = 0; i < args.size(); ++i) {
        if (args[i] == "--mac" && i + 1 < args.size()) {
            result.options["mac"] = args[i + 1];
            hasMac = true;
            ++i;
        } else if (args[i] == "--pin" && i + 1 < args.size()) {
            result.options["pin"] = args[i + 1];
            ++i;
        }
    }

    if (!hasMac) {
        result.errorMessage = "Missing required parameter: --mac";
        return result;
    }

    result.isValid = true;
    return result;
}

void CommandLineParser::printUsage()
{
    QTextStream out(stdout);
    out << "Bluetooth Device Pairing Tool\n";
    out << "\n";
    out << "Usage:\n";
    out << "  bluetooth-device-pairing <command> [options]\n";
    out << "\n";
    out << "Commands:\n";
    out << "  discover        Discover nearby Bluetooth devices\n";
    out << "  pair-by-mac     Pair a device using its MAC address\n";
    out << "\n";
    out << "Options for 'discover':\n";
    out << "  --discovery-time <seconds>  How long to search for devices (1-30, default: 10)\n";
    out << "\n";
    out << "Options for 'pair-by-mac':\n";
    out << "  --mac <address>   MAC address of the device (required, format: XX:XX:XX:XX:XX:XX)\n";
    out << "  --pin <code>      PIN code for pairing (default: 0000)\n";
    out << "\n";
    out << "Examples:\n";
    out << "  bluetooth-device-pairing discover\n";
    out << "  bluetooth-device-pairing discover --discovery-time 15\n";
    out << "  bluetooth-device-pairing pair-by-mac --mac AA:BB:CC:DD:EE:FF\n";
    out << "  bluetooth-device-pairing pair-by-mac --mac AA:BB:CC:DD:EE:FF --pin 1234\n";
    out.flush();
}
