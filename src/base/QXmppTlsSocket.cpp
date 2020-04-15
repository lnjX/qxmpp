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

#include "QXmppTlsSocket_p.h"

#include <QSslSocket>

QXmppTlsSocket::QXmppTlsSocket(QObject *parent)
    : QXmppTcpSocket(parent)
{
}

void QXmppTlsSocket::connectToHost(const QString &host, quint16 port)
{
    socket->connectToHostEncrypted(host, port);
}
