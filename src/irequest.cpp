#include "irequest.h"

#include <QTimer>

namespace Requests {

IRequest::IRequest(std::shared_ptr<Manager> manager, const QString &baseUrl)
    : QObject(nullptr),
      m_baseUrl(baseUrl),
      m_requestManager{manager}
{

}

void Requests::IRequest::setBaseUrl(const QString &baseUrl)
{
    m_baseUrl = baseUrl;
}

bool IRequest::isSuccess() const
{
    return m_success;
}

bool IRequest::isAuthorizable() const
{
    return false;
}

int IRequest::retryInterval() const
{
    return m_retryIntervalMs;
}

void IRequest::setRetryInterval(int interval)
{
    m_retryIntervalMs = interval;
}

int IRequest::retryAttemptsAmount() const
{
    return m_retryAttemptsAmount;
}

void IRequest::setRetryAttemptsAmount(int attemptsAmount)
{
    m_retryAttemptsAmount = attemptsAmount;
}

QString IRequest::url() const
{
    return m_request.url().toString();
}

QString IRequest::networkErrorString() const
{
    if (m_reply) {
        return QString("%1:%2")
            .arg(QString::number(m_reply->error()), m_reply->errorString());
    }
    return QString();
}

void IRequest::send(QNetworkAccessManager* manager)
{
    if (m_reply) {
        return;
    }

    markRequestAsFailed();

    m_networkManager = manager;

    if (prepareRequest()) {
        sendRequest();
    } else {
        complete();
    }
}

QNetworkRequest IRequest::request() const
{
    return m_request;
}

void IRequest::markRequestAsFailed()
{
    m_success = false;
}

void IRequest::processReply()
{
    m_reply->setParent(this);
    connect(m_reply,
        &QNetworkReply::finished,
        this,
        &IRequest::processFinishedReply);
}

void IRequest::retry()
{
    if( allRetryAttemptsUsed() )
    {
        return;
    }

    ++m_retryAttemptsMadeAmount;
    QTimer::singleShot(m_retryIntervalMs,
        this,
        [this] {
            qDebug() << QString("Retrying request for: %0, attempt %1/%2").arg(url())
                                .arg(m_retryAttemptsMadeAmount)
                                           .arg(m_retryAttemptsAmount);
            emit retried();
            sendRequest();
        });
}

void IRequest::abort()
{
    if (m_reply){
        QNetworkReply * reply = m_reply;
        m_reply = nullptr;
        disconnect(reply, &QNetworkReply::finished, this, &IRequest::processFinishedReply);

        if (reply->isOpen()){
            reply->close();
        }

        reply->deleteLater();

        qDebug() << QString("Request for: %0 aborted").arg(url());
        emit aborted();
    }
}

int IRequest::retryAttemptsMadeAmount() const
{
    return m_retryAttemptsMadeAmount;
}

bool IRequest::allRetryAttemptsUsed() const
{
    return m_retryAttemptsAmount <= m_retryAttemptsMadeAmount;
}

void IRequest::disableRetry()
{
    setRetryAttemptsAmount(0);
}

void IRequest::sendRequest()
{
    m_ignoreError = false;
    m_reply = processRequest();

    if( !m_reply )
    {
        complete();
        return;
    }

    processReply();
}

void IRequest::processFinishedReply()
{
    if (!m_reply) {
        return;
    }

    if (m_reply->error() == QNetworkReply::NoError) {
        m_success = true;
        handleSuccess();
    } else {
        qDebug() << QString("Request for: %0 returned error %1").arg(url(), networkErrorString());
        handleError();
    }

    m_reply->deleteLater();
    m_reply = nullptr;

    if (m_ignoreError) {
        return;
    }

    if ( !m_success && !allRetryAttemptsUsed()) {
        emit failed();
        retry();
        return;
    }

    complete();
}

void IRequest::complete()
{
    emit finished();
}
}
