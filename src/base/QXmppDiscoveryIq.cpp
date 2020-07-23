/*
 * Copyright (C) 2008-2020 The QXmpp developers
 *
 * Author:
 *  Jeremy Lainé
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

#include "QXmppDiscoveryIq.h"

#include "QXmppConstants_p.h"
#include "QXmppUtils.h"

#include <QBitArray>
#include <QCryptographicHash>
#include <QDomElement>
#include <QSharedData>

static const QStringList FEATURE_STRINGS = {
    ns_disco_info,
    ns_disco_items,
    ns_extended_addressing,
    ns_muc,
    ns_muc_admin,
    ns_muc_owner,
    ns_muc_user,
    ns_vcard,
    ns_rsm,
};

static bool identityLessThan(const QXmppDiscoveryIq::Identity &i1, const QXmppDiscoveryIq::Identity &i2)
{
    if (i1.category() < i2.category())
        return true;
    else if (i1.category() > i2.category())
        return false;

    if (i1.type() < i2.type())
        return true;
    else if (i1.type() > i2.type())
        return false;

    if (i1.language() < i2.language())
        return true;
    else if (i1.language() > i2.language())
        return false;

    if (i1.name() < i2.name())
        return true;
    else if (i1.name() > i2.name())
        return false;

    return false;
}

class QXmppDiscoveryIdentityPrivate : public QSharedData
{
public:
    QString category;
    QString language;
    QString name;
    QString type;
};

QXmppDiscoveryIq::Identity::Identity()
    : d(new QXmppDiscoveryIdentityPrivate)
{
}

QXmppDiscoveryIq::Identity::Identity(const QXmppDiscoveryIq::Identity &other) = default;

QXmppDiscoveryIq::Identity::~Identity() = default;

QXmppDiscoveryIq::Identity &QXmppDiscoveryIq::Identity::operator=(const QXmppDiscoveryIq::Identity &) = default;

QString QXmppDiscoveryIq::Identity::category() const
{
    return d->category;
}

void QXmppDiscoveryIq::Identity::setCategory(const QString &category)
{
    d->category = category;
}

QString QXmppDiscoveryIq::Identity::language() const
{
    return d->language;
}

void QXmppDiscoveryIq::Identity::setLanguage(const QString &language)
{
    d->language = language;
}

QString QXmppDiscoveryIq::Identity::name() const
{
    return d->name;
}

void QXmppDiscoveryIq::Identity::setName(const QString &name)
{
    d->name = name;
}

QString QXmppDiscoveryIq::Identity::type() const
{
    return d->type;
}

void QXmppDiscoveryIq::Identity::setType(const QString &type)
{
    d->type = type;
}

class QXmppDiscoveryItemPrivate : public QSharedData
{
public:
    QString jid;
    QString name;
    QString node;
};

QXmppDiscoveryIq::Item::Item()
    : d(new QXmppDiscoveryItemPrivate)
{
}

QXmppDiscoveryIq::Item::Item(const QXmppDiscoveryIq::Item &) = default;

QXmppDiscoveryIq::Item::~Item() = default;

QXmppDiscoveryIq::Item &QXmppDiscoveryIq::Item::operator=(const QXmppDiscoveryIq::Item &) = default;

QString QXmppDiscoveryIq::Item::jid() const
{
    return d->jid;
}

void QXmppDiscoveryIq::Item::setJid(const QString &jid)
{
    d->jid = jid;
}

QString QXmppDiscoveryIq::Item::name() const
{
    return d->name;
}

void QXmppDiscoveryIq::Item::setName(const QString &name)
{
    d->name = name;
}

QString QXmppDiscoveryIq::Item::node() const
{
    return d->node;
}

void QXmppDiscoveryIq::Item::setNode(const QString &node)
{
    d->node = node;
}

class QXmppDiscoveryIqPrivate : public QSharedData
{
public:
    QBitArray features = QBitArray(QXmppDiscoveryIq::FEATURES_COUNT);
    QStringList customFeatures;
    QList<QXmppDiscoveryIq::Identity> identities;
    QList<QXmppDiscoveryIq::Item> items;
    QXmppDataForm form;
    QString queryNode;
    enum QXmppDiscoveryIq::QueryType queryType;
};

QXmppDiscoveryIq::QXmppDiscoveryIq()
    : d(new QXmppDiscoveryIqPrivate)
{
}

std::optional<QXmppDiscoveryIq::Feature> QXmppDiscoveryIq::featureFromString(const QString &featureString)
{
    auto index = FEATURE_STRINGS.indexOf(featureString);
    if (index >= 0) {
        return Feature(index);
    }
    return std::nullopt;
}

QString QXmppDiscoveryIq::featureToString(QXmppDiscoveryIq::Feature feature)
{
    return FEATURE_STRINGS.at(int(feature));
}

QXmppDiscoveryIq::QXmppDiscoveryIq(const QXmppDiscoveryIq &) = default;

QXmppDiscoveryIq::~QXmppDiscoveryIq() = default;

QXmppDiscoveryIq &QXmppDiscoveryIq::operator=(const QXmppDiscoveryIq &) = default;

QStringList QXmppDiscoveryIq::features() const
{
    QStringList strings;
    for (int i = 0; i < d->features.size(); i++) {
        if (d->features.at(i)) {
            strings << FEATURE_STRINGS.at(i);
        }
    }

    return strings << d->customFeatures;
}

void QXmppDiscoveryIq::setFeatures(const QStringList &features)
{
    clearFeatures();

    for (const auto &featureString : features) {
        addFeature(featureString);
    }
}

void QXmppDiscoveryIq::setFeatures(const QXmppDiscoveryIq::Features &features)
{
    clearFeatures();

    for (const auto &feature : features) {
        d->features.setBit(int(feature));
    }
}

bool QXmppDiscoveryIq::hasFeature(QXmppDiscoveryIq::Feature feature) const
{
    return d->features.at(int(feature));
}

bool QXmppDiscoveryIq::hasFeature(const QString &featureString) const
{
    // check custom features first (should in most cases use less comparisons)
    if (d->customFeatures.contains(featureString)) {
        return true;
    }

    if (auto feature = featureFromString(featureString)) {
        return d->features.at(int(*feature));
    }
    return false;
}

void QXmppDiscoveryIq::addFeature(QXmppDiscoveryIq::Feature feature)
{
    d->features.setBit(int(feature));
}

void QXmppDiscoveryIq::addFeature(const QString &featureString)
{
    if (auto feature = featureFromString(featureString)) {
        d->features.setBit(int(*feature));
    } else {
        if (!d->customFeatures.contains(featureString)) {
            d->customFeatures << featureString;
        }
    }
}

void QXmppDiscoveryIq::removeFeature(QXmppDiscoveryIq::Feature feature)
{
    d->features.clearBit(int(feature));
}

void QXmppDiscoveryIq::removeFeature(const QString &featureString)
{
    auto removedCount = d->customFeatures.removeAll(featureString);
    if (!removedCount) {
        if (auto feature = featureFromString(featureString)) {
            d->features.clearBit(int(*feature));
        }
    }
}

void QXmppDiscoveryIq::clearFeatures()
{
    // clear() resizes the bit array to 0
    d->features.fill(false);
    d->customFeatures.clear();
}

QList<QXmppDiscoveryIq::Identity> QXmppDiscoveryIq::identities() const
{
    return d->identities;
}

void QXmppDiscoveryIq::setIdentities(const QList<QXmppDiscoveryIq::Identity> &identities)
{
    d->identities = identities;
}

QList<QXmppDiscoveryIq::Item> QXmppDiscoveryIq::items() const
{
    return d->items;
}

void QXmppDiscoveryIq::setItems(const QList<QXmppDiscoveryIq::Item> &items)
{
    d->items = items;
}

/// Returns the QXmppDataForm for this IQ, as defined by
/// \xep{0128}: Service Discovery Extensions.
///

QXmppDataForm QXmppDiscoveryIq::form() const
{
    return d->form;
}

/// Sets the QXmppDataForm for this IQ, as define by
/// \xep{0128}: Service Discovery Extensions.
///
/// \param form
///

void QXmppDiscoveryIq::setForm(const QXmppDataForm &form)
{
    d->form = form;
}

QString QXmppDiscoveryIq::queryNode() const
{
    return d->queryNode;
}

void QXmppDiscoveryIq::setQueryNode(const QString &node)
{
    d->queryNode = node;
}

enum QXmppDiscoveryIq::QueryType QXmppDiscoveryIq::queryType() const
{
    return d->queryType;
}

void QXmppDiscoveryIq::setQueryType(enum QXmppDiscoveryIq::QueryType type)
{
    d->queryType = type;
}

/// Calculate the verification string for \xep{0115}: Entity Capabilities

QByteArray QXmppDiscoveryIq::verificationString() const
{
    QString S;
    QList<QXmppDiscoveryIq::Identity> sortedIdentities = d->identities;
    std::sort(sortedIdentities.begin(), sortedIdentities.end(), identityLessThan);
    QStringList sortedFeatures = features();
    std::sort(sortedFeatures.begin(), sortedFeatures.end());
    sortedFeatures.removeDuplicates();
    for (const auto &identity : sortedIdentities)
        S += QString("%1/%2/%3/%4<").arg(identity.category(), identity.type(), identity.language(), identity.name());
    for (const auto &feature : sortedFeatures)
        S += feature + QLatin1String("<");

    if (!d->form.isNull()) {
        QMap<QString, QXmppDataForm::Field> fieldMap;
        for (const auto &field : d->form.fields()) {
            fieldMap.insert(field.key(), field);
        }

        if (fieldMap.contains("FORM_TYPE")) {
            const QXmppDataForm::Field field = fieldMap.take("FORM_TYPE");
            S += field.value().toString() + QLatin1String("<");

            QStringList keys = fieldMap.keys();
            std::sort(keys.begin(), keys.end());
            for (const auto &key : keys) {
                const QXmppDataForm::Field field = fieldMap.value(key);
                S += key + QLatin1String("<");
                if (field.value().canConvert<QStringList>()) {
                    QStringList list = field.value().toStringList();
                    list.sort();
                    S += list.join(QLatin1String("<"));
                } else {
                    S += field.value().toString();
                }
                S += QLatin1String("<");
            }
        } else {
            qWarning("QXmppDiscoveryIq form does not contain FORM_TYPE");
        }
    }

    QCryptographicHash hasher(QCryptographicHash::Sha1);
    hasher.addData(S.toUtf8());
    return hasher.result();
}

/// \cond
bool QXmppDiscoveryIq::isDiscoveryIq(const QDomElement &element)
{
    QDomElement queryElement = element.firstChildElement("query");
    return (queryElement.namespaceURI() == ns_disco_info ||
            queryElement.namespaceURI() == ns_disco_items);
}

void QXmppDiscoveryIq::parseElementFromChild(const QDomElement &element)
{
    QDomElement queryElement = element.firstChildElement("query");
    d->queryNode = queryElement.attribute("node");
    if (queryElement.namespaceURI() == ns_disco_items)
        d->queryType = ItemsQuery;
    else
        d->queryType = InfoQuery;

    QDomElement itemElement = queryElement.firstChildElement();
    while (!itemElement.isNull()) {
        if (itemElement.tagName() == "feature") {
            addFeature(itemElement.attribute("var"));
        } else if (itemElement.tagName() == "identity") {
            Identity identity;
            identity.setLanguage(itemElement.attribute("xml:lang"));
            identity.setCategory(itemElement.attribute("category"));
            identity.setName(itemElement.attribute("name"));
            identity.setType(itemElement.attribute("type"));

            // FIXME: for some reason the language does not found,
            // so we are forced to use QDomNamedNodeMap
            QDomNamedNodeMap m(itemElement.attributes());
            for (int i = 0; i < m.size(); ++i) {
                if (m.item(i).nodeName() == "xml:lang") {
                    identity.setLanguage(m.item(i).nodeValue());
                    break;
                }
            }

            d->identities.append(identity);
        } else if (itemElement.tagName() == "item") {
            QXmppDiscoveryIq::Item item;
            item.setJid(itemElement.attribute("jid"));
            item.setName(itemElement.attribute("name"));
            item.setNode(itemElement.attribute("node"));
            d->items.append(item);
        } else if (itemElement.tagName() == "x" &&
                   itemElement.namespaceURI() == ns_data) {
            d->form.parse(itemElement);
        }
        itemElement = itemElement.nextSiblingElement();
    }
}

void QXmppDiscoveryIq::toXmlElementFromChild(QXmlStreamWriter *writer) const
{
    writer->writeStartElement("query");
    writer->writeDefaultNamespace(
        d->queryType == InfoQuery ? ns_disco_info : ns_disco_items);
    helperToXmlAddAttribute(writer, "node", d->queryNode);

    if (d->queryType == InfoQuery) {
        for (const auto &identity : d->identities) {
            writer->writeStartElement("identity");
            helperToXmlAddAttribute(writer, "xml:lang", identity.language());
            helperToXmlAddAttribute(writer, "category", identity.category());
            helperToXmlAddAttribute(writer, "name", identity.name());
            helperToXmlAddAttribute(writer, "type", identity.type());
            writer->writeEndElement();
        }

        const auto &featureStrings = features();
        for (const auto &feature : featureStrings) {
            writer->writeStartElement("feature");
            helperToXmlAddAttribute(writer, "var", feature);
            writer->writeEndElement();
        }
    } else {
        for (const auto &item : d->items) {
            writer->writeStartElement("item");
            helperToXmlAddAttribute(writer, "jid", item.jid());
            helperToXmlAddAttribute(writer, "name", item.name());
            helperToXmlAddAttribute(writer, "node", item.node());
            writer->writeEndElement();
        }
    }

    d->form.toXml(writer);

    writer->writeEndElement();
}
/// \endcond
