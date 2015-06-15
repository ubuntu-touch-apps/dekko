#ifndef UNITYNETWORKWATCHER_H
#define UNITYNETWORKWATCHER_H

#include <QObject>
#include <QScopedPointer>
#include <ubuntu/connectivity/networking-status.h>
#include "Imap/Model/Model.h"
#include "Imap/Model/NetworkWatcher.h"
#include "Imap/Model/NetworkPolicy.h"
namespace Imap {
namespace Mailbox {
using ubuntu::connectivity::NetworkingStatus;
class UnityNetworkWatcher : public NetworkWatcher
{
    Q_OBJECT
public:
    UnityNetworkWatcher(QObject *parent, Imap::Mailbox::Model *model);
    virtual NetworkPolicy effectiveNetworkPolicy() const;

public slots:
    void reconnectModelNetwork();
    void onUnityNetworkStatusChanged(NetworkingStatus::Status status);

protected:
    virtual void setDesiredNetworkPolicy(const NetworkPolicy policy);

private:
    QScopedPointer<NetworkingStatus> m_netStatus;
    bool isOnline() const;
};
}
}
#endif // UNITYNETWORKWATCHER_H
