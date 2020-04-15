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

#include "QXmppTcpSocket_p.h"

#include <QHostAddress>
#include <QNetworkProxy>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslSocket>

QXmppTcpSocket::QXmppTcpSocket(QObject *parent)
    : QXmppSocket(parent),
      socket(new QSslSocket(this))
{
    connect(socket, &QAbstractSocket::connected,
            this, &QXmppSocket::connected);
    connect(socket, &QAbstractSocket::disconnected,
            this, &QXmppSocket::disconnected);
    connect(socket, &QSslSocket::encrypted,
            this, &QXmppSocket::encryptionStarted);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(socket, &QAbstractSocket::errorOccurred,
            this, &QXmppSocket::errorOccured);
#else
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &QXmppSocket::errorOccured);
#endif
    connect(socket, &QAbstractSocket::stateChanged,
            this, &QXmppSocket::stateChanged);
    connect(socket, &QSslSocket::readyRead, this, [this]() {
        emit textMessageReceived(QString::fromUtf8(socket->readAll()));
    });
    connect(socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
            this, &QXmppSocket::sslErrors);
}

void QXmppTcpSocket::connectToHost(const QString &host, quint16 port)
{
    socket->connectToHost(host, port);
}

void QXmppTcpSocket::disconnectFromHost()
{
    socket->disconnectFromHost();
}

bool QXmppTcpSocket::sendTextMessage(const QString &data)
{
    return socket->write(data.toUtf8());
}

bool QXmppTcpSocket::flush()
{
    return socket->flush();
}

QAbstractSocket::SocketState QXmppTcpSocket::state() const
{
    return socket->state();
}

QAbstractSocket::SocketError QXmppTcpSocket::error() const
{
    return socket->error();
}

QString QXmppTcpSocket::errorString() const
{
    return socket->errorString();
}

QHostAddress QXmppTcpSocket::localAddress() const
{
    return socket->localAddress();
}

quint16 QXmppTcpSocket::localPort() const
{
    return socket->localPort();
}

QHostAddress QXmppTcpSocket::peerAddress() const
{
    return socket->peerAddress();
}

quint16 QXmppTcpSocket::peerPort() const
{
    return socket->peerPort();
}

QNetworkProxy QXmppTcpSocket::proxy() const
{
    return socket->proxy();
}

void QXmppTcpSocket::setProxy(const QNetworkProxy &proxy)
{
    socket->setProxy(proxy);
}

void QXmppTcpSocket::startClientEncryption()
{
    socket->startClientEncryption();
}

void QXmppTcpSocket::startServerEncryption()
{
    socket->startServerEncryption();
}

bool QXmppTcpSocket::isEncrypted() const
{
    return socket->isEncrypted();
}

bool QXmppTcpSocket::supportsEncryption() const
{
    return socket->supportsSsl();
}

void QXmppTcpSocket::setPeerVerifyName(const QString &peerName)
{
    socket->setPeerVerifyName(peerName);
}

void QXmppTcpSocket::ignoreSslErrors(const QList<QSslError> &errors)
{
    socket->ignoreSslErrors(errors);
}

QSslConfiguration QXmppTcpSocket::sslConfiguration() const
{
    return socket->sslConfiguration();
}

void QXmppTcpSocket::setSslConfiguration(const QSslConfiguration &sslConfiguration)
{
    socket->setSslConfiguration(sslConfiguration);
}

bool QXmppTcpSocket::setSocketDescriptor(qintptr socketDescriptor, QAbstractSocket::SocketState state, QIODevice::OpenMode openMode)
{
    return socket->setSocketDescriptor(socketDescriptor, state, openMode);
}

void QXmppTcpSocket::ignoreSslErrors()
{
    socket->ignoreSslErrors();
}
