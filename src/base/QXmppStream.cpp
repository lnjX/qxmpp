/*
 * Copyright (C) 2008-2020 The QXmpp developers
 *
 * Authors:
 *  Manjeet Dahiya
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

#include "QXmppStream.h"

#include "QXmppConstants_p.h"
#include "QXmppLogger.h"
#include "QXmppStanza.h"
#include "QXmppStreamManagement_p.h"
#include "QXmppUtils.h"

#include <QBuffer>
#include <QDomDocument>
#include <QHostAddress>
#include <QMap>
#include <QRegularExpression>
#include <QSslSocket>
#include <QStringBuilder>
#include <QStringList>
#include <QTime>
#include <QXmlStreamWriter>

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
static bool randomSeeded = false;
#endif

class QXmppStreamPrivate
{
public:
    QXmppStreamPrivate();

    QSslSocket *socket;
    QString dataBuffer;

    // incoming stream state
    QXmlStreamReader reader;
    QXmlStreamNamespaceDeclarations streamNamespaces;
    QString stanzaXmlWrapper;
    qint64 processedIndex;

    bool streamManagementEnabled;
    QMap<unsigned, QByteArray> unacknowledgedStanzas;
    unsigned lastOutgoingSequenceNumber;
    unsigned lastIncomingSequenceNumber;
};

QXmppStreamPrivate::QXmppStreamPrivate()
    : socket(nullptr), streamManagementEnabled(false), lastOutgoingSequenceNumber(0), lastIncomingSequenceNumber(0)
{
}

///
/// Constructs a base XMPP stream.
///
/// \param parent
///
QXmppStream::QXmppStream(QObject *parent)
    : QXmppLoggable(parent),
      d(new QXmppStreamPrivate)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    // Make sure the random number generator is seeded
    if (!randomSeeded) {
        qsrand(QTime(0, 0, 0).msecsTo(QTime::currentTime()) ^ reinterpret_cast<quintptr>(this));
        randomSeeded = true;
    }
#endif
}

///
/// Destroys a base XMPP stream.
///
QXmppStream::~QXmppStream()
{
    delete d;
}

///
/// Disconnects from the remote host.
///
void QXmppStream::disconnectFromHost()
{
    d->streamManagementEnabled = false;
    if (d->socket) {
        if (d->socket->state() == QAbstractSocket::ConnectedState) {
            sendData(QByteArrayLiteral("</stream:stream>"));
            d->socket->flush();
        }
        // FIXME: according to RFC 6120 section 4.4, we should wait for
        // the incoming stream to end before closing the socket
        d->socket->disconnectFromHost();
    }
}

///
/// Handles a stream start event, which occurs when the underlying transport
/// becomes ready (socket connected, encryption started).
///
/// If you redefine handleStart(), make sure to call the base class's method.
///
void QXmppStream::handleStart()
{
    d->streamManagementEnabled = false;
    d->dataBuffer.clear();
    d->reader.clear();
    d->processedIndex = 0;
    d->stanzaXmlWrapper.clear();
}

///
/// Returns true if the stream is connected.
///
bool QXmppStream::isConnected() const
{
    return d->socket &&
        d->socket->state() == QAbstractSocket::ConnectedState;
}

///
/// Sends raw data to the peer.
///
/// \param data
///
bool QXmppStream::sendData(const QByteArray &data)
{
    logSent(QString::fromUtf8(data));
    if (!d->socket || d->socket->state() != QAbstractSocket::ConnectedState)
        return false;
    return d->socket->write(data) == data.size();
}

///
/// Sends an XMPP packet to the peer.
///
/// \param packet
///
bool QXmppStream::sendPacket(const QXmppStanza &packet)
{
    // prepare packet
    QByteArray data;
    QXmlStreamWriter xmlStream(&data);
    packet.toXml(&xmlStream);

    bool isXmppStanza = packet.isXmppStanza();
    if (isXmppStanza && d->streamManagementEnabled)
        d->unacknowledgedStanzas[++d->lastOutgoingSequenceNumber] = data;

    // send packet
    bool success = sendData(data);
    if (isXmppStanza)
        sendAcknowledgementRequest();
    return success;
}

///
/// Returns the QSslSocket used for this stream.
///
QSslSocket *QXmppStream::socket() const
{
    return d->socket;
}

///
/// Sets the QSslSocket used for this stream.
///
void QXmppStream::setSocket(QSslSocket *socket)
{
    d->socket = socket;
    if (!d->socket)
        return;

    // socket events
    connect(socket, &QAbstractSocket::connected, this, &QXmppStream::_q_socketConnected);
    connect(socket, &QSslSocket::encrypted, this, &QXmppStream::_q_socketEncrypted);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(socket, &QSslSocket::errorOccurred, this, &QXmppStream::_q_socketError);
#else
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error), this, &QXmppStream::_q_socketError);
#endif
    connect(socket, &QIODevice::readyRead, this, &QXmppStream::_q_socketReadyRead);
}

void QXmppStream::_q_socketConnected()
{
    info(QStringLiteral("Socket connected to %1 %2").arg(d->socket->peerAddress().toString(), QString::number(d->socket->peerPort())));
    handleStart();
}

void QXmppStream::_q_socketEncrypted()
{
    debug("Socket encrypted");
    handleStart();
}

void QXmppStream::_q_socketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    warning(QStringLiteral("Socket error: ") + socket()->errorString());
}

void QXmppStream::_q_socketReadyRead()
{
    processData(QString::fromUtf8(d->socket->readAll()));
}

///
/// Enables Stream Management acks / reqs (\xep{0198}).
///
/// \param resetSequenceNumber Indicates if the sequence numbers should be
/// reset. This must be done if the stream is not resumed.
///
/// \since QXmpp 1.0
///
void QXmppStream::enableStreamManagement(bool resetSequenceNumber)
{
    d->streamManagementEnabled = true;

    if (resetSequenceNumber) {
        d->lastOutgoingSequenceNumber = 0;
        d->lastIncomingSequenceNumber = 0;

        // resend unacked stanzas
        if (!d->unacknowledgedStanzas.empty()) {
            QMap<unsigned, QByteArray> oldUnackedStanzas = d->unacknowledgedStanzas;
            d->unacknowledgedStanzas.clear();
            for (QMap<unsigned, QByteArray>::iterator it = oldUnackedStanzas.begin(); it != oldUnackedStanzas.end(); ++it) {
                d->unacknowledgedStanzas[++d->lastOutgoingSequenceNumber] = it.value();
                sendData(it.value());
            }
            sendAcknowledgementRequest();
        }
    } else {
        // resend unacked stanzas
        if (!d->unacknowledgedStanzas.empty()) {
            for (QMap<unsigned, QByteArray>::iterator it = d->unacknowledgedStanzas.begin(); it != d->unacknowledgedStanzas.end(); ++it)
                sendData(it.value());
            sendAcknowledgementRequest();
        }
    }
}

///
/// Returns the sequence number of the last incoming stanza (\xep{0198}).
///
/// \since QXmpp 1.0
///
unsigned QXmppStream::lastIncomingSequenceNumber() const
{
    return d->lastIncomingSequenceNumber;
}

///
/// Sets the last acknowledged sequence number for outgoing stanzas
/// (\xep{0198}).
///
/// \since QXmpp 1.0
///
void QXmppStream::setAcknowledgedSequenceNumber(unsigned sequenceNumber)
{
    for (QMap<unsigned, QByteArray>::iterator it = d->unacknowledgedStanzas.begin(); it != d->unacknowledgedStanzas.end();) {
        if (it.key() <= sequenceNumber)
            it = d->unacknowledgedStanzas.erase(it);
        else
            ++it;
    }
}

///
/// Handles an incoming acknowledgement from \xep{0198}.
///
/// \param element
///
/// \since QXmpp 1.0
///
void QXmppStream::handleAcknowledgement(QDomElement &element)
{
    if (!d->streamManagementEnabled)
        return;

    QXmppStreamManagementAck ack;
    ack.parse(element);
    setAcknowledgedSequenceNumber(ack.seqNo());
}

///
/// Sends an acknowledgement as defined in \xep{0198}.
///
/// \since QXmpp 1.0
///
void QXmppStream::sendAcknowledgement()
{
    if (!d->streamManagementEnabled)
        return;

    // prepare packet
    QByteArray data;
    QXmlStreamWriter xmlStream(&data);
    QXmppStreamManagementAck ack(d->lastIncomingSequenceNumber);
    ack.toXml(&xmlStream);

    // send packet
    sendData(data);
}

///
/// Sends an acknowledgement request as defined in \xep{0198}.
///
/// \since QXmpp 1.0
///
void QXmppStream::sendAcknowledgementRequest()
{
    if (!d->streamManagementEnabled)
        return;

    // prepare packet
    QByteArray data;
    QXmlStreamWriter xmlStream(&data);
    QXmppStreamManagementReq::toXml(&xmlStream);

    // send packet
    sendData(data);
}

void QXmppStream::processData(const QString &newData)
{
    d->dataBuffer.append(newData);
    d->reader.addData(newData);

    auto currentToken = d->reader.readNext();
    while (true) {
        qDebug() << "PROCESS DATA" << currentToken;
        switch (currentToken) {
        case QXmlStreamReader::StartDocument:
            // XML document starts, we need the next token to see if it's a
            // correct stream
            d->processedIndex = d->reader.characterOffset();
            d->dataBuffer = d->dataBuffer.mid(d->reader.characterOffset());
            qDebug() << "StartDocument: processedIndex=" << d->processedIndex;
            break;
        case QXmlStreamReader::StartElement: {
            // special case: stream start
            qDebug() << "StartElement:" << d->reader.name() << d->reader.namespaceUri();
            if (d->reader.name() == QLatin1String("stream") &&
                d->reader.namespaceUri() == QLatin1String(ns_stream)) {
                const auto tagEnd = d->reader.characterOffset() - d->processedIndex;
                auto tagData = d->dataBuffer.left(tagEnd);
                // insert '/' to close the tag
                tagData.insert(tagData.size() - 1, u'/');

                QDomDocument doc;
                doc.setContent(tagData, true);

                handleStream(doc.documentElement());

                // remove from buffer and update processed index
                d->dataBuffer = d->dataBuffer.mid(tagEnd + 1);
                d->processedIndex = d->reader.characterOffset();

                // When parsing the stanzas to DOM we use a wrapper that
                // contains the namespaces from the stream.
                //
                // This is required for successful parsing of the stanzas using
                // a QDomDocument. Especially for <stream:features/>, but also
                // for having the correct namespaces (e.g. 'jabber:client') when
                // parsing other stanzas.
                //
                // We need to save the new namespace(s) now.
                d->stanzaXmlWrapper = createStanzaXmlWrapper(d->reader);
                break;
            }

            // skip element; we only want to know where the stanza ends
            // further processing happens in DOM
            d->reader.skipCurrentElement();

            // element processing and errors are handled in the next loop run
            currentToken = d->reader.tokenType();
            continue;
        }
        case QXmlStreamReader::EndElement: {
            qDebug() << "\tEndElement" << d->reader.name();
            qDebug() << d->reader.error();

            if (d->reader.hasError() && d->reader.error() == QXmlStreamReader::PrematureEndOfDocumentError) {
                return;
            }

            // special case: stream end
            if (d->reader.name() == QLatin1String("stream") &&
                d->reader.namespaceUri() == QLatin1String(ns_stream)) {
                disconnectFromHost();
                return;
            }

            const auto bufferElementEnd = d->reader.characterOffset() - d->processedIndex;
            const auto stanzaData = d->dataBuffer.left(bufferElementEnd);

            // process stanza (will use QDom-based parsing)
            processReceivedStanza(stanzaData);

            // remove stanza data from data buffer
            d->dataBuffer = d->dataBuffer.mid(bufferElementEnd + 1);
            // increase processed index
            d->processedIndex = d->reader.characterOffset();

            break;
        }
        case QXmlStreamReader::Invalid:
            if (d->reader.error() == QXmlStreamReader::PrematureEndOfDocumentError) {
                // the stanza has not been received completely yet
                // wait for more data
                qDebug() << "PrematureEndOfDocument";
                return;
            } else {
                // fatal error: disconnect?
            }
            break;
        case QXmlStreamReader::EndDocument:
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }

        if (d->reader.atEnd()) {
            break;
        }

        currentToken = d->reader.readNext();
    }
}

void QXmppStream::processReceivedStanza(const QString &stanzaData)
{
    // log only the pure stanza data
    logReceived(stanzaData);

    QDomDocument doc;
    // parse the wrapped stanza (required for namespaces)
    doc.setContent(d->stanzaXmlWrapper.arg(stanzaData), true);
    auto stanzaElement = doc.documentElement().firstChildElement();

    if (QXmppStreamManagementAck::isStreamManagementAck(stanzaElement)) {
        handleAcknowledgement(stanzaElement);
    } else if (QXmppStreamManagementReq::isStreamManagementReq(stanzaElement)) {
        sendAcknowledgement();
    } else {
        handleStanza(stanzaElement);

        if (stanzaElement.tagName() == QLatin1String("message") ||
            stanzaElement.tagName() == QLatin1String("presence") ||
            stanzaElement.tagName() == QLatin1String("iq")) {
            d->lastIncomingSequenceNumber++;
        }
    }
}

QString QXmppStream::createStanzaXmlWrapper(const QXmlStreamReader &reader)
{
    QString output;
    QXmlStreamWriter wrapperWriter(&output);
    wrapperWriter.writeStartDocument();
    wrapperWriter.writeStartElement(QStringLiteral("wrapper"));
    // write default namespace (e.g. 'jabber:client')
    wrapperWriter.writeDefaultNamespace(reader.namespaceUri().toString());

    // write other definied namespaces (e.g. 'stream:xmlns="..."')
    const auto xmlNamespaces = reader.namespaceDeclarations();
    for (const auto &xmlNamespace : xmlNamespaces) {
        wrapperWriter.writeNamespace(xmlNamespace.namespaceUri().toString(),
                                     xmlNamespace.prefix().toString());
    }

    // will be used for inserting the stanza
    wrapperWriter.writeCharacters("%1");
    wrapperWriter.writeEndElement();
    wrapperWriter.writeEndDocument();

    return output;
}
