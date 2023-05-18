#include "iauthorizablerequest.h"
#include <QNetworkRequest>
#include <QNetworkReply>

namespace Requests {

IAuthorizableRequest::IAuthorizableRequest(std::shared_ptr<Manager> manager,
    const QString &baseUrl,
    const QByteArray &authorizationToken)
    : IRequest(manager, baseUrl),
      m_authorizationToken(authorizationToken),
      m_authorizationErrorCount(0)
{
}


QByteArray IAuthorizableRequest::authorizationToken() const
{
    return m_authorizationToken;
}

QByteArray IAuthorizableRequest::authorizationHeaderName() const
{
    return QByteArrayLiteral("Authorization");
}

void IAuthorizableRequest::setAuthorizationToken(const QByteArray &token)
{
    m_authorizationToken = token;
}

void IAuthorizableRequest::handleError()
{
    if (m_reply->error() == QNetworkReply::AuthenticationRequiredError) {
        m_authorizationErrorCount++;

        if (allAuthorizationErrorRetriesUsed()) {
            return;
        }

        m_ignoreError = true;
        emit authorizationError(QPrivateSignal());
    }
}

bool IAuthorizableRequest::prepareRequest()
{
    prepareAuthorizationHeader();
    return true;
}

void IAuthorizableRequest::prepareAuthorizationHeader()
{
    if (authorizationToken().isEmpty()) {
        return;
    }

    m_request.setRawHeader(authorizationHeaderName(), authorizationToken());
}

bool IAuthorizableRequest::allAuthorizationErrorRetriesUsed()
{
    return m_authorizationErrorCount >= MAX_AUTHORIZATION_ERROR_COUNT;
}

}
