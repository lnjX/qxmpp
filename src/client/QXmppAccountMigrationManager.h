// SPDX-FileCopyrightText: 2023 Filipe Azevedo <pasnox@gmail.com>
// SPDX-FileCopyrightText: 2023 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef QXMPPACCOUNTMIGRATIONMANAGER_H
#define QXMPPACCOUNTMIGRATIONMANAGER_H

#include "QXmppClientExtension.h"
#include "QXmppError.h"
#include "QXmppFutureUtils_p.h"
#include "QXmppTask.h"

#include <any>
#include <typeindex>

struct QXmppAccountDataPrivate;

class QXMPP_EXPORT QXmppAccountData
{
public:
    QXMPP_PRIVATE_DECLARE_RULE_OF_SIX(QXmppAccountData)

    static std::variant<QXmppAccountData, QXmppError> fromDom(const QDomElement &);
    void toXml(QXmlStreamWriter *) const;

    const QVector<std::any> &extensions() const;
    void setExtensions(const QVector<std::any> &);
    void addExtension(const std::any &);

    template<typename T>
    using ExtensionParser = std::variant<T, QXmppError> (*)(const QDomElement &);
    template<typename T>
    using ExtensionSerializer = void (*)(const T &, QXmlStreamWriter &);

    template<typename T, ExtensionParser<T> parse, ExtensionSerializer<T> serialize>
    static void registerExtension(QStringView tagName, QStringView xmlns)
    {
        using namespace std;
        using namespace QXmpp::Private;

        ExtensionParser<any> parseAny = [](const QDomElement &el) {
            return visit(overloaded {
                             [](T data) { return any(move(data)); },
                             [](QXmppError e) { return e; },
                         },
                         parse(el));
        };

        ExtensionSerializer<any> serializeAny = [](const any &data, QXmlStreamWriter *w) {
            return serialize(any_cast<T>(data), w);
        };

        registerExtensionInternal(type_index(typeid(T)), parseAny, serializeAny, tagName, xmlns);
    }

private:
    static void registerExtensionInternal(std::type_index, ExtensionParser<std::any>, ExtensionSerializer<std::any>, QStringView tagName, QStringView xmlns);

    QSharedDataPointer<QXmppAccountDataPrivate> d;
};

struct QXmppAccountMigrationManagerPrivate;

class QXMPP_EXPORT QXmppAccountMigrationManager : public QXmppClientExtension
{
    Q_OBJECT

public:
    using ExportResult = std::variant<QXmppAccountData, QXmppError>;
    using ImportResult = std::variant<QXmpp::Success, QXmppError>;

    QXmppAccountMigrationManager();
    ~QXmppAccountMigrationManager() override;

    QXmppTask<ExportResult> exportData();
    QXmppTask<ImportResult> importData(const QXmppAccountData &account);

    template<typename DataType, typename ImportFunc, typename ExportFunc>
    void registerExtension(ImportFunc importFunc, ExportFunc exportFunc);

    template<typename DataType>
    void unregisterExtension();

private:
    using ImportTask = QXmppTask<std::variant<QXmpp::Success, QXmppError>>;
    using ExportTask = QXmppTask<std::variant<std::any, QXmppError>>;
    using ImportCallback = std::function<ImportTask(std::any)>;
    using ExportCallback = std::function<ExportTask()>;

    void registerMigrationDataInternal(std::type_index dataType, ImportCallback, ExportCallback);
    void unregisterMigrationDataInternal(std::type_index dataType);

    std::unique_ptr<QXmppAccountMigrationManagerPrivate> d;
};

template<typename DataType, typename ImportFunc, typename ExportFunc>
void QXmppAccountMigrationManager::registerExtension(ImportFunc importFunc, ExportFunc exportFunc)
{
    using namespace QXmpp;
    using namespace QXmpp::Private;
    using namespace std;

    static_assert(is_constructible_v<std::function<QXmppTask<variant<Success, QXmppError>>(DataType)>, ImportFunc>);
    static_assert(is_constructible_v<std::function<QXmppTask<variant<DataType, QXmppError>>()>, ExportFunc>);
    // using X = decltype(importFunc(DataType()));
    // static_assert(is_invocable_v<ImportFunc (DataType)>);
    // static_assert(is_invocable_v<ExportFunc ()>);
    // static_assert(is_same_v<result_of_t<ImportFunc(DataType)>, QXmppTask<variant<Success, QXmppError>>>);
    // static_assert(is_same_v<result_of_t<ExportFunc()>, QXmppTask<variant<DataType, QXmppError>>>);
    // static_assert(is_same_v<first_argument_t<ImportFunc>, DataType>);

    auto importInternal = [importFunc = move(importFunc)](any data) -> QXmppTask<variant<Success, QXmppError>> {
        Q_ASSERT(type_index(data.type()) == type_index(typeid(DataType)));

        return importFunc(std::any_cast<DataType>(std::move(data)));
    };

    using AnyResult = variant<any, QXmppError>;
    auto exportInternal = [this, exportFunc = std::move(exportFunc)]() -> QXmppTask<AnyResult> {
        return chain<AnyResult>(exportFunc(), this, [](auto &&result) {
            return visit(overloaded {
                             [](DataType data) -> AnyResult { return any(move(data)); },
                             [](QXmppError err) -> AnyResult { return err; } },
                         move(result));
        });
    };

    registerMigrationDataInternal(type_index(typeid(DataType)), move(importInternal), move(exportInternal));
}

template<typename DataType>
void QXmppAccountMigrationManager::unregisterExtension()
{
    unregisterMigrationDataInternal(std::type_index(typeid(DataType)));
}

#endif
