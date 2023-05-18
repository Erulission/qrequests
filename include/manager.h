#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QQueue>

class QNetworkReply;
class QNetworkAccessManager;

namespace Requests {
class IRequest;
class ITokenProvider;
class Manager : public QObject
{
    Q_OBJECT
public:
    static constexpr int MAX_ACTIVE_REQUEST = 3;
    explicit Manager(std::shared_ptr<ITokenProvider> tokenProvider, QObject *parent = nullptr);

    void send(std::shared_ptr<IRequest> request);

    int maxActiveRequestCount() const;
    void setMaxActiveRequestCount(int newMaxActiveRequestCount);

protected:
    QNetworkAccessManager *m_networkManager = nullptr;
    void removeActiveRequest(QObject *sender);
private slots:
    void handleFinishedRequest();
    void handleAuthorizationError();
    void handleTokenRefreshed();
private:
    QList<std::shared_ptr<IRequest>> m_activeRequests;
    QQueue<std::shared_ptr<Requests::IRequest>> m_pendingRequests;
    QQueue<std::shared_ptr<Requests::IRequest>> m_unauthorizedRequests;
    std::shared_ptr<ITokenProvider> m_tokenProvider;

    int m_maxActiveRequestCount;
};
}

#endif // MANAGER_H
