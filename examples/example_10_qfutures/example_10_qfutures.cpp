#include "QXmppClient.h"
#include "QXmppMessage.h"

#include <QCoreApplication>
#include <QFuture>
#include <QFutureWatcher>

template<class T>
void futureOnFinished(const QFuture<T> &future, const std::function<void(QFuture<T>)> &handler)
{
    auto *watcher = new QFutureWatcher<T>();
    QObject::connect(watcher, &QFutureWatcher<T>::finished, [=]() {
        handler(future);
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}

class Client : public QXmppClient
{
    Q_OBJECT

public:
    Client(QObject *parent = nullptr)
        : QXmppClient(parent)
    {
        connect(this, &QXmppClient::connected, this, &Client::onConnected);
    }

    Q_SLOT void onConnected()
    {
        QXmppMessage msg;
        msg.setTo("jbb@kaidan.im");
        msg.setBody("You successfully received SPAM.");

        qDebug() << "SEND";
        auto future = send(msg);
        futureOnFinished<QXmpp::PacketState>(future, [](const QFuture<QXmpp::PacketState> &future) {
            qDebug() << future.results();
        });

        //        auto *watcher = new QFutureWatcher<QXmppStream::SendPacketState>();
        //        connect(watcher, &QFutureWatcher<QXmppStream::SendPacketState>::finished, this, [=]() {
        //            qDebug() << watcher->future().results();

        //            watcher->deleteLater();
        //        });
        //        watcher->setFuture(future);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Client client;
    client.logger()->setLoggingType(QXmppLogger::StdoutLogging);
    client.connectToServer("", "");

    return app.exec();
}

#include "example_10_qfutures.moc"
