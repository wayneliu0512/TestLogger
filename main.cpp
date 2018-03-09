#include <QCoreApplication>
#include <QCommandLineParser>
#include <QtCore>
#include <testitemreporter.h>

#define VERSION "1.01.01"

enum CommandLineParseResult {
    CommandLineTestStart,
    CommandLineTestFinish,
    CommandLineError,
    CommandLineVersionRequested,
    CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString *errorMessage, QString *comPort, QString *testName,
                                        QString *fileName);
void testStart(const QString &com, const QString &testname);
void testFinal(const QString &com, const QString &testname, const QString &filename);

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("Test logger");
    QCoreApplication::setApplicationVersion(VERSION);
    QCommandLineParser parser;
    parser.setApplicationDescription("This program can communicate with robot system, updating test status\n\n"
                                     "Example: TestLogger.exe -s 3 USB20\n"
                                     "         TestLogger.exe -f C:\\Users\\wayneliu\\Desktop\\usb20.txt 3 USB20\n");

    QString errorMessage;
    QString comPort;
    QString testName;
    QString fileName;

    switch (parseCommandLine(parser, &errorMessage, &comPort, &testName, &fileName)) {
    case CommandLineTestStart:
        testStart(comPort, testName);
        return 0;
    case CommandLineTestFinish:
        testFinal(comPort, testName, fileName);
        return 0;
    case CommandLineVersionRequested:
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()), qPrintable(QCoreApplication::applicationVersion()));
        return 0;
    case CommandLineError:
        fputs(qPrintable(errorMessage), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    case CommandLineHelpRequested:
        parser.showHelp();
        Q_UNREACHABLE();
    }

    return 0;
}

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, QString *errorMessage, QString *comPort, QString *testName,
                                        QString *fileName) {
    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();
    parser.addPositionalArgument("com_port", QCoreApplication::translate("main", "Number of the com port."));
    parser.addPositionalArgument("testname", QCoreApplication::translate("main", "current test name."));

    QCommandLineOption startOption(QStringList() << "s" << "start",
                                   QCoreApplication::translate("main", "Report current test is about to start."));
    parser.addOption(startOption);

    QCommandLineOption finishOption(QStringList() << "f" << "finish",
                                    QCoreApplication::translate("main", "Report current test finished, "
                                                                        "and result will get from <filename>"),
                                    QCoreApplication::translate("main", "filename"));
    parser.addOption(finishOption);

    if (!parser.parse(QCoreApplication::arguments())) {
        *errorMessage = parser.errorText();
        return CommandLineError;
    }

    if(parser.isSet(helpOption)) {
        return CommandLineHelpRequested;
    }

    if (parser.isSet(versionOption)) {
        return CommandLineVersionRequested;
    }

    if (parser.positionalArguments().size() < 2) {
        *errorMessage = "Argument 'com_port' or 'testname' missing.";
        return CommandLineError;
    }

    if (parser.positionalArguments().size() > 2) {
        *errorMessage = "Several 'com_port' or 'testname' arguments specified.";
        return CommandLineError;
    }

    *comPort = parser.positionalArguments().first();
    QRegExp re("\\d*");  // a digit (\d), zero or more times (*)
    if (!re.exactMatch(*comPort)) {
        *errorMessage = "com_port is not numbers.";
        return CommandLineError;
    }

    *testName = parser.positionalArguments().at(1);
    if (testName->isEmpty()) {
        *errorMessage = "testname is empty.";
        qDebug() << *testName;
        return CommandLineError;
    }

    if (parser.isSet(startOption)) {
        return CommandLineTestStart;
    }

    if (parser.isSet(finishOption)) {
        *fileName = parser.value(finishOption);
        return CommandLineTestFinish;
    }

    *errorMessage = "Argument error.";
    return CommandLineError;
}

void testStart(const QString &com, const QString &testname) {
    TestItemReporter reporter(nullptr, com, testname);
    reporter.startTest();
}

void testFinal(const QString &com, const QString &testname, const QString &filename) {
    TestItemReporter reporter(nullptr, com, testname);
    reporter.finishTest(filename);
}
