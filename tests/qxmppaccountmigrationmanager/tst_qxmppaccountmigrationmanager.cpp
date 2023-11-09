// SPDX-FileCopyrightText: 2023 Filipe Azevedo <pasnox@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "QXmppAccountMigrationManager.h"
#include "QXmppClient.h"
#include "QXmppRosterManager.h"
#include "QXmppVCardIq.h"
#include "QXmppVCardManager.h"

#include "util.h"

#define QSL QStringLiteral

class tst_QXmppAccountMigrationManager : public QObject
{
    Q_OBJECT

private:
    Q_SLOT void initTestCase();
    Q_SLOT void testKK();
    Q_SLOT void testExportData();
    Q_SLOT void testImportData();

//    using ExportData = QXmppAccountMigrationManager::ExportData;

    QXmppRosterIq::Item newRosterItem(const QString &bareJid, const QString &name, const QSet<QString> &groups = {}) const
    {
        QXmppRosterIq::Item item;
        item.setBareJid(bareJid);
        item.setName(name);
        item.setGroups(groups);
        item.setSubscriptionType(QXmppRosterIq::Item::NotSet);
        return item;
    }

    QXmppRosterIq newRoster() const
    {
        QXmppRosterIq roster;
        // we need an id equaling QXmppRosterManagerPrivate::rosterReqId to be considered initial request
        roster.setId({});
        roster.setType(QXmppIq::Type::Result);
        roster.addItem(newRosterItem(QSL("1@bare.com"), QSL("1 Bare"), { QSL("all") }));
        roster.addItem(newRosterItem(QSL("2@bare.com"), QSL("2 Bare"), { QSL("all") }));
        roster.addItem(newRosterItem(QSL("3@bare.com"), QSL("3 Bare"), { QSL("all") }));
        return roster;
    }

    bool receiveRoster(const QXmppRosterIq &roster)
    {
        return m_rosterManager->handleStanza(writePacketToDom(roster));
    }

    QXmppVCardIq newClientVCard() const
    {
        QXmppVCardIq vcard;
        vcard.setFirstName(QSL("First"));
        vcard.setLastName(QSL("Last"));
        vcard.setNickName(QSL("It's me mario"));
        return vcard;
    }

    bool receiveClientVCard(const QXmppVCardIq &vcard)
    {
        return m_vcardManager->handleStanza(writePacketToDom(vcard));
    }

    QXmppClient *m_client = nullptr;
    QXmppAccountMigrationManager *m_migrationManager = nullptr;
    QXmppRosterManager *m_rosterManager = nullptr;
    QXmppVCardManager *m_vcardManager = nullptr;
};

void tst_QXmppAccountMigrationManager::initTestCase()
{
    m_client = new QXmppClient(QXmppClient::NoExtensions, this);

    m_client->addNewExtension<QXmppAccountMigrationManager>();
    m_client->addNewExtension<QXmppRosterManager>(m_client);
    m_client->addNewExtension<QXmppVCardManager>();
    m_client->configuration().setJid("pasnox@xmpp.example");

    m_migrationManager = m_client->findExtension<QXmppAccountMigrationManager>();
    m_rosterManager = m_client->findExtension<QXmppRosterManager>();
    m_vcardManager = m_client->findExtension<QXmppVCardManager>();
}

void tst_QXmppAccountMigrationManager::testKK()
{
    auto task = m_migrationManager->exportData();

    task.then(this, [](QXmppAccountMigrationManager::Account &&account) {
        qDebug() << "Done" << account.bareJid;

        if (account.vcard) {
            if (auto vcard = std::any_cast<QXmppVCardIq>(&*account.vcard)) {
                qDebug() << "VCard:" << packetToXml(*vcard);
            } else {
                qDebug() << "VCard error:" << std::any_cast<QXmppError>(*account.vcard).description;
            }
        }

        if (account.roster) {
            if (auto roster = std::any_cast<QXmppRosterIq>(&*account.roster)) {
                qDebug() << "Roster:" << packetToXml(*roster);
            } else {
                qDebug() << "Roster error:" << std::any_cast<QXmppError>(*account.roster).description;
            }
        }
    });

    QTimer::singleShot(500, this, [this]() {
        receiveClientVCard(newClientVCard());
        receiveRoster(newRoster());
    });

    QVERIFY(QTest::qWaitFor([task]() {
        return task.isFinished();
    }));
}

void tst_QXmppAccountMigrationManager::testExportData()
{
//    // Export, no data yet
//    {
//        QVERIFY(!m_rosterManager->isRosterReceived());
//        QVERIFY(!m_vcardManager->isClientVCardReceived());

//        auto task = m_migrationManager->exportData();

//        auto future = task.toFuture(this);
//        future.waitForFinished();

//        auto result = future.result();

//        QVERIFY(std::holds_alternative<QXmppError>(result));
//        const auto error = std::get<QXmppError>(result);
//        QVERIFY(error.holdsType<void>());
//        QCOMPARE(error.description, QSL("Did not received VCard."));
//    }

//    // Export, set roster / vcard
//    {
//        QVERIFY(!m_rosterManager->isRosterReceived());
//        QVERIFY(!m_vcardManager->isClientVCardReceived());

//        QVERIFY(receiveRoster(newRoster()));
//        QVERIFY(receiveClientVCard(newClientVCard()));

//        auto task = m_migrationManager->exportData();

//        auto future = task.toFuture(this);
//        future.waitForFinished();

//        auto result = future.result();

//        QVERIFY(std::holds_alternative<ExportData>(result));
//        const auto data = std::get<ExportData>(result);
//        QCOMPARE(data.roster.items().count(), 3);
//        QCOMPARE(data.vcard.firstName(), QSL("First"));
//    }
}

void tst_QXmppAccountMigrationManager::testImportData()
{
//    ExportData data;

//    // Export, with current roster / vcard
//    {
//        QVERIFY(m_rosterManager->isRosterReceived());
//        QVERIFY(m_vcardManager->isClientVCardReceived());

//        auto task = m_migrationManager->exportData();

//        auto future = task.toFuture(this);
//        future.waitForFinished();

//        auto result = future.result();

//        QVERIFY(std::holds_alternative<ExportData>(result));
//        data = std::get<ExportData>(result);
//        QCOMPARE(data.roster.items().count(), 3);
//        QCOMPARE(data.vcard.firstName(), QSL("First"));
//    }

//    // Import exported data without changing the client crendentials
//    {
//        auto task = m_migrationManager->importData(data);

//        auto future = task.toFuture(this);
//        future.waitForFinished();

//        auto error = future.result();

//        QVERIFY(error.holdsType<bool>());
//        QCOMPARE(error.description, QSL("Can not import data into the same account."));
//        QCOMPARE(error.value<bool>(), false);
//    }

//    // Import exported data with changing the client crendetials
//    {
//        m_client.configuration().setJid(QSL("bookri@xmpp.example"));

//        // We are not connected, so the client roster subscription will fails
//        QTest::ignoreMessage(QtWarningMsg, "Can not send subscription for 1@bare.com.");
//        QTest::ignoreMessage(QtWarningMsg, "Can not send subscription for 2@bare.com.");
//        QTest::ignoreMessage(QtWarningMsg, "Can not send subscription for 3@bare.com.");

//        int version = 0;
//        auto task = m_migrationManager->importData(data);

//        auto future = task.toFuture(this);
//        future.waitForFinished();

//        auto error = future.result();

//        QCOMPARE(error.description, QString());
//        QVERIFY(error.holdsType<void>());
//    }
}

QTEST_MAIN(tst_QXmppAccountMigrationManager)
#include "tst_qxmppaccountmigrationmanager.moc"
