// SPDX-FileCopyrightText: 2023 Filipe Azevedo <pasnox@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "QXmppAccountMigrationManager.h"

#include "QXmppClient.h"
#include "QXmppPromise.h"
#include "QXmppTask.h"

///
/// \brief The QXmppAccountMigrationManager class provides access to account migration.
///
/// It allow to export server and client side data and import them into another server.
/// Use \c QXmppAccountMigrationManager::exportData to start export task.
/// When the application is ready to import the previously exported data use
/// \c QXmppAccountMigrationManager::importData to start the import task.
/// Note than before importing data, it is important to change the client credentials
/// with the new user account, failing to do that would result in an import error.
///
/// \ingroup Managers
/// \since QXmpp 1.7
///

///
/// \class QXmppAccountMigrationManager
///
/// This manager help migrating a user account into another server.
///

///
/// \typedef QXmppAccountMigrationManager::ClientDataExport
///
/// A callback signature to export client side data of the given jid.
///

///
/// \typedef QXmppAccountMigrationManager::ClientDataImport
///
/// A callback signature to import client side data to the given jid.
///

///
/// \typedef QXmppAccountMigrationManager::ExportResult
///
/// Contains Data<ClientData> for success or a QXmppError for an error.
///

///
/// \typedef QXmppAccountMigrationManager::ImportResult
///
/// Contains a QXmppError error.
///

///
/// Constructs an account migration manager.
///
QXmppAccountMigrationManager::QXmppAccountMigrationManager() = default;

QXmppAccountMigrationManager::~QXmppAccountMigrationManager() = default;

///
/// \brief Create an export task.
/// \return Returns Data on success or QXmppError on error.
///
QXmppTask<QXmppAccountMigrationManager::Account> QXmppAccountMigrationManager::exportData()
{
    QXmppPromise<Account> promise;
    auto account = std::make_shared<Account>();
    auto counter = std::make_shared<int>(m_registeredExtensions.count());
    auto it = m_registeredExtensions.constBegin();

    account->bareJid = client()->configuration().jidBare();

    while (it != m_registeredExtensions.constEnd()) {
        it->exportCallback(*account).then(this, [promise, account, counter]() mutable {
            if ((--(*counter)) == 0) {
                promise.finish(*account);
            }
        });

        ++it;
    }

    if (m_registeredExtensions.isEmpty()) {
        promise.finish(*account);
    }

    return promise.task();
}

///
/// \brief Create an import task.
/// \param data The data to import.
/// \return Returns QXmppError, if there is no error the error is empty.
///
QXmppTask<QList<QXmppError>> QXmppAccountMigrationManager::importData(const Account &account)
{
    using ErrorList = QList<QXmppError>;
    QXmppPromise<ErrorList> promise;

    if (client()->configuration().jidBare() == account.bareJid) {
        promise.finish(ErrorList { QXmppError { tr("Can not import data into the same account."), {} } });
    } else {
        auto errors = std::make_shared<ErrorList>();
        auto counter = std::make_shared<int>(m_registeredExtensions.count());
        auto it = m_registeredExtensions.constBegin();

        errors->reserve(m_registeredExtensions.count());

        while (it != m_registeredExtensions.constEnd()) {
            it->importCallback(account).then(this, [promise, errors, counter](ImportResult &&result) mutable {
                if (result) {
                    errors->append(std::move(*result));
                }

                if ((--(*counter)) == 0) {
                    promise.finish(*errors);
                }
            });

            ++it;
        }

        if (m_registeredExtensions.isEmpty()) {
            promise.finish(*errors);
        }
    }

    return promise.task();
}
