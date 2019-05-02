/*
 * Copyright (C) 2008-2019 The QXmpp developers
 *
 * Author:
 *  Linus Jahn <lnj@kaidan.im>
 *
 * Source:
 *  https://github.com/qxmpp-project/qxmpp
 *
 * This file is a part of QXmpp library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifndef QXMPPMIXMANAGER_H
#define QXMPPMIXMANAGER_H

#include "QXmppClientExtension.h"

class QXmppMixIq;

class QXmppMixService
{
public:
    QString jid() const;
    void setJid(const QString &jid);

    bool searchable() const;
    void setSearchable(bool searchable);

    bool canCreateChannel() const;
    void setCanCreateChannel(bool canCreateChannel);

private:
    QString m_jid;
    bool m_searchable = false;
    bool m_canCreateChannel = false;
};

class QXmppMixManager : public QXmppClientExtension
{
    Q_OBJECT
    Q_PROPERTY(bool mixSupported READ mixSupported NOTIFY mixSupportedChanged)
    Q_PROPERTY(bool archivingSupported READ archivingSupported
                                       NOTIFY archivingSupportedChanged)

public:
    QXmppMixManager();

    QStringList discoveryFeatures() const override;

    QList<QXmppMixService> mixServices() const;

    bool mixSupported() const;
    bool archivingSupported() const;

    void joinChannel(const QString &channelJid, const QString &nick = QString());
    void joinChannel(const QString &channelJid, const QString &nick, const QStringList &nodes);

    void leaveChannel(const QString &channelJid);

    void setNick(const QString &channelJid, const QString &requestedNick);

    void createPrivateChannel(const QString &mixService);
    void createPublicChannel(const QString &channelJid);
    void createPublicChannel(const QString &mixService, const QString &channelId);

    void destroyChannel(const QString &channelJid);
    void destroyChannel(const QString &mixService, const QString &channelId);

    void updateSubscription(const QString &channelJid, const QStringList &nodes);

    bool handleStanza(const QDomElement &element) override;

signals:
    void mixSupportedChanged();
    void archivingSupportedChanged();
    void mixServiceDiscovered(const QXmppMixService&);

    void channelJoined(const QXmppMixIq &iq);
    void channelLeft(const QXmppMixIq &iq);
    void nickChanged(const QXmppMixIq &iq);
    void channelCreated(const QXmppMixIq &iq);
    void channelDestroyed(const QXmppMixIq &iq);
    void subscriptionUpdated(const QXmppMixIq &iq);

protected:
    void setClient(QXmppClient *client) override;

private slots:
    void handleDiscoInfo(const QXmppDiscoveryIq &iq);

private:
    void setMixSupported(bool mixSupported);
    void setArchivingSupported(bool archivingSupported);

    QList<QXmppMixService> m_mixServices;
    bool m_mixSupported = false;
    bool m_archivingSupported = false;
};

#endif // QXMPPMIXMANAGER_H
