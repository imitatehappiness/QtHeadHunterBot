#include "mainwindow.h"

#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QFile>
#include <QMessageBox>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    /// Set Style
    a.setStyle(new QProxyStyle(QStyleFactory::create("Fusion")));
    QString stylePath = ":/resources/styles/appstyles.qss";
    QFile styleFile(stylePath);
    styleFile.open(QFile::ReadOnly);
    QString styleQSS = styleFile.readAll();

    if (styleQSS.size() == 0){
        QMessageBox mBox;
        mBox.setWindowIcon(QIcon(":/resources/icons/hh_manager-logo.png"));
        mBox.setIcon(QMessageBox::Warning);
        mBox.setText("Ошибка чтения appstyles.qss!\nПрограмма может работать некорректно!!");
        mBox.setButtonText(QMessageBox::Ok, "Ok");
        mBox.exec();
    }

    a.setStyleSheet(styleQSS);

    /// Set Russian language
    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(), "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
        a.installTranslator(&qtTranslator);
    }

    QTranslator qtBaseTranslator;
    if (qtBaseTranslator.load("qtbase_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))){
        a.installTranslator(&qtBaseTranslator);
    }


    MainWindow w;
    w.show();
    return a.exec();
}

