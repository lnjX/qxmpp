#pragma once

#include <QTimer>

template<typename Notifier, typename Sender = typename QtPrivate::FunctionPointer<Notifier>::Object, typename std::enable_if<QtPrivate::FunctionPointer<Notifier>::IsPointerToMemberFunction, Notifier> * = nullptr>
class QXmppWait : public QObject
{
public:
    using Then = std::function<void(bool)>;

    QXmppWait(Sender *sender, Notifier notifier, QObject *parent = nullptr)
        : QObject(parent), m_sender(sender), m_notifier(notifier)
    {
        m_timer.setSingleShot(true);
    }

    ~QXmppWait() override
    {
        m_timer.stop();
        QObject::disconnect(m_conn);
    }

    void then(Then func, int msecs = 5000)
    {
        Q_ASSERT(!m_timer.isActive());
        Q_ASSERT(!m_conn);

        m_conn = connect(m_sender.data(), m_notifier, this, [this, func]() mutable {
            m_timer.stop();
            func(true);
            deleteLater();
        });

        m_timer.callOnTimeout(this, [this, func]() mutable {
            func(false);
            deleteLater();
        });

        m_timer.start(msecs);
    }

private:
    QTimer m_timer;
    QPointer<Sender> const m_sender = nullptr;
    Notifier const m_notifier = nullptr;
    QMetaObject::Connection m_conn;
};
