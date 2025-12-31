#ifndef APPEXCEPTION_H
#define APPEXCEPTION_H

#include <QString>
#include <exception>

class AppException : public std::exception {
public:
    explicit AppException(const QString& message);
    const char* what() const noexcept override;
    QString message() const;

private:
    QString m_message;
    mutable QByteArray m_whatBuffer;
};

#endif // APPEXCEPTION_H
