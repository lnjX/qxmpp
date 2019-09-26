/*
 * Copyright (C) 2008-2019 The QXmpp developers
 *
 * Author:
 *  Yury Gubich <blue@macaw.me>
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
#include <QByteArray>
#include <QMimeDatabase>
#include "QXmppUploadRequestManager.h"
#include "QXmppClient.h"
#include "QXmppDiscoveryManager.h"
#include "QXmppLogger.h"
#include "util.h"

class TestHelper : public QObject 
{
    Q_OBJECT
public:
    TestHelper(bool expectedEvent, bool expectedError);
    ~TestHelper();
    
public slots:
    void onSlotReceived(const QXmppHttpUploadSlotIq &slot);
    void onRequestFailed(const QXmppHttpUploadRequestIq &request);
    
private:
    bool expectedEvent;
    bool expectedError;
    bool event;
    bool error;
};

class ManagerHeir : public QXmppUploadRequestManager {                  //i need this to give up protected methods to be able to test them
public:
    bool handleStanza(const QDomElement & stanza) override;
};

class DiscoveryHeir : public QXmppDiscoveryManager {                       //i need this to emulate discovery events
    
};

class tst_QXmppUploadRequestManager : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    
    void testDiscoveryService();
    void testDiscoveryService_data();
    
    void testHandleStanza();
    void testHandleStanza_data();
    
    void testSending();
    void testSending_data();
    
private:
    ManagerHeir* manager;
    QXmppClient client;
    DiscoveryHeir* discovery;
    QString uploadServiceName;
    quint64 maxFileSize;
};

void tst_QXmppUploadRequestManager::testHandleStanza_data()
{
    QTest::addColumn<QByteArray>("xml");
    QTest::addColumn<bool>("accepted");
    QTest::addColumn<bool>("event");
    QTest::addColumn<bool>("error");

    QTest::newRow("notAccepted")
        << QByteArray("<message xmlns='jabber:client'"
                      "from='romeo@montague.example'"
                      "to='romeo@montague.example/home'"
                      "type='chat'>"
               "<received xmlns='urn:xmpp:carbons:2'>"
                 "<forwarded xmlns='urn:xmpp:forward:0'>"
                   "<message xmlns='jabber:client'"
                            "from='juliet@capulet.example/balcony'"
                            "to='romeo@montague.example/garden'"
                            "type='chat'>"
                     "<body>What man art thou that, thus bescreen'd in night, so stumblest on my counsel?</body>"
                     "<thread>0e3141cd80894871a68e6fe6b1ec56fa</thread>"
                   "</message>"
                 "</forwarded>"
               "</received>"
             "</message>")
        << false << false << false;
        
    QTest::newRow("slotReceived")
        << QByteArray("<iq from='upload.montague.tld' id='step_03' to='romeo@montague.tld/garden' type='result'>"
                        "<slot xmlns='urn:xmpp:http:upload:0'>"
                            "<put url='https://upload.montague.tld/4a771ac1-f0b2-4a4a-9700-f2a26fa2bb67/tr%C3%A8s%20cool.jpg'>"
                                "<header name='Authorization'>Basic Base64String==</header>"
                                "<header name='Cookie'>foo=bar; user=romeo</header>"
                            "</put>"
                            "<get url='https://download.montague.tld/4a771ac1-f0b2-4a4a-9700-f2a26fa2bb67/tr%C3%A8s%20cool.jpg' />"
                        "</slot>"
                    "</iq>")
        << true << true << false;
        
    QTest::newRow("tooLargeError")
        << QByteArray("<iq from='upload.montague.tld' id='step_03' to='romeo@montague.tld/garden' type='error'>"
                        "<request xmlns='urn:xmpp:http:upload:0' filename='très cool.jpg' size='23456' content-type='image/jpeg' />"
                        "<error type='modify'>"
                            "<not-acceptable xmlns='urn:ietf:params:xml:ns:xmpp-stanzas' />"
                            "<text xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'>File too large. The maximum file size is 20000 bytes</text>"
                            "<file-too-large xmlns='urn:xmpp:http:upload:0'>"
                            "<max-file-size>20000</max-file-size>"
                            "</file-too-large>"
                        "</error>"
                    "</iq>")
        << true << true << true;
        
    QTest::newRow("quotaReachedError")
        << QByteArray("<iq from='upload.montague.tld' id='step_03' to='romeo@montague.tld/garden' type='error'>"
                        "<request xmlns='urn:xmpp:http:upload:0' filename='très cool.jpg' size='23456' content-type='image/jpeg' />"
                        "<error type='wait'>"
                            "<resource-constraint xmlns='urn:ietf:params:xml:ns:xmpp-stanzas' />"
                            "<text xmlns='urn:ietf:params:xml:ns:xmpp-stanzas'>Quota reached. You can only upload 5 files in 5 minutes</text>"
                            "<retry xmlns='urn:xmpp:http:upload:0' stamp='2017-12-03T23:42:05Z' />"
                        "</error>"
                    "</iq>")
        << true << true << true;
}

void tst_QXmppUploadRequestManager::testHandleStanza() 
{
    QFETCH(QByteArray, xml);
    QFETCH(bool, accepted);
    QFETCH(bool, event);
    QFETCH(bool, error);

    TestHelper helper(event, error);
    connect(manager, &QXmppUploadRequestManager::slotReceived, &helper, &TestHelper::onSlotReceived);
    connect(manager, &QXmppUploadRequestManager::requestFailed, &helper, &TestHelper::onRequestFailed);

    QDomDocument doc;
    QCOMPARE(doc.setContent(xml, true), true);
    QDomElement element = doc.documentElement();

    bool realAaccepted = manager->handleStanza(element);

    QCOMPARE(realAaccepted, accepted);
}

void tst_QXmppUploadRequestManager::testDiscoveryService()
{
    QFETCH(QByteArray, xml);
    QFETCH(bool, discovered);
    
    QDomDocument doc;
    QCOMPARE(doc.setContent(xml, true), true);
    QDomElement element = doc.documentElement();
    
    bool accepted = discovery->handleStanza(element);
    QCOMPARE(accepted, true);
    QCOMPARE(manager->serviceFound(), discovered);
}

void tst_QXmppUploadRequestManager::testDiscoveryService_data()
{
    QTest::addColumn<QByteArray>("xml");
    QTest::addColumn<bool>("discovered");
    
    QTest::newRow("mixDiscoveryStanzaIq")
        << QByteArray("<iq from='mix.shakespeare.example' id='lx09df27' to='hag66@shakespeare.example/UUID-c8y/1573' type='result'>"
                        "<query xmlns='http://jabber.org/protocol/disco#info'>"
                            "<identity category='conference' name='Shakespearean Chat Service' type='mix '/>"
                            "<feature var='urn:xmpp:mix:core:1' />"
                            "<feature var='urn:xmpp:mix:core:1#searchable' />"
                        "</query>"
                    "</iq>")
        << false;
        
    QTest::newRow("HTTPUploadDiscoveryStansaIq")
    << QByteArray("<iq from='" + uploadServiceName.toUtf8() + "' id='step_02' to='romeo@montague.tld/garden' type='result'>"
                    "<query xmlns='http://jabber.org/protocol/disco#info'>"
                        "<identity category='store' type='file' name='HTTP File Upload' />"
                        "<feature var='urn:xmpp:http:upload:0' />"
                        "<x type='result' xmlns='jabber:x:data'>"
                            "<field var='FORM_TYPE' type='hidden'>"
                                "<value>urn:xmpp:http:upload:0</value>"
                            "</field>"
                            "<field var='max-file-size'>"
                                "<value>" + QString(std::to_string(maxFileSize).c_str()).toUtf8() + "</value>"
                            "</field>"
                        "</x>"
                    "</query>"
                "</iq>")
    << true;
}

void tst_QXmppUploadRequestManager::testSending()
{
    QFETCH(QString, fileName);
    QFETCH(quint64, fileSize);
    QFETCH(QString, fileType);
    
    QXmppLogger* logger = new QXmppLogger();
    logger->setLoggingType(QXmppLogger::SignalLogging);
    connect(logger, &QXmppLogger::message, [this, fileName, fileSize, fileType](QXmppLogger::MessageType type, const QString &text) {
        QCOMPARE(type, QXmppLogger::SentMessage);
        
        QDomDocument doc;
        QCOMPARE(doc.setContent(text, true), true);
        QDomElement element = doc.documentElement();
        
        QXmppIq iq;
        iq.parse(element);
        
        QCOMPARE(iq.type(), QXmppIq::Get);
        QCOMPARE(iq.to(), uploadServiceName);
        
        QXmppElementList exts = iq.extensions();
        QCOMPARE(exts.size(), 1);
        QXmppElement elem = exts.first();
        QCOMPARE(elem.tagName(), "request");
        QCOMPARE(elem.attribute("filename"), fileName);
        QCOMPARE(elem.attribute("size"), std::to_string(fileSize).c_str());
        QCOMPARE(elem.attribute("content-type"), fileType);
    });
    client.setLogger(logger);
    
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForName(fileType);              //I do not check the returning value of requestUploadSlot
    manager->requestUploadSlot(fileName, fileSize, type);       //Because it doesn't get sent, client is not connected
}

void tst_QXmppUploadRequestManager::testSending_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<quint64>("fileSize");
    QTest::addColumn<QString>("fileType");
    
    QTest::newRow("fileWithSizeBelowLimit") <<                  //there is no size above limit handling in request manager
        "whatever.jpeg" << 698547ULL << "image/jpeg";           //there is also no code that selects an upload service with proper
                                                                //size limit above requesting file size.
    QTest::newRow("fileWithSizeAboveLimit") <<                  //Is it something to worry about?
        "some.pdf" << 65896498547ULL << "application/pdf";
}

void tst_QXmppUploadRequestManager::initTestCase()
{
    uploadServiceName = "upload.montague.tld";
    maxFileSize = 500UL * 1024UL * 1024UL;
    manager = new ManagerHeir();
    discovery = new DiscoveryHeir();
    client.insertExtension(0, discovery);
    client.addExtension(manager);
}

TestHelper::TestHelper(bool p_expectedEvent, bool p_expectedError):
    QObject(),
    expectedEvent(p_expectedEvent),
    expectedError(p_expectedError),
    event(false),
    error(false)
{
    
}

void TestHelper::onRequestFailed(const QXmppHttpUploadRequestIq& request)
{
    event = true;
    error = true;
}

void TestHelper::onSlotReceived(const QXmppHttpUploadSlotIq& slot)
{
    event = true;
    error = false;
}

bool ManagerHeir::handleStanza(const QDomElement& stanza)
{
    return QXmppUploadRequestManager::handleStanza(stanza);
}

TestHelper::~TestHelper()
{
    QCOMPARE(event, expectedEvent);
    QCOMPARE(error, expectedError);
}


QTEST_MAIN(tst_QXmppUploadRequestManager)
#include "tst_qxmppuploadrequestmanager.moc"

