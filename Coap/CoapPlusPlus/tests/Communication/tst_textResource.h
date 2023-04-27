#include "coap/server/TextResource.h"

#include <QObject> 
#include <QString>
#include <QDebug>

namespace Coap {

class SimpleServer;
class Tst_TextResource : public QObject, public TextResource
{
    Q_OBJECT
public:
    Tst_TextResource(const std::string& uriPath, bool observable = false, QObject* parent = nullptr)
        : QObject(parent)
        , TextResource(uriPath, observable)
    {
    }

    void onUpdateTextFromClient(const std::string& text) noexcept override
    {
        auto t = QString::fromStdString(text);
        if(t == m_text)
            return;
        m_text = t;
        Q_EMIT textChanged(m_text);
    }

    std::string text() const noexcept override
    {
        return m_text.toStdString();
    }

    void setText(const std::string& text) noexcept
    {
        auto t = QString::fromStdString(text);
        if(t == m_text)
            return;
        m_text = t;
        notifyObservers();
    }

    Q_SIGNAL void textChanged(const QString& text);

private:
    QString m_text = "Tst_TextResource";   
};

}; // namespace Coap