// SPDX-FileCopyrightText: 2023 Filipe Azevedo <pasnox@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef QXMPPACCOUNTMIGRATIONMANAGER_H
#define QXMPPACCOUNTMIGRATIONMANAGER_H

#include "QXmppClientExtension.h"
#include "QXmppError.h"
#include "QXmppRosterIq.h"
#include "QXmppVCardIq.h"

#include <QMap>

class QXMPP_EXPORT QXmppAccountMigrationManager : public QXmppClientExtension
{
    Q_OBJECT

public:
    QXmppAccountMigrationManager();
    ~QXmppAccountMigrationManager() override;

    QXmppTask<Account> exportData();
    QXmppTask<QList<QXmppError>> importData(const Account &account);

    template<typename Extension>
    void registerExtension(Extension *extension)
    {
        Q_ASSERT(!m_registeredExtensions.contains(extension));
        m_registeredExtensions.insert(extension, RegisteredExtension {
                                                     std::bind(&Extension::exportData, extension, std::placeholders::_1),
                                                     std::bind(&Extension::importData, extension, std::placeholders::_1),
                                                 });
    }

private:
    struct RegisteredExtension
    {
        ExportCallback exportCallback;
        ImportCallback importCallback;
    };

    QMap<QObject *, RegisteredExtension> m_registeredExtensions;
};

#endif
