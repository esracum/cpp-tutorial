#include "objectdetect.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Object_Detect_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    qApp->setStyleSheet(
        "QMainWindow { background-color: #2b2b2b; }"
        "QLabel { color: #ffffff; font-weight: bold; font-family: 'Segoe UI'; }"
        "QPushButton { "
        "   background-color: #3a3a3a; color: #00ff00; "
        "   border: 1px solid #00ff00; border-radius: 5px; padding: 5px; }"
        "QPushButton:hover { background-color: #00ff00; color: #000000; }"
    );
    ObjectDetect w;
    w.show();
    return a.exec();
}
