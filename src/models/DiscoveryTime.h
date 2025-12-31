#ifndef DISCOVERYTIME_H
#define DISCOVERYTIME_H

class DiscoveryTime {
public:
    explicit DiscoveryTime(int seconds);

    int seconds() const;

    static constexpr int MIN_SECONDS = 1;
    static constexpr int MAX_SECONDS = 30;
    static constexpr int DEFAULT_SECONDS = 10;

private:
    int m_seconds;
};

#endif // DISCOVERYTIME_H
