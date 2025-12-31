#ifndef DISCOVERCOMMAND_H
#define DISCOVERCOMMAND_H

#include "ICommand.h"

class DiscoverCommand : public ICommand {
public:
    explicit DiscoverCommand(int discoveryTimeSeconds);
    int execute() override;

private:
    int m_discoveryTimeSeconds;
};

#endif // DISCOVERCOMMAND_H
