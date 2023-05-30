#include "coap/server/TextResource.h"

#include <QObject> 
#include <QString>
#include <QDebug>

namespace CoapPlusPlus {

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

    void getRequestCallback(coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response) override
    {
        qDebug() << "Tst_TextResource::getRequestCallback" << session << coap_session_get_type(session) << coap_session_get_state(session);
        if (auto a = coap_session_get_addr_remote(session)) {
            qDebug() << "Tst_TextResource::coap_session_get_addr_remote" << coap_address_get_port(a);
        }if (auto a = coap_session_get_addr_mcast(session)) {
            qDebug() << "Tst_TextResource::coap_session_get_addr_mcast" << coap_address_get_port(a);
        }if (auto a = 	coap_session_get_addr_local(session)) {
            qDebug() << "Tst_TextResource::coap_session_get_addr_local " << coap_address_get_port(a);
        }

        auto uri_path = coap_resource_get_uri_path(getResource());
        if (!uri_path) {
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_NOT_FOUND);
            return;
        }
        coap_pdu_set_type(response, COAP_MESSAGE_CON);
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
        
        uint8_t buf[4];
        auto formatOptionLen = coap_encode_var_safe(buf, sizeof(buf), COAP_MEDIATYPE_TEXT_PLAIN);
        coap_add_option(response, COAP_OPTION_CONTENT_FORMAT, formatOptionLen, buf);

        // 使用coap_add_data_large_response()函数，可以自动实现块传输
        auto temp = text();
        coap_add_data_large_response(getResource(), session, request, response, query, 
                                        COAP_MEDIATYPE_TEXT_PLAIN, -1, 0, temp.length(),
                                        reinterpret_cast<const uint8_t*>(temp.c_str()), 
                                        nullptr, nullptr);
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

};// namespace CoapPlusPlus 