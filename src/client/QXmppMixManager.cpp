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

#include "QXmppMixManager.h"
#include "QXmppConstants_p.h"
#include "QXmppClient.h"
#include "QXmppDiscoveryManager.h"
#include "QXmppMixIq.h"
#include "QXmppMixItem.h"
#include "QXmppUtils.h"

#include <QDomElement>

const QStringList DEFAULT_SUBSCRIBE_NODES = QStringList()
        << ns_mix_node_config
        << ns_mix_node_info
        << ns_mix_node_messages
        << ns_mix_node_participants
        << ns_mix_node_presence;

QString QXmppMixService::jid() const
{
    return m_jid;
}

void QXmppMixService::setJid(const QString &jid)
{
    m_jid = jid;
}

bool QXmppMixService::searchable() const
{
    return m_searchable;
}

void QXmppMixService::setSearchable(bool searchable)
{
    m_searchable = searchable;
}

bool QXmppMixService::canCreateChannel() const
{
    return m_canCreateChannel;
}

void QXmppMixService::setCanCreateChannel(bool canCreateChannel)
{
    m_canCreateChannel = canCreateChannel;
}

QXmppMixManager::QXmppMixManager() = default;

QStringList QXmppMixManager::discoveryFeatures() const
{
    return QStringList() << ns_mix;
}

QList<QXmppMixService> QXmppMixManager::mixServices() const
{
    return m_mixServices;
}

void QXmppMixManager::handleDiscoInfo(const QXmppDiscoveryIq &iq)
{
    // check features of own server
    if (iq.from().isEmpty() || iq.from() == client()->configuration().domain()) {
        // check whether MIX is supported at all
        if (iq.features().contains(ns_mix_pam)) {
            setMixSupported(true);

            // check whether MIX archiving is supported
            if (iq.features().contains(ns_mix_pam_archiving))
                setArchivingSupported(true);
        }
    }

    if (!iq.features().contains(ns_mix))
        return;

    for (const QXmppDiscoveryIq::Identity &identity : iq.identities()) {
        if (identity.category() == "conference" && identity.type() == "mix") {
            QXmppMixService service;
            service.setJid(iq.from().isEmpty()
                           ? client()->configuration().domain()
                           : iq.from());
            service.setSearchable(iq.features().contains(ns_mix_searchable));
            service.setCanCreateChannel(iq.features().contains(
                                        ns_mix_create_channel));

            if (!m_mixServices.contains(service)) {
                m_mixServices.append(service);
                emit mixServiceDiscovered(service);
            }
            return;
        }
    }
}

bool QXmppMixManager::mixSupported() const
{
    return m_mixSupported;
}

bool QXmppMixManager::archivingSupported() const
{
    return m_archivingSupported;
}

/// Joins a channel with default configuration.

void QXmppMixManager::joinChannel(const QString &channelJid, const QString &nick)
{
    joinChannel(channelJid, nick, DEFAULT_SUBSCRIBE_NODES);
}

/// Joins a channel with specific nodes.

void QXmppMixManager::joinChannel(const QString &channelJid, const QString &nick, const QStringList &nodes)
{
    QXmppMixIq iq;
    iq.setType(QXmppIq::Set);
    iq.setTo(client()->configuration().domain());
    iq.setActionType(QXmppMixIq::ClientJoin);
    iq.setJid(channelJid);
    iq.setNodes(nodes);
    iq.setNick(nick);

    client()->sendPacket(iq);
}

void QXmppMixManager::leaveChannel(const QString &channelJid)
{
    QXmppMixIq iq;
    iq.setType(QXmppIq::Set);
    iq.setTo(client()->configuration().domain());
    iq.setActionType(QXmppMixIq::ClientLeave);
    iq.setJid(channelJid);

    client()->sendPacket(iq);
}

void QXmppMixManager::setNick(const QString &channelJid, const QString &requestedNick)
{
    QXmppMixIq iq;
    iq.setType(QXmppIq::Set);
    iq.setTo(channelJid);
    iq.setActionType(QXmppMixIq::SetNick);
    iq.setNick(requestedNick);

    client()->sendPacket(iq);
}

/// Creates an unlisted channel with a random ID.
///
/// \param mixService The MIX service to create the channel on.

void QXmppMixManager::createPrivateChannel(const QString &mixService)
{
    createPublicChannel(mixService, QString());
}

/// Creates a publicly listed channel with a specified JID.
///
/// \param jid Complete bare JID of the MIX channel.

void QXmppMixManager::createPublicChannel(const QString &jid)
{
    createPublicChannel(QXmppUtils::jidToDomain(jid),
                        QXmppUtils::jidToUser(jid));
}

/// Creates a publicly listed channel with a specified ID.
///
/// \param mixService The MIX service to create the channel on.
/// \param channelId The requested ID of the new channel. This is only the user
///                  part of the JID.

void QXmppMixManager::createPublicChannel(const QString &mixService, const QString &channelId)
{
    QXmppMixIq iq;
    iq.setType(QXmppIq::Set);
    iq.setTo(mixService);
    iq.setActionType(QXmppMixIq::Create);
    iq.setChannelName(channelId);

    client()->sendPacket(iq);
}

/// Destroys a channel.
///
/// \param jid Complete bare JID of the MIX channel.

void QXmppMixManager::destroyChannel(const QString &jid)
{
    destroyChannel(QXmppUtils::jidToDomain(jid), QXmppUtils::jidToUser(jid));
}

/// Destroys a given channel on a MIX service.
///
/// \param mixService The MIX service to destroy the channel on.
/// \param channelId The ID of the channel to destroy. This is only the user
///                  part of the JID.

void QXmppMixManager::destroyChannel(const QString &mixService, const QString &channelId)
{
    QXmppMixIq iq;
    iq.setType(QXmppIq::Set);
    iq.setTo(mixService);
    iq.setActionType(QXmppMixIq::Destroy);
    iq.setChannelName(channelId);

    client()->sendPacket(iq);
}

/// Updates the subscriptions to a channel.
///
/// \param channelJid The MIX channel JID.
/// \param nodes List of nodes to subscribe to.

void QXmppMixManager::updateSubscription(const QString &channelJid, const QStringList &nodes)
{
    QXmppMixIq iq;
    iq.setType(QXmppIq::Set);
    iq.setTo(channelJid);
    iq.setActionType(QXmppMixIq::UpdateSubscription);
    iq.setNodes(nodes);

    client()->sendPacket(iq);
}

bool QXmppMixManager::handleStanza(const QDomElement &element)
{
    if (element.tagName() == "iq" && QXmppMixIq::isMixIq(element)) {
        QXmppMixIq iq;
        iq.parse(element);

        if (iq.type() != QXmppIq::Result)
            return false;

        switch (iq.actionType()) {
        case QXmppMixIq::ClientJoin:
            emit channelJoined(iq); break;
        case QXmppMixIq::ClientLeave:
            emit channelLeft(iq); break;
        case QXmppMixIq::UpdateSubscription:
            emit subscriptionUpdated(iq); break;
        case QXmppMixIq::SetNick:
            emit nickChanged(iq); break;
        case QXmppMixIq::Create:
            emit channelCreated(iq); break;
        case QXmppMixIq::Destroy:
            emit channelDestroyed(iq); break;
        default:
            return false;
        }
        return true;
    } else if (element.tagName() == "message") {
        QDomElement xElement = element.firstChildElement();
        while (!xElement.isNull())
        {
			if (xElement.tagName() == "event" && xElement.namespaceURI() == ns_pubsub_event) {
				QDomElement itemsElement = xElement.firstChildElement("items");
				if (itemsElement.attribute("node") == ns_mix_node_participants) {
					QDomElement itemElement = itemsElement.firstChildElement("item");
					while (!itemElement.isNull()) {
						QString id = itemElement.attribute("id");
						QXmppMixParticipantItem item;
						item.parse(QXmppElement(itemElement.firstChildElement()));

						emit participantJoined(id, item);
						itemElement = itemsElement.nextSiblingElement("item");
					}
				}
			}
            xElement = element.nextSiblingElement();
        }
    }
    return false;
}

void QXmppMixManager::setMixSupported(bool mixSupported)
{
    if (m_mixSupported != mixSupported) {
        m_mixSupported = mixSupported;
        emit mixSupportedChanged();
    }
}

void QXmppMixManager::setArchivingSupported(bool archivingSupported)
{
    if (m_archivingSupported != archivingSupported) {
        m_archivingSupported = archivingSupported;
        emit archivingSupportedChanged();
    }
}

void QXmppMixManager::setClient(QXmppClient *client)
{
    QXmppClientExtension::setClient(client);
    // get service discovery manager
    auto *disco = client->findExtension<QXmppDiscoveryManager>();
    if (disco) {
        // check all incoming information about entities
        connect(disco, &QXmppDiscoveryManager::infoReceived,
                this, &QXmppMixManager::handleDiscoInfo);

        // on client disconnect remove all mix services
        connect(client, &QXmppClient::disconnected, [this] () {
            setMixSupported(false);
            setArchivingSupported(false);
            m_mixServices.clear();
        });
    }
}
