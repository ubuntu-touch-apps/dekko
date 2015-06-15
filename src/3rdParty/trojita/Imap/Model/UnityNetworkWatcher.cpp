#include "UnityNetworkWatcher.h"

namespace Imap {
namespace Mailbox {
using ubuntu::connectivity::NetworkingStatus;

UnityNetworkWatcher::UnityNetworkWatcher(QObject *parent, Model *model) :
    NetworkWatcher(parent, model), m_netStatus(new NetworkingStatus())
{
    QObject::connect(m_netStatus.data(),
                     &NetworkingStatus::statusChanged,
                     [=](NetworkingStatus::Status value)
    {
        onUnityNetworkStatusChanged(value);
        switch(value) {
        case NetworkingStatus::Offline:
            qDebug() << "System networking status changed to: Offline";
            break;
        case NetworkingStatus::Connecting:
            qDebug() << "System networking status changed to: Connecting";
            break;
        case NetworkingStatus::Online:
            qDebug() << "System networking status changed to: Online";
            break;
        }
    });
    // normal getter
    if (m_netStatus->limitations().isEmpty())
        qDebug() << "No limitations";
    // Subscribe to limitation changes
    QObject::connect(m_netStatus.data(),
                     &NetworkingStatus::limitationsChanged,
                     [this](){
        if (m_netStatus->limitations().isEmpty()) {
            qDebug() << "No limitations.";
            return;
        }
        qDebug() << "Limitations:";
        if (m_netStatus->limitations().contains(NetworkingStatus::Limitations::Bandwith)) {
            qDebug() << "    - Bandwith";
        }
    });
}

Imap::Mailbox::NetworkPolicy UnityNetworkWatcher::effectiveNetworkPolicy() const
{
    return m_model->networkPolicy();
}

void UnityNetworkWatcher::reconnectModelNetwork()
{
    switch (m_netStatus->status()) {
    case NetworkingStatus::Status::Offline:
    case NetworkingStatus::Status::Connecting:
        m_model->setNetworkPolicy(NETWORK_OFFLINE);
        break;
    case NetworkingStatus::Status::Online:
        m_model->setNetworkPolicy(m_desiredPolicy);
        break;
    }
}

void UnityNetworkWatcher::onUnityNetworkStatusChanged(NetworkingStatus::Status status)
{
    switch (status) {
    case NetworkingStatus::Status::Offline:
    case NetworkingStatus::Status::Connecting:
        m_model->setNetworkPolicy(NETWORK_OFFLINE);
        break;
    case NetworkingStatus::Status::Online:
        qDebug() << "onUnityNetworkStatusChanged: WE ARE ONLINE!!! ";
        m_model->logTrace(0, Common::LOG_OTHER, QLatin1String("Network Session"), QLatin1String("System is back online"));
        setDesiredNetworkPolicy(m_desiredPolicy);
        break;
    }
}

void UnityNetworkWatcher::setDesiredNetworkPolicy(const NetworkPolicy policy)
{
    if (policy != m_desiredPolicy) {
        m_desiredPolicy = policy;
    }
    // If the models policy is offline but the network status has just come online
    // then set model to new policy
    if (m_model->networkPolicy() == NETWORK_OFFLINE && policy != NETWORK_OFFLINE) {
        if (isOnline()) {
            reconnectModelNetwork();
        }
    } else if (m_model->networkPolicy() != NETWORK_OFFLINE && policy == NETWORK_OFFLINE) {
        m_model->setNetworkPolicy(NETWORK_OFFLINE);
    } else {
        m_model->setNetworkPolicy(m_desiredPolicy);
    }
}

bool UnityNetworkWatcher::isOnline() const
{
    return m_netStatus->status() == NetworkingStatus::Status::Online;
}
}
}
