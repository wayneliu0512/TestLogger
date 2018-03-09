#ifndef TESTITEMREPORTER_H
#define TESTITEMREPORTER_H

#include <QObject>

class QSerialPort;

class TestItemReporter : public QObject
{
    Q_OBJECT

    enum FileResult { Pass, Fail, OpenError};
public:
    explicit TestItemReporter(QObject *parent = nullptr, const QString com = "0", const QString name = "");

    void startTest();
    void finishTest(const QString &fileName);

private:
    bool initSerial();
    bool checkTestName();
    FileResult getTestResult(const QString &fileName) const;

    QString comport;
    QString testname;
    QSerialPort *serial;
};

#endif // TESTITEMREPORTER_H
