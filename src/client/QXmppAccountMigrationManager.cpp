// SPDX-FileCopyrightText: 2023 Filipe Azevedo <pasnox@gmail.com>
// SPDX-FileCopyrightText: 2023 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "QXmppAccountMigrationManager.h"

#include "QXmppClient.h"
#include "QXmppPromise.h"
#include "QXmppTask.h"

// Utilities for account data extensions
using AnyParser = QXmppAccountData::ExtensionParser<std::any>;
using AnySerializer = QXmppAccountData::ExtensionSerializer<std::any>;

struct XmlElementId
{
    QStringView tagName;
    QStringView xmlns;

    bool operator==(const XmlElementId &other) const
    {
        return tagName == other.tagName && xmlns == other.xmlns;
    }
};

template<>
struct std::hash<XmlElementId>
{
    std::size_t operator()(const XmlElementId &m) const noexcept
    {
        auto h1 = std::hash<QStringView> {}(m.tagName);
        auto h2 = std::hash<QStringView> {}(m.xmlns);
        return h1 ^ (h2 << 1);
    }
};

static std::unordered_map<XmlElementId, AnyParser> &accountDataParsers()
{
    thread_local static std::unordered_map<XmlElementId, AnyParser> registry;
    return registry;
}

static std::unordered_map<std::type_index, AnySerializer> &accountDataSerializers()
{
    thread_local static std::unordered_map<std::type_index, AnySerializer> registry;
    return registry;
}

struct QXmppAccountDataPrivate : public QSharedData
{
    QVector<std::any> extensions;
};

QXMPP_PRIVATE_DEFINE_RULE_OF_SIX(QXmppAccountData)

std::variant<QXmppAccountData, QXmppError> QXmppAccountData::fromDom(const QDomElement &el)
{
    // use accountDataParsers()
}

void QXmppAccountData::toXml(QXmlStreamWriter *writer) const
{
    // use accountDataSerializers()
}

void QXmppAccountData::registerExtensionInternal(std::type_index type, ExtensionParser<std::any> parse, ExtensionSerializer<std::any> serialize, QStringView tagName, QStringView xmlns)
{
    accountDataParsers().emplace(XmlElementId { tagName, xmlns }, parse);
    accountDataSerializers().emplace(type, serialize);
}

struct QXmppAccountMigrationManagerPrivate
{
    using ImportFunction = std::function<QXmppTask<std::variant<QXmpp::Success, QXmppError>>(std::any)>;
    using ExportFunction = std::function<QXmppTask<std::variant<std::any, QXmppError>>()>;

    struct ExtensionData
    {
        ImportFunction importFunction;
        ExportFunction exportFunction;
    };
    std::unordered_map<std::type_index, ExtensionData> extensions;
};

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
QXmppAccountMigrationManager::QXmppAccountMigrationManager()
    : d(std::make_unique<QXmppAccountMigrationManagerPrivate>())
{
}

QXmppAccountMigrationManager::~QXmppAccountMigrationManager() = default;

///
/// \brief Create an export task.
/// \return Returns Data on success or QXmppError on error.
///
QXmppTask<QXmppAccountMigrationManager::ExportResult> QXmppAccountMigrationManager::exportData()
{
    QXmppPromise<ExportResult> promise;

    // use d->extensions

    //    auto account = std::make_shared<Account>();
    //    auto counter = std::make_shared<int>(m_registeredExtensions.count());
    //    auto it = m_registeredExtensions.constBegin();

    //    account->bareJid = client()->configuration().jidBare();

    //    while (it != m_registeredExtensions.constEnd()) {
    //        it->exportCallback(*account).then(this, [promise, account, counter]() mutable {
    //            if ((--(*counter)) == 0) {
    //                promise.finish(*account);
    //            }
    //        });

    //        ++it;
    //    }

    //    if (m_registeredExtensions.isEmpty()) {
    //        promise.finish(*account);
    //    }

    return promise.task();
}

///
/// \brief Create an import task.
/// \param data The data to import.
/// \return Returns QXmppError, if there is no error the error is empty.
///
QXmppTask<QXmppAccountMigrationManager::ImportResult> QXmppAccountMigrationManager::importData(const QXmppAccountData &account)
{
    QXmppPromise<ImportResult> promise;

    // use d->extensions

    //    if (client()->configuration().jidBare() == account.bareJid) {
    //        promise.finish(ErrorList { QXmppError { tr("Can not import data into the same account."), {} } });
    //    } else {
    //        auto errors = std::make_shared<ErrorList>();
    //        auto counter = std::make_shared<int>(m_registeredExtensions.count());
    //        auto it = m_registeredExtensions.constBegin();

    //        errors->reserve(m_registeredExtensions.count());

    //        while (it != m_registeredExtensions.constEnd()) {
    //            it->importCallback(account).then(this, [promise, errors, counter](ImportResult &&result) mutable {
    //                if (result) {
    //                    errors->append(std::move(*result));
    //                }

    //                if ((--(*counter)) == 0) {
    //                    promise.finish(*errors);
    //                }
    //            });

    //            ++it;
    //        }

    //        if (m_registeredExtensions.isEmpty()) {
    //            promise.finish(*errors);
    //        }
    //    }

    return promise.task();
}

void QXmppAccountMigrationManager::registerMigrationDataInternal(std::type_index dataType, ImportCallback importFunc, ExportCallback exportFunc)
{
}

void QXmppAccountMigrationManager::unregisterMigrationDataInternal(std::type_index dataType)
{
}
