/*
 * Copyright (C) 2008-2019 The QXmpp developers
 *
 * Authors:
 *  Manjeet Dahiya
 *  Jeremy Lainé
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


#ifndef QXMPPOUTGOINGCLIENT_H
#define QXMPPOUTGOINGCLIENT_H

#include "QXmppClient.h"
#include "QXmppStanza.h"
#include "QXmppStream.h"

class QDomElement;
class QSslError;

class QXmppConfiguration;
class QXmppPresence;
class QXmppIq;
class QXmppMessage;

class QXmppOutgoingClientPrivate;

/// \brief The QXmppOutgoingClient class represents an outgoing XMPP stream
/// to an XMPP server.
///

class QXMPP_EXPORT QXmppOutgoingClient : public QXmppStream
{
    Q_OBJECT

public:
    QXmppOutgoingClient(QObject *parent);
    ~QXmppOutgoingClient();

    void connectToHost();
    bool isAuthenticated() const;
    bool isConnected() const;
    bool isClientStateIndicationEnabled() const;

    QXmppSocket *socket() const { return QXmppStream::socket(); };
    QXmppStanza::Error::Condition xmppStreamError();

    QXmppConfiguration& configuration();

signals:
    /// This signal is emitted when an error is encountered.
    void error(QXmppClient::Error);

    /// This signal is emitted when an element is received.
    void elementReceived(const QDomElement &element, bool &handled);

    /// This signal is emitted when a presence is received.
    void presenceReceived(const QXmppPresence&);

    /// This signal is emitted when a message is received.
    void messageReceived(const QXmppMessage&);

    /// This signal is emitted when an IQ is received.
    void iqReceived(const QXmppIq&);
#if QXMPP_USE_WEBSOCKETS
#else
    /// This signal is emitted when SSL errors are encountered.
    void sslErrors(const QList<QSslError> &errors);
#endif

protected:
    /// \cond
    // Overridable methods
    virtual void handleStart();
    virtual void handleStanza(const QDomElement &element);
    virtual void handleStream(const QDomElement &element);
    /// \endcond

public slots:
    virtual void disconnectFromHost();

private slots:
    void _q_dnsLookupFinished();
    void _q_socketDisconnected();
    void socketError(QAbstractSocket::SocketError);
#if QXMPP_USE_WEBSOCKETS
#else
    void socketSslErrors(const QList<QSslError>&);
#endif

    void pingStart();
    void pingStop();
    void pingSend();
    void pingTimeout();

private:
    friend class QXmppOutgoingClientPrivate;
    QXmppOutgoingClientPrivate * const d;
};

#endif // QXMPPOUTGOINGCLIENT_H
