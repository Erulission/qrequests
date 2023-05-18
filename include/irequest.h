#ifndef IREQUEST_H
#define IREQUEST_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace Requests {

class Manager;
class IRequest : public QObject
{
    Q_OBJECT
public:
    explicit IRequest(std::shared_ptr<Requests::Manager> manager,
        const QString& baseUrl);

    virtual void setBaseUrl(const QString& baseUrl);
    virtual bool isSuccess() const;
    virtual bool isAuthorizable() const;

    int retryInterval() const;
    void setRetryInterval(int intervalInMs);
    int retryAttemptsAmount() const;
    void setRetryAttemptsAmount(int attemptsAmount);

    QString url() const;
    QString networkErrorString() const;

    QNetworkRequest request() const;

signals:
    void finished();
    void retried();
    void failed();
    void aborted();

public slots:
    virtual void send(QNetworkAccessManager* manager);

protected:
    QString m_baseUrl;
    QNetworkRequest m_request;
    QNetworkReply* m_reply = nullptr;
    QNetworkAccessManager* m_networkManager = nullptr;
    std::shared_ptr<Manager> m_requestManager;
    bool m_success = false;
    bool m_ignoreError = false;
    int m_retryIntervalMs = 0;
    int m_retryAttemptsAmount = 0;
    int m_retryAttemptsMadeAmount = 0;

    virtual void handleSuccess() = 0;
    virtual void handleError() = 0;

    void markRequestAsFailed();

    virtual QNetworkReply * processRequest() = 0;

    virtual void processReply();
    virtual bool prepareRequest() = 0;

    virtual void retry();
    virtual void abort();

    int retryAttemptsMadeAmount() const;
    bool allRetryAttemptsUsed() const;
    void disableRetry();

private:
    void sendRequest();
    void processFinishedReply();
    void complete();
};
}
#endif // IREQUEST_H
