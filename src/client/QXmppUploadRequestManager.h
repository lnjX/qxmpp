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

#ifndef QXMPPUPLOADREQUESTMANAGER_H
#define QXMPPUPLOADREQUESTMANAGER_H

#include <QXmppClientExtension.h>

class QFileInfo;
class QMimeType;
class QXmppHttpUploadRequestIq;
class QXmppHttpUploadSlotIq;
class QXmppUploadServicePrivate;
class QXmppUploadRequestManagerPrivate;

class QXMPP_EXPORT QXmppUploadService
{
public:
    QXmppUploadService();

    QString jid() const;
    void setJid(const QString &jid);

    qint64 sizeLimit() const;
    void setSizeLimit(qint64 sizeLimit);

private:
    QXmppUploadServicePrivate *d;
};

/// \class QXmppUploadRequestManager This class implements the core of
/// XEP-0369: HTTP File Upload.
///
/// It handles the discovery of QXmppUploadServices and can send upload
/// requests and outputs the upload slots. It doesn't do the actual upload via.
/// HTTP.

class QXMPP_EXPORT QXmppUploadRequestManager : public QXmppClientExtension
{
    Q_OBJECT
    Q_PROPERTY(bool serviceFound READ serviceFound NOTIFY serviceFoundChanged)

public:
    QXmppUploadRequestManager();

    QString requestUploadSlot(const QFileInfo &file,
                              const QString &uploadService = QString());
    QString requestUploadSlot(const QFileInfo &file,
                              const QString &customFileName,
                              const QString &uploadService = QString());
    QString requestUploadSlot(const QString &fileName, qint64 fileSize,
                              const QMimeType &mimeType,
                              const QString &uploadService = QString());

    bool serviceFound() const;

    QList<QXmppUploadService> uploadServices() const;

signals:
    /// Emitted when an upload slot was received.
    void slotReceived(const QXmppHttpUploadSlotIq &slot);

    /// Emitted when the slot request failed.
    void requestFailed(const QXmppHttpUploadRequestIq &request);

    void serviceFoundChanged();

protected:
    void setClient(QXmppClient *client) override;
    bool handleStanza(const QDomElement &stanza) override;

private:
    void handleDiscoInfo(const QXmppDiscoveryIq &iq);

    QXmppUploadRequestManagerPrivate *d;
};

#endif // QXMPPUPLOADREQUESTMANAGER_H
