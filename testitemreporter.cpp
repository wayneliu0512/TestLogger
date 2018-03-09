#include "testitemreporter.h"
#include <QSerialPort>
#include <QtCore>

TestItemReporter::TestItemReporter(QObject *parent, const QString com, const QString name) : QObject(parent), comport(com),
    testname(name)
{}

void TestItemReporter::startTest()
{
    if (!initSerial() || !checkTestName())
        return;

    QString sendStr = "@" + testname + ";";
    serial->write(sendStr.toLocal8Bit());
    serial->waitForBytesWritten(100);
    qDebug() << "Complete";
}

void TestItemReporter::finishTest(const QString &fileName)
{
    if (!initSerial() || !checkTestName())
        return;

    QString sendStr;
    switch (getTestResult(fileName)) {
    case Pass:
        sendStr = "@" + testname + "/P;";
        break;
    case Fail:
        sendStr = "@" + testname + "/F;";
        break;
    case OpenError:
        return;
        Q_UNREACHABLE();
    }
    serial->write(sendStr.toLocal8Bit());
    serial->waitForBytesWritten(100);
    qDebug() << "Complete";
}

bool TestItemReporter::initSerial()
{
    serial = new QSerialPort(this);
    serial->setPortName("COM" + comport);
    serial->setBaudRate(QSerialPort::Baud57600);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);

    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Serialport open success.";
        return true;
    } else {
        qDebug() << "Serialport open error.";
        return false;
    }
}

bool TestItemReporter::checkTestName()
{
    if (testname.length() > 13) {
        qDebug() << "testname characters is too much.";
        return false;
    }
    return true;
}

TestItemReporter::FileResult TestItemReporter::getTestResult(const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "file open error.";
        return OpenError;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains("pass", Qt::CaseInsensitive))
            return Pass;
    }
    return Fail;
}

