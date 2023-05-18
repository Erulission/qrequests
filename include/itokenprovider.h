#ifndef ITOKENPROVIDER_H
#define ITOKENPROVIDER_H

#include <QObject>

namespace Requests {
class ITokenProvider : public QObject {
    Q_OBJECT
public:
    virtual bool isRefreshing() = 0;
    virtual QByteArray authorizationToken() = 0;
    virtual void refreshToken() = 0;
    virtual QDateTime lastRefresh() = 0;
signals:
    void refreshed();
    void error();
};
}

#endif // ITOKENPROVIDER_H
