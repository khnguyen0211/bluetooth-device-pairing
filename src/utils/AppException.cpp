#include "AppException.h"

AppException::AppException(const QString& message)
    : m_message(message)
{
}

const char* AppException::what() const noexcept
{
    m_whatBuffer = m_message.toUtf8();
    return m_whatBuffer.constData();
}

QString AppException::message() const
{
    return m_message;
}
