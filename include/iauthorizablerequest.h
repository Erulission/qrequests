#ifndef IAUTHORIZABLEREQUEST_H
#define IAUTHORIZABLEREQUEST_H

#include "irequest.h"

namespace Requests {
class IAuthorizableRequest : public IRequest
{
    Q_OBJECT
public:
    static constexpr int MAX_AUTHORIZATION_ERROR_COUNT = 2;

    IAuthorizableRequest(std::shared_ptr<Requests::Manager> manager,
        const QString& baseUrl, const QByteArray& authorizationToken);

    virtual QByteArray authorizationToken() const;
    virtual QByteArray authorizationHeaderName() const;
    virtual void setAuthorizationToken(const QByteArray &token);

protected:    
    virtual void handleError() override;
    virtual bool prepareRequest() override;
    virtual void prepareAuthorizationHeader();
    virtual bool allAuthorizationErrorRetriesUsed();

signals:
    void authorizationError(QPrivateSignal);

private:
    QByteArray m_authorizationToken;
    int m_authorizationErrorCount;
};
}

#endif // IAUTHORIZABLEREQUEST_H
