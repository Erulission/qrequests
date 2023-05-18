#include "../include/ijsonrequest.h"
namespace Requests {
IJsonRequest::IJsonRequest(std::shared_ptr<Requests::Manager> manager,
                           const QString& baseUrl, const QByteArray& authorizationToken)
    : IAuthorizableRequest(manager, baseUrl, authorizationToken)
{

}

void IJsonRequest::handleSuccess()
{
    if (parseJson()) {
        parseSuccessResponse();
    } else {
        parseErrorResponse();
    }
}

void IJsonRequest::handleError()
{
    IAuthorizableRequest::handleError();
    parseJson();
    parseErrorResponse();
}

QNetworkReply *IJsonRequest::processRequest()
{
    parseJson();
}

bool IJsonRequest::prepareRequest()
{
    m_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_request.setRawHeader("Accept", "application/json");
    m_request.setRawHeader("Accept-Encoding", "application/json");

    return IAuthorizableRequest::prepareRequest();
}

void IJsonRequest::parseErrorResponse()
{
    if (m_reply->error() == QNetworkReply::AuthenticationRequiredError && !allAuthorizationErrorRetriesUsed()) {
        qDebug() << Q_FUNC_INFO << "Authentication error";
        return;
    }
    markRequestAsFailed();
}

bool IJsonRequest::parseJson()
{
    const QByteArray jsonByteArray = m_reply->readAll();

    if (jsonByteArray.isEmpty()) {
        return false;
    }

    QJsonParseError parseError;
    m_replyDocument = QJsonDocument::fromJson(jsonByteArray, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return false;
    }

    return true;
}
}
