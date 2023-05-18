#ifndef AUTHORIZABLE_H
#define AUTHORIZABLE_H

namespace Requests {
class Authorizable
{
public:
    virtual QByteArray authorizationToken() const = 0;
    virtual QByteArray authorizationHeaderName() const = 0;
    virtual void setAuthorizationToken(const QByteArray &token) = 0;
signals:
    void authorizationError();
};
}

#endif // AUTHORIZABLE_H
