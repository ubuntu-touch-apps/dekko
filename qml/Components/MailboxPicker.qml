import QtQuick 2.3
import Ubuntu.Components 1.1
import "../MailboxView"
import DekkoCore 0.2

Item {
    id: p
    signal mailboxSelected(string mboxName)

    property Account account
    property var picker
    function pick() {
        picker = pickerPage.createObject(p.parent, { account: account });
        picker.mailboxSelected.connect(mailboxSelected);
        pageStack.push(picker);
    }

    Component {
        id: pickerPage
        Page {
            property Account account

            DekkoHeader {
                id: header
                title: qsTr("Select a mailbox")
                backAction: Action {
                    id: backAction
                    enabled: true
                    iconName: "back"
                    onTriggered: pageStack.pop()
                }
                enableSearching: true
                onSearchActivated: {
                    searchFilterModel.queryString = searchString
                    searchFilterModel.invalidate()
                }
                onSearchCanceled: {
                    searchFilterModel.queryString = ""
                    searchFilterModel.invalidate()
                }
            }
            flickable: null
            signal mailboxSelected(string mailbox)
            onMailboxSelected: {
                pageStack.pop()
            }

            MailboxFilterModel {
                id: filterModel
                filterRole: MailboxFilterModel.FILTER_NONE
                sourceModel: account.mailboxModel
            }

            FlatteningProxyModel {
                id: flatModel
                sourceModel: filterModel
            }
            MailboxFilterModel {
                id: searchFilterModel
                sourceModel: header.isSearchMode ? flatModel : null
                filterRole: MailboxFilterModel.FILTER_QUERY
            }
            MailboxListView {
                id: mboxListView
                property bool isSearchMode: header.isSearchMode
                isPickMode: true
                lockDelegate: true
                anchors.fill: parent
                anchors.topMargin: units.gu(7)
                model: header.isSearchMode ? searchFilterModel : flatModel
                onOpenMailbox: {
                    mailboxSelected(mboxName)
                }
            }
        }
    }
}
