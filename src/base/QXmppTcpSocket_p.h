/*
 * Copyright (C) 2008-2020 The QXmpp developers
 *
 * Author:
 *  Linus Jahn
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

#ifndef QXMPPTCPSOCKET_H
#define QXMPPTCPSOCKET_H

#include "QXmppSocket.h"

class QSslCertificate;
class QSslKey;
class QSslSocket;

class QXmppTcpSocket : public QXmppSocket
{
    Q_OBJECT

public:
    explicit QXmppTcpSocket(QObject *parent = nullptr);

    void connectToHost(const QString &host, quint16 port);
    void disconnectFromHost() override;
    bool sendTextMessage(const QString &data) override;
    bool flush() override;

    QAbstractSocket::SocketState state() const override;

    QAbstractSocket::SocketError error() const override;
    QString errorString() const override;

    QHostAddress localAddress() const override;
    quint16 localPort() const override;
    QHostAddress peerAddress() const override;
    quint16 peerPort() const override;

    QNetworkProxy proxy() const override;
    void setProxy(const QNetworkProxy &proxy) override;

    void startClientEncryption();
    void startServerEncryption();
    bool isEncrypted() const override;
    bool supportsEncryption() const override;
    void setPeerVerifyName(const QString &peerName) override;
    void ignoreSslErrors(const QList<QSslError> &errors) override;
    QSslConfiguration sslConfiguration() const override;
    void setSslConfiguration(const QSslConfiguration &sslConfiguration) override;

    bool setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState state = QAbstractSocket::ConnectedState, QIODevice::OpenMode openMode = QIODevice::ReadWrite);

public Q_SLOTS:
    void ignoreSslErrors() override;

protected:
    QSslSocket *socket;
};

#endif // QXMPPTCPSOCKET_H
