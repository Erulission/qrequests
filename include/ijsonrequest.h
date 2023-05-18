#ifndef IJSONREQUEST_H
#define IJSONREQUEST_H

#include <iauthorizablerequest.h>
#include <QJsonDocument>

namespace Requests {
class IJsonRequest : public IAuthorizableRequest
{
public:
    IJsonRequest(std::shared_ptr<Requests::Manager> manager,
                 const QString& baseUrl, const QByteArray& authorizationToken);

    // IRequest interface
protected:
    virtual void handleSuccess() override;
    virtual void handleError() override;
    virtual QNetworkReply *processRequest() override;
    virtual bool prepareRequest() override;

    virtual void parseErrorResponse();
    virtual void parseSuccessResponse() = 0;

    bool parseJson();

    QJsonDocument m_replyDocument;
};
}
#endif // IJSONREQUEST_H
