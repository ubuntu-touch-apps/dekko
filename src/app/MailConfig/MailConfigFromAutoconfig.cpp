/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>
 * Copyright (C) 2014-2015 Boren Zhang <bobo1993324@gmail.com>

   This file is part of Dekko email client for Ubuntu Devices/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "MailConfigFromAutoconfig.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtXml/QDomDocument>
#include "app/Settings/Settings.h"
namespace Dekko
{

MailConfigFromAutoconfig::MailConfigFromAutoconfig() {
    m_config = new MailConfig();
    m_nam = new QNetworkAccessManager();
    connect(m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

MailConfigFromAutoconfig::~MailConfigFromAutoconfig() {
    m_nam->deleteLater();
    m_config->deleteLater();
}

void MailConfigFromAutoconfig::setUrl(QString url) {
    m_url = url;
}

void MailConfigFromAutoconfig::config() {
    //https://developer.mozilla.org/en-US/docs/Mozilla/Thunderbird/Autoconfiguration
    //Given the email address "fred@example.com", Thunderbird first checks
    //<http://autoconfig.example.com/mail/config-v1.1.xml?emailaddress=fred@example.com> and then
    //<http://example.com/.well-known/autoconfig/mail/config-v1.1.xml>. Then
    //ISP database
    qDebug() << "get" << m_url;
    m_nam->get(QNetworkRequest(QUrl(m_url)));
}

//parse mail config file at https://developer.mozilla.org/en-US/docs/Mozilla/Thunderbird/Autoconfiguration/FileFormat/HowTo
void MailConfigFromAutoconfig::requestFinished(QNetworkReply * reply) {
    QVariant statusCodeV =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (redirect.isValid()) {
        qDebug() << "Redirect to" << redirect.toUrl();
        m_nam->get(QNetworkRequest(redirect.toUrl()));
        delete reply;
        return;
    }

    bool successful = true;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();  // bytes
        QString string(bytes); // string
        qDebug() << string;
        QDomDocument document;
        document.setContent(string);
        QDomNodeList incomeServerlist = document.elementsByTagName("incomingServer");
        QDomElement * incomeServer = 0;
        for (int i = 0; i < incomeServerlist.size(); i++) {
            if (incomeServerlist.at(i).attributes().namedItem("type").nodeValue() == "imap") {
                incomeServer = new QDomElement(incomeServerlist.at(i).toElement());
                break;
            }
        }
        if (incomeServer == 0) {
            qDebug() << "no incomingServer in xml file";
            successful = false;
        } else {
            m_config->setImapHost(incomeServer->elementsByTagName("hostname").at(0).firstChild().nodeValue());
            m_config->setImapPort(incomeServer->elementsByTagName("port").at(0).firstChild().nodeValue().toInt());
            QString imapSocketType = incomeServer->elementsByTagName("socketType").at(0).firstChild().nodeValue();
            if (imapSocketType == "SSL") {
                m_config->setImapMethod(Dekko::Settings::ImapSettings::ConnectionMethod::SSL);
            } else if (imapSocketType == "STARTTLS") {
                m_config->setImapMethod(Dekko::Settings::ImapSettings::ConnectionMethod::STARTTLS);
            } else {
                m_config->setImapMethod(Dekko::Settings::ImapSettings::ConnectionMethod::NONE);
            }
        }
        QDomNodeList outgoServerlist = document.elementsByTagName("outgoingServer");
        if (!outgoServerlist.isEmpty()) {
            QDomElement outgoingServer = outgoServerlist.at(0).toElement();
            m_config->setSmtpHost(outgoingServer.elementsByTagName("hostname").at(0).firstChild().nodeValue());
            m_config->setSmtpPort(outgoingServer.elementsByTagName("port").at(0).firstChild().nodeValue().toInt());
            QString smtpSocketType = outgoingServer.elementsByTagName("socketType").at(0).firstChild().nodeValue();
            if (smtpSocketType == "STARTTLS")
                m_config->setSmtpMethod(Dekko::Settings::SmtpSettings::SubmissionMethod::SMTP_STARTTLS);
            else if (smtpSocketType == "SSL")
                m_config->setSmtpMethod(Dekko::Settings::SmtpSettings::SubmissionMethod::SSMTP);
            else
                m_config->setSmtpMethod(Dekko::Settings::SmtpSettings::SubmissionMethod::SMTP);
        } else {
            qDebug() << "no outgoingServer in xml file";
            successful = false;
        }
    } else {
        successful = false;
    }
    if (successful) {
        m_config->print();
        emit complete(m_config);
    } else {
        emit failed();
    }
    delete reply;
}

}
