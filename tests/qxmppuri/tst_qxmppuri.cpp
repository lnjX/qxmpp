/*
 * Copyright (C) 2008-2019 The QXmpp developers
 *
 * Author:
 *  Linus Jahn
 *  Melvin Keskin
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

#include <QObject>

#include "QXmppUri.h"
#include "QXmppMessage.h"
#include "util.h"

class tst_QXmppUri : public QObject
{
    Q_OBJECT

private slots:
    void testBasic_data();
    void testBasic();

    void testMessage();
};

void tst_QXmppUri::testBasic_data()
{
	QTest::addColumn<QString>("uriString");

	QTest::newRow("") << QStringLiteral("xmpp:user@example.org?message;body=Hello%20World");
// 	QTest::newRow("") << QStringLiteral("xmpp:user@example.org?message;body=][!@#$%^&*(;");
}

void tst_QXmppUri::testBasic()
{
    QFETCH(QString, uriString);

    QXmppUri uri(uriString);
    QCOMPARE(uri.toString(), uriString);
}

void tst_QXmppUri::testMessage()
{
	QXmppMessage msg;
	msg.setBody(QStringLiteral("(Hello!\n\t@#$%^&*()_+[]{}'\\\"/?<>)"));
	msg.setSubject(QStringLiteral("random 12abAB??;&;^"));
	msg.setId(QStringLiteral("new-message-3"));

    QXmppUri uri;
	uri.setJid(QStringLiteral("alice@example.org"));
	uri.setAction(QXmppUri::Message);
	uri.setMessage(msg);

	QXmppUri uri2(uri.toString());
	QCOMPARE(uri2.jid(), uri.jid());
	QCOMPARE(uri2.action(), uri.action());
	QCOMPARE(uri2.message().body(), msg.body());
	QCOMPARE(uri2.message().subject(), msg.subject());
	QCOMPARE(uri2.message().id(), msg.id());
}

QTEST_MAIN(tst_QXmppUri)
#include "tst_qxmppuri.moc"
