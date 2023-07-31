#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settings.h"
#include "PopUp.h"

#include <QFileDialog>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStyle>
#include <QCloseEvent>
#include <QIcon>
#include <QDateTime>
#include <QMessageBox>
#include <QTextCursor>
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mSettingMenu(new QMenu(tr("&Setting"),this))
    , mLoadSetting(new QAction(tr("&Load"),this))
    , mSaveSetting(new QAction(tr("&Save"),this))
    , mTimer(new QTimer)
    , mTrayMenu(new QMenu(this))
    , mTrayIcon(new QSystemTrayIcon(this))
    , mViewWindow(new QAction("View window", this))
    , mQuitAction(new QAction("Exit", this))
    , mManager(new QNetworkAccessManager)
    , mNotification(new PopUp(this))
{
    ui->setupUi(this);

    mSettingMenu->addAction(mLoadSetting);
    mSettingMenu->addAction(mSaveSetting);

    menuBar()->addMenu(mSettingMenu);

    connect(mLoadSetting, SIGNAL(triggered()), this, SLOT(loadSetting()));
    connect(mSaveSetting, SIGNAL(triggered()), this, SLOT(saveSetting()));

    /// 14400000 milliseconds equals 4 hours
    mTimer->setInterval(14400000);

    connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseFromServer(QNetworkReply*)));
    connect(mTimer, SIGNAL(timeout()), this, SLOT(requestOnTimer()));

    connect(mViewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(mQuitAction, SIGNAL(triggered()), this, SLOT(close()));

    mTrayMenu->addAction(mViewWindow);
    mTrayMenu->addAction(mQuitAction);

    mTrayIcon->setIcon((QIcon(":/resources/icons/hh-logo.png")));
    mTrayIcon->setContextMenu(mTrayMenu);
    mTrayIcon->show();

    connect(mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    ui->lE_idResume->setToolTip("ID resume: The unique identifier of your resume on HeadHunter.");
    ui->lE_hhtoken->setToolTip("hhtoken: HeadHunter API token required for authentication.");
    ui->lE_hhuid->setToolTip("hhuid: HeadHunter user ID associated with your account.");
    ui->lE_xsrf->setToolTip("xsrf: Cross-Site Request Forgery token for secure communication with the HeadHunter website.");

}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(this->isVisible()){
        event->ignore();
        this->hide();

        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
        mTrayIcon->showMessage("Tray Program", "", icon, 2000);
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

    QString color;

    if(httpStatusCode == 200){
        color = "green";
        mNotification->setPopupText("Resume is updated!");
    }else{
        mNotification->setPopupText("Resume not updated!\nCode: " + httpStatusCode);
        color = "red";
    }

    ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">[" + getCurrentDateTime() + "]</strong>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: #0d6efd;\">Answer from server: </strong>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: #0d6efd;\">> </strong> <span style=\"color: " + color + ";\">Code: " + httpStatusCode + "</span>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: #0d6efd;\">> </strong> <span style=\"color: " + color + ";\">Status: " + httpReasonPhrase + "</span><br><br>");

    setDefaultStyleTextBrowser();

    if(httpStatusCode != 200){
        ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">Possible reasons: <br>");
        ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">> </strong> 1. Parameters are set incorrectly<br>");
        ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">> </strong> 2. The resume was updated less than 4 hours ago<br><br>");
    }

    playSound("resources/sounds/notify.mp3");
    mNotification->show();
    MoveCursorToEnd();
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
    return currentDateTime.toString(Qt::DefaultLocaleShortDate);
}

bool MainWindow::fieldsIsEmpty(){
    return ui->lE_idResume->text() == "" || ui->lE_hhtoken->text() == "" || ui->lE_hhuid->text() == "" || ui->lE_xsrf->text() == "";
}

void MainWindow::showMessageBox(const QString &mesasge){
    QMessageBox msgBox;
    msgBox.setText(mesasge);
    msgBox.setStandardButtons( QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(QIcon(":/resources/icons/hh-logo.png"));
    msgBox.setWindowTitle("hh-bot: Information");
    msgBox.exec();
}

void MainWindow::MoveCursorToEnd(){
    QTextCursor cursor = ui->tE_logInfo->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->tE_logInfo->setTextCursor(cursor);
}

void MainWindow::setDefaultStyleTextBrowser(){
    QTextCursor cursor = ui->tE_logInfo->textCursor();
    QTextCharFormat defaultFormat;
    cursor.setCharFormat(defaultFormat);
    ui->tE_logInfo->setTextCursor(cursor);
}

void MainWindow::playSound(const QString &path){
    QMediaPlayer *player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(path));
    player->setVolume(70);
    player->play();
}


void MainWindow::on_pB_updateResume_clicked(){
    if(fieldsIsEmpty()){
        showMessageBox("Fill in all the fields!\t\t\n");
    }else{
        sendRequest();
    }
}


void MainWindow::on_pB_startAutoUpdate_clicked(){
    if(fieldsIsEmpty()){
        showMessageBox("Fill in all the fields!\t\t\n");
    }else{
        ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">[" + getCurrentDateTime() + "]</strong>");
        ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: #0d6efd;\">> Auto Update started </strong> <br><br>");

        setDefaultStyleTextBrowser();

        sendRequest();

        mTimer->start();

        ui->pB_startAutoUpdate->setEnabled(false);
        ui->pB_stopAutoUpdate->setEnabled(true);
    }
    clearFocus();
}


void MainWindow::on_pB_stopAutoUpdate_clicked(){
    ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">[" + getCurrentDateTime() + "]</strong>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: #0d6efd;\">> Auto Update stoped </strong> <br><br>");

    setDefaultStyleTextBrowser();

    mTimer->stop();

    ui->pB_startAutoUpdate->setEnabled(true);
    ui->pB_stopAutoUpdate->setEnabled(false);

    MoveCursorToEnd();
    clearFocus();
}

