#include "manager.h"

#include <QNetworkAccessManager>

#include "irequest.h"
#include "itokenprovider.h"
#include "iauthorizablerequest.h"

namespace Requests {

Manager::Manager(std::shared_ptr<ITokenProvider> tokenProvider, QObject *parent)
    : QObject{parent},
      m_tokenProvider{tokenProvider},
      m_maxActiveRequestCount{MAX_ACTIVE_REQUEST}
{
    m_networkManager = new QNetworkAccessManager(this);

    if (m_tokenProvider) {
        connect(tokenProvider.get(), &ITokenProvider::refreshed, this, &Manager::handleTokenRefreshed);
    }
}

void Manager::send(std::shared_ptr<IRequest> request)
{
    if (m_activeRequests.size() < m_maxActiveRequestCount) {
        m_activeRequests.push_back(request);

        request->send(m_networkManager);
        connect(request.get(), &IRequest::finished, this, &Manager::handleFinishedRequest);

        if (auto authorizable = qobject_cast<Requests::IAuthorizableRequest*>(request.get());
            authorizable) {
            connect(authorizable, &IAuthorizableRequest::authorizationError, this, &Manager::handleAuthorizationError);
        }
    } else {
        m_pendingRequests.enqueue(request);
    }
}

int Manager::maxActiveRequestCount() const
{
    return m_maxActiveRequestCount;
}

void Manager::setMaxActiveRequestCount(int newMaxActiveRequestCount)
{
    m_maxActiveRequestCount = newMaxActiveRequestCount;
}

void Manager::removeActiveRequest(QObject *sender)
{
    if (!sender) {
        return;
    }

    for (const auto &request : m_activeRequests) {
        if (request.get() == sender) {
            m_activeRequests.removeOne(request);
            return;
        }
    }
}

void Manager::handleFinishedRequest()
{
    removeActiveRequest(sender());
    if (!m_pendingRequests.isEmpty())
        send(m_pendingRequests.dequeue());
}

void Manager::handleAuthorizationError()
{
    if (!m_tokenProvider) {
        return;
    }

    std::shared_ptr<IRequest> request;
    for (const auto &r : m_activeRequests) {
        if (r.get() == sender()) {
            request = r;
            break;
        }
    }

    auto authorizable = qobject_cast<Requests::IAuthorizableRequest*>(request.get());

    if (!authorizable) {
        return;
    }

    if (authorizable->authorizationToken() == m_tokenProvider->authorizationToken()) {
        m_unauthorizedRequests.enqueue(request);

        if (!m_tokenProvider->isRefreshing()) {
            m_tokenProvider->refreshToken();
        }
    } else {
        authorizable->setAuthorizationToken(m_tokenProvider->authorizationToken());
        request->send(m_networkManager);
    }
}

void Manager::handleTokenRefreshed()
{
    // Update all unauthorized requests with fresh token, and resend them
    while(!m_unauthorizedRequests.empty()) {
        auto request = m_unauthorizedRequests.dequeue();
        if (auto authorizable = dynamic_cast<Requests::IAuthorizableRequest*>(request.get());
            authorizable) {
            authorizable->setAuthorizationToken(m_tokenProvider->authorizationToken());
            request->send(m_networkManager);
        } else {
            m_activeRequests.removeOne(request);
        }
    }

    //Update pending requests if needed:
    for (auto& request : m_pendingRequests) {
        if (auto authorizable = dynamic_cast<Requests::IAuthorizableRequest*>(request.get());
            authorizable) {
            authorizable->setAuthorizationToken(m_tokenProvider->authorizationToken());
        }
    }
}
}
