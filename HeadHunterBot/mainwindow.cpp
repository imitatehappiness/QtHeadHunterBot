#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settings.h"

#include <QFileDialog>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStyle>
#include <QCloseEvent>
#include <QIcon>
#include <QDateTime>
#include <QTextCharFormat>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mSettingMenu(new QMenu(tr("&Setting"),this))
    , mLoadSetting(new QAction(tr("&Load"),this))
    , mSaveSetting(new QAction(tr("&Save"),this))
    , mTimer(new QTimer)
    , trayMenu(new QMenu(this))
    , trayIcon(new QSystemTrayIcon(this))
    , viewWindow(new QAction("View window", this))
    , quitAction(new QAction("Exit", this))
    , mManager(new QNetworkAccessManager)
{
    ui->setupUi(this);

    mSettingMenu->addAction(mLoadSetting);
    mSettingMenu->addAction(mSaveSetting);

    menuBar()->addMenu(mSettingMenu);

    connect(mLoadSetting, SIGNAL(triggered()), this, SLOT(loadSetting()));
    connect(mSaveSetting, SIGNAL(triggered()), this, SLOT(saveSetting()));

    mTimer->setInterval(14400000);// 4 hour

    connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseFromServer(QNetworkReply*)));
    connect(mTimer, SIGNAL(timeout()), this, SLOT(requestOnTimer()));

    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    trayMenu->addAction(viewWindow);
    trayMenu->addAction(quitAction);

    trayIcon->setIcon((QIcon(":/resources/icons/hh-logo.png")));
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(this->isVisible()){
        event->ignore();
        this->hide();

        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
        trayIcon->showMessage("Tray Program", "", icon, 2000);
    }
}

void MainWindow::show(){
    if(!isHidden()) {
        this->showNormal();
        this->activateWindow();
    }else{
        QWidget::show();
    }
}

void MainWindow::loadSetting(){
    QString fileName = QFileDialog::getOpenFileName(this, "", "", tr("Settings (*.json);;All Files (*)"));
    if("" != fileName){
        Settings::instance().loadSettings(fileName);

        ui->lE_idResume->setText(Settings::instance().getIDResume());
        ui->lE_hhtoken->setText(Settings::instance().getHhtoken());
        ui->lE_hhuid->setText(Settings::instance().getHhuid());
        ui->lE_xsrf->setText(Settings::instance().getXsrf());
    }
}

void MainWindow::saveSetting(){
    QString fileName = QFileDialog::getSaveFileName(this, "", "", tr("Settings (*.json);;All Files (*)"));
    if("" != fileName){
        setSettings();

        Settings::instance().saveSettings(fileName);
    }
}

void MainWindow::requestOnTimer(){
    sendRequest();
}

void MainWindow::responseFromServer(QNetworkReply *reply){
    QString httpStatusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toString();
    QString httpReasonPhrase = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();

    ui->tE_logInfo->append(getCurrentDateTime() + "\nAnswer from server:");
    ui->tE_logInfo->append("Code: " + httpStatusCode);
    ui->tE_logInfo->append("Status: " + httpReasonPhrase);
    ui->tE_logInfo->append("");
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason){
    case QSystemTrayIcon::Trigger:
        if(false == this->isVisible()){
            this->show();
        }
        else{
            this->hide();
        }
        break;
    default:
        break;
    }
}

void MainWindow::setSettings(){
    Settings::instance().setIDResume(ui->lE_idResume->text());
    Settings::instance().setHhtoken(ui->lE_hhtoken->text());
    Settings::instance().setHhuid(ui->lE_hhuid->text());
    Settings::instance().setXsrf(ui->lE_xsrf->text());
}

void MainWindow::sendRequest(){
    setSettings();

    QNetworkRequest request(QUrl("https://hh.ru/applicant/resumes/touch"));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");

    request.setRawHeader("X-Xsrftoken", Settings::instance().getXsrf().toUtf8());

    request.setRawHeader("Cookie", "hhtoken=" + Settings::instance().getHhtoken().toUtf8() + "; " +
                         "hhuid=" + Settings::instance().getHhuid().toUtf8() + "; " +
                         "_xsrf=" + Settings::instance().getXsrf().toUtf8() + "; "
                         );

    QByteArray postData = "resume=" + Settings::instance().getIDResume().toUtf8() +
            "&undirectable=" + "true";

    mManager->post(request, postData);
}

QString MainWindow::getCurrentDateTime(){
    QDateTime currentDateTime = QDateTime::currentDateTime();
    return currentDateTime.toString(Qt::DefaultLocaleLongDate);
}

bool MainWindow::fieldsIsEmpty(){
    return ui->lE_idResume->text() == "" || ui->lE_hhtoken->text() == "" || ui->lE_hhuid->text() == "" || ui->lE_xsrf->text() == "";
}


void MainWindow::on_pB_updateResume_clicked(){
    if(fieldsIsEmpty()){
        QMessageBox msgBox;
        msgBox.setText("Fill in all the fields!\t\t\n");
        msgBox.setStandardButtons( QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowIcon(QIcon(":/resources/icons/hh-logo.png"));
        msgBox.setWindowTitle("hh-bot: Information");
        msgBox.exec();
    }else{
        sendRequest();
    }
}


void MainWindow::on_pB_startAutoUpdate_clicked(){
    ui->tE_logInfo->append(getCurrentDateTime() + "\nTimer started");
    ui->tE_logInfo->append("");

    sendRequest();

    mTimer->start();

    ui->pB_startAutoUpdate->setEnabled(false);
    ui->pB_stopAutoUpdate->setEnabled(true);
}


void MainWindow::on_pB_stopAutoUpdate_clicked(){
    ui->tE_logInfo->append(getCurrentDateTime() + "\nTimer stoped");
    ui->tE_logInfo->append("");

    mTimer->stop();

    ui->pB_startAutoUpdate->setEnabled(true);
    ui->pB_stopAutoUpdate->setEnabled(false);
}

