/*
 * Copyright (C) 2008-2019 The QXmpp developers
 *
 * Author:
 *  Linus Jahn <lnj@kaidan.im>
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

#include <QFileInfo>
#include <QMimeDatabase>

#include "QXmppClient.h"
#include "QXmppConstants_p.h"
#include "QXmppDiscoveryManager.h"
#include "QXmppDiscoveryIq.h"
#include "QXmppDataForm.h"
#include "QXmppHttpUploadIq.h"

#include "QXmppUploadRequestManager.h"

class QXmppUploadServicePrivate : public QSharedData
{
public:
    QString jid;
    qint64 sizeLimit = -1;
};

QXmppUploadService::QXmppUploadService()
    : d(new QXmppUploadServicePrivate)
{
}

/// Returns the JID of the HTTP File Upload service.

QString QXmppUploadService::jid() const
{
    return d->jid;
}

/// Sets the JID of the HTTP File Upload service.

void QXmppUploadService::setJid(const QString &jid)
{
    d->jid = jid;
}

/// Returns the size limit of files that can be uploaded to this upload
/// service.

qint64 QXmppUploadService::sizeLimit() const
{
    return d->sizeLimit;
}

/// Sets the size limit of files that can be uploaded to this upload service.

void QXmppUploadService::setSizeLimit(qint64 sizeLimit)
{
    d->sizeLimit = sizeLimit;
}

class QXmppUploadRequestManagerPrivate : public QSharedData
{
public:
    QList<QXmppUploadService> uploadServices;
};

QXmppUploadRequestManager::QXmppUploadRequestManager()
    : d(new QXmppUploadRequestManagerPrivate)
{
}

/// Requests an upload slot from the server.
///
/// \param file The info of the file to be uploaded.
/// \param uploadService The HTTP File Upload service that is used to request
///                      the upload slot. If this is empty, the first
///                      discovered one is used.
/// \return The id of the sent IQ. If sendPacket() isn't successful or there
///         has no upload service been found, an empty string will be returned.

QString QXmppUploadRequestManager::requestUploadSlot(const QFileInfo &file,
                                                     const QString &uploadService)
{
    return requestUploadSlot(file, file.fileName(), uploadService);
}

/// Requests an upload slot from the server.
///
/// \param file The info of the file to be uploaded.
/// \param customFileName This name is used instead of the file's name for
///                       requesting the upload slot.
/// \param uploadService The HTTP File Upload service that is used to request
///                      the upload slot. If this is empty, the first
///                      discovered one is used.
/// \return The id of the sent IQ. If sendPacket() isn't successful or there
///         has no upload service been found, an empty string will be returned.

QString QXmppUploadRequestManager::requestUploadSlot(const QFileInfo &file,
                                                     const QString &customFileName,
                                                     const QString &uploadService)
{
    return requestUploadSlot(customFileName, file.size(),
                             QMimeDatabase().mimeTypeForFile(file),
                             uploadService);
}

/// Requests an upload slot from the server.
///
/// \param fileName The name of the file to be uploaded. This may be used to
///                 generate the URL by the server.
/// \param fileSize The size of the file to be uploaded. The server may reject
///                 too large files.
/// \param mimeType The content-type of the file. This can be used by the
///                 server to set the HTTP MIME-type of the URL.
/// \param uploadService The HTTP File Upload service that is used to request
///                      the upload slot. If this is empty, the first
///                      discovered one is used.
/// \return The id of the sent IQ. If sendPacket() isn't successful or there
///         has no upload service been found, an empty string will be returned.

QString QXmppUploadRequestManager::requestUploadSlot(const QString &fileName,
                                                     qint64 fileSize,
                                                     const QMimeType &mimeType,
                                                     const QString &uploadService)
{
    if (!serviceFound())
        return "";

    QXmppHttpUploadRequestIq iq;
    iq.setTo(uploadService.isEmpty() ? d->uploadServices.first().jid()
                                     : uploadService);
    iq.setType(QXmppIq::Get);
    iq.setFileName(fileName);
    iq.setSize(fileSize);
    iq.setContentType(mimeType);

    if (client()->sendPacket(iq))
        return iq.id();
    return "";
}

/// Returns true, if an HTTP File Upload service has been discovered.

bool QXmppUploadRequestManager::serviceFound() const
{
    return !d->uploadServices.isEmpty();
}

/// Returns all discovered HTTP File Upload services.

QList<QXmppUploadService> QXmppUploadRequestManager::uploadServices() const
{
    return d->uploadServices;
}

bool QXmppUploadRequestManager::handleStanza(const QDomElement &element)
{
    if (QXmppHttpUploadSlotIq::isHttpUploadSlotIq(element)) {
        QXmppHttpUploadSlotIq slot;
        slot.parse(element);

        emit slotReceived(slot);
        return true;
    } else if (QXmppHttpUploadRequestIq::isHttpUploadRequestIq(element)) {
        QXmppHttpUploadRequestIq requestError;
        requestError.parse(element);

        emit requestFailed(requestError);
        return true;
    }
    return false;
}

void QXmppUploadRequestManager::handleDiscoInfo(const QXmppDiscoveryIq &iq)
{
    if (!iq.features().contains(ns_http_upload))
        return;

    for (const QXmppDiscoveryIq::Identity &identity : iq.identities()) {
        if (identity.category() == "store" && identity.type() == "file") {
            QXmppUploadService service;
            service.setJid(iq.from());

            // get size limit
            bool isFormNsCorrect = false;
            for (const QXmppDataForm::Field &field : iq.form().fields()) {
                if (field.key() == "FORM_TYPE") {
                    isFormNsCorrect = field.value() == ns_http_upload;
                } else if (isFormNsCorrect && field.key() == "max-file-size") {
                    service.setSizeLimit(field.value().toLongLong());
                }
            }

            d->uploadServices.append(service);
            emit serviceFoundChanged();
        }
    }
    return;
}

void QXmppUploadRequestManager::setClient(QXmppClient *client)
{
    QXmppClientExtension::setClient(client);
    // connect to service discovery manager
    auto *disco = client->findExtension<QXmppDiscoveryManager>();
    if (disco) {
        // scan info of all entities for upload services
        connect(disco, &QXmppDiscoveryManager::infoReceived,
                this, &QXmppUploadRequestManager::handleDiscoInfo);

        // on client disconnect remove all upload services
        connect(client, &QXmppClient::disconnected, [this] () {
            d->uploadServices.clear();
            emit serviceFoundChanged();
        });
    }
}
