/*
 * Copyright (C) 2008-2010 The QXmpp developers
 *
 * Authors:
 *	Ian Reinhart Geiser
 *	Jeremy Lainé
 *
 * Source:
 *	http://code.google.com/p/qxmpp
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

#ifndef QXMPPRPCIQ_H
#define QXMPPRPCIQ_H

#include "QXmppIq.h"
#include <QVariant>

class QXmlStreamWriter;
class QDomElement;

namespace XMLRPC
{
    void marshall( QXmlStreamWriter *writer, const QVariant &value);
    QVariant demarshall(const QDomElement &elem, QStringList &errors);
}

/// \brief The QXmppRpcResponseIq class represents an IQ used to carry
/// an RPC response as specified by XEP-0009: Jabber-RPC.
///
/// \ingroup Stanzas

class QXmppRpcResponseIq : public QXmppIq
{
public:
    QXmppRpcResponseIq();

    int faultCode() const;
    void setFaultCode(int faultCode);

    QString faultString() const;
    void setFaultString(const QString &faultString);

    QVariantList values() const;
    void setValues(const QVariantList &values);

    static bool isRpcResponseIq(const QDomElement &element);

protected:
    /// \cond
    void parseElementFromChild(const QDomElement &element);
    void toXmlElementFromChild(QXmlStreamWriter *writer) const;
    /// \endcond

private:
    int m_faultCode;
    QString m_faultString;
    QVariantList m_values;
};

/// \brief The QXmppRpcInvokeIq class represents an IQ used to carry
/// an RPC invocation as specified by XEP-0009: Jabber-RPC.
///
/// \ingroup Stanzas

class QXmppRpcInvokeIq : public QXmppIq
{
public:
    QXmppRpcInvokeIq();

    QString method() const;
    void setMethod( const QString &method );

    QVariantList arguments() const;
    void setArguments(const QVariantList &arguments);

    static bool isRpcInvokeIq(const QDomElement &element);

protected:
    /// \cond
    void parseElementFromChild(const QDomElement &element);
    void toXmlElementFromChild(QXmlStreamWriter *writer) const;
    /// \endcond

private:
    QVariantList m_arguments;
    QString m_method;

    friend class QXmppRpcErrorIq;
};

class QXmppRpcErrorIq : public QXmppIq
{
public:
    QXmppRpcErrorIq();

    QXmppRpcInvokeIq query() const;
    void setQuery(const QXmppRpcInvokeIq &query);

    static bool isRpcErrorIq(const QDomElement &element);

protected:
    /// \cond
    void parseElementFromChild(const QDomElement &element);
    void toXmlElementFromChild(QXmlStreamWriter *writer) const;
    /// \endcond

private:
    QXmppRpcInvokeIq m_query;
};

#endif // QXMPPRPCIQ_H
