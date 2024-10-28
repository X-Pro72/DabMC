#include <QObject>
#include <functional>

class LambdaHelper : public QObject {
    Q_OBJECT
   public:
    LambdaHelper(std::function<void()>&& fun, QObject* parent) : QObject(parent), m_fun(std::move(fun)) {}
    static QMetaObject::Connection connect(QObject* sender, const char* signal, std::function<void()>&& fun)
    {
        if (!sender)
            return {};
        return QObject::connect(sender, signal, new LambdaHelper(std::move(fun), sender), SLOT(call()));
    }

    Q_SLOT void call() { m_fun(); }

   private:
    std::function<void()> m_fun;
};


class LambdaHelperString : public QObject {
    Q_OBJECT
   public:
    LambdaHelperString(std::function<void(const QString&)>&& fun, QObject* parent) : QObject(parent), m_fun(std::move(fun)) {}
    static QMetaObject::Connection connect(QObject* sender, const char* signal, std::function<void(const QString&)>&& fun)
    {
        if (!sender)
            return {};
        return QObject::connect(sender, signal, new LambdaHelperString(std::move(fun), sender), SLOT(call()));
    }

    Q_SLOT void call(const QString& s) { m_fun(s); }

   private:
    std::function<void(const QString&)> m_fun;
};
