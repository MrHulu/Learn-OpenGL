#include <QtTest>
#include <QSignalSpy>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDateTime>

#include "coap/Log.h"

using namespace CoapPlusPlus;
class tst_Log : public QObject {
    Q_OBJECT
public:
    tst_Log() {
        m_path = QDir::currentPath() + "/tst_log.txt";

        m_function = [](CoapPlusPlus::LOG_LEVEL level, const char* message){
            static auto path = QDir::currentPath() + "/tst_log.txt";
            QFile file(path);
            if(file.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&file);
                auto levelstr =QString("[%1] ").arg(Log::LevelToString(level).c_str());
                out << levelstr << message;
                out.flush();
                file.close();
            }
        };
    }
    ~tst_Log() {  
        QFile file(m_path);
        if(file.exists()) {
            file.remove();
        }
    }
private:
    QString m_path;
    std::function<void(CoapPlusPlus::LOG_LEVEL, const char*)> m_function;
    QString read();

private slots:
    void test_LevelToString();

    void test_set_get_level();

    void test_set_log_handler();
};

QString tst_Log::read()
{
    QFile file(m_path);
    QString content;
    if(file.exists()) {
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            content = file.readAll();
            file.close();
        }
    }
    return content;
}

QTEST_MAIN(tst_Log)

#include "tst_Log.moc"

void tst_Log::test_LevelToString()
{
    LOG_LEVEL level = LOG_LEVEL::EMERG;
    QCOMPARE(Log::LevelToString(level).c_str(), "EMERG");

    level = LOG_LEVEL::INFO;
    QCOMPARE(Log::LevelToString(level).c_str(), "INFO");
}

void tst_Log::test_set_get_level()
{
    QCOMPARE(Log::GetLevel(), LOG_LEVEL::WARN);
    Log::Logging(LOG_LEVEL::EMERG, "EMERG: test_logging\n");
    Log::Logging(LOG_LEVEL::INFO, "INFO: test_logging\n");

    Log::SetLevel(LOG_LEVEL::INFO);
    QCOMPARE(Log::GetLevel(), LOG_LEVEL::INFO);
    Log::Logging(LOG_LEVEL::DEBUG, "DEBUG: test_logging\n");
    Log::Logging(LOG_LEVEL::NOTICE, "NOTICE: test_logging\n");
}

void tst_Log::test_set_log_handler()
{
    Log::SetLogHandler(m_function);
    Log::Logging(LOG_LEVEL::EMERG, "hello world\n");

    QTest::qWait(100);
    QCOMPARE(read(), QString("[%1] hello world\n").arg(Log::LevelToString(LOG_LEVEL::EMERG).c_str()));

    Log::Logging(LOG_LEVEL::DEBUG, "Ok\n");
    QCOMPARE(read(), QString("[%1] hello world\n").arg(Log::LevelToString(LOG_LEVEL::EMERG).c_str()));
}
