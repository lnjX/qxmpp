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

#ifndef QXMPPSOCKET_H
#define QXMPPSOCKET_H

#include <QAbstractSocket>

class QSslConfiguration;
class QSslError;

class QXmppSocket : public QObject
{
    Q_OBJECT

public:
    enum SocketType {
        TcpSocket,
    };
    Q_ENUM(SocketType)

    static QVector<SocketType> supportedSocketTypes();

    explicit QXmppSocket(QObject *parent = nullptr);

    virtual void connectToHost(const QString &host, quint16 port = 0) = 0;
    virtual void disconnectFromHost() = 0;
    virtual bool sendTextMessage(const QString &data) = 0;
    virtual bool flush() = 0;

    virtual QAbstractSocket::SocketState state() const = 0;

    virtual QAbstractSocket::SocketError error() const = 0;
    virtual QString errorString() const = 0;

    virtual QHostAddress localAddress() const = 0;
    virtual quint16 localPort() const = 0;
    virtual QHostAddress peerAddress() const = 0;
    virtual quint16 peerPort() const = 0;

    virtual QNetworkProxy proxy() const = 0;
    virtual void setProxy(const QNetworkProxy &proxy) = 0;

    virtual bool isEncrypted() const = 0;
    virtual bool supportsEncryption() const = 0;
    virtual void setPeerVerifyName(const QString &peerName) = 0;
    virtual void ignoreSslErrors(const QList<QSslError> &errors) = 0;
    virtual QSslConfiguration sslConfiguration() const = 0;
    virtual void setSslConfiguration(const QSslConfiguration &sslConfiguration) = 0;

public Q_SLOTS:
    virtual void ignoreSslErrors() = 0;

Q_SIGNALS:
    void connected();
    void disconnected();
    void encryptionStarted();
    void errorOccured(QAbstractSocket::SocketError error);
    void stateChanged(QAbstractSocket::SocketState state);
    void textMessageReceived(const QString &data);
    void sslErrors(const QList<QSslError> &errors);
};

#endif // QXMPPSOCKET_H
