// SPDX-FileCopyrightText: 2024 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef QXMPPMAMMETADATA_H
#define QXMPPMAMMETADATA_H

#include "QXmppGlobal.h"

#include <QDateTime>
#include <QSharedDataPointer>

class QDomElement;
class QXmlStreamWriter;
class QXmppMamMetadataData;

class QXmppMamMetadata
{
public:
    struct MessageReference {
        QString id;
        QDateTime timestamp;
    };

    struct Range {
        MessageReference start;
        MessageReference end;
    };

    QXmppMamMetadata();
    QXMPP_PRIVATE_DECLARE_RULE_OF_SIX(QXmppMamMetadata)

    std::optional<Range> archiveRange() const;
    void setArchiveRange(const std::optional<Range> &);

    static std::optional<QXmppMamMetadata> fromDom(const QDomElement &el);
    void toXml(QXmlStreamWriter *w) const;

private:
    QSharedDataPointer<QXmppMamMetadataData> d;
};

#endif  // QXMPPMAMMETADATA_H
