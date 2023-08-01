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
#include <QNetworkCookieJar>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mSettingMenu(new QMenu(tr("&Setting"),this))
    , mTimer(new QTimer(this))
    , mTrayMenu(new QMenu(this))
    , mTrayIcon(new QSystemTrayIcon(this))
    , mViewWindow(new QAction("View window", this))
    , mQuitAction(new QAction("Exit", this))
    , mNotification(new PopUp(this))
    , mManager(new QNetworkAccessManager)
{
    ui->setupUi(this);

    /// Setting Menu
    QAction* loadSetting = new QAction(tr("&Load"),this);
    QAction* saveSetting = new QAction(tr("&Save"),this);
    QAction* clearSetting = new QAction(tr("&Clear"),this);

    mSettingMenu->addAction(loadSetting);
    mSettingMenu->addAction(saveSetting);
    mSettingMenu->addAction(clearSetting);

    menuBar()->addMenu(mSettingMenu);

    connect(loadSetting, SIGNAL(triggered()), this, SLOT(loadSetting()));
    connect(saveSetting, SIGNAL(triggered()), this, SLOT(loadSetting()));
    QObject::connect(clearSetting, &QAction::triggered, this, [this](){
        ui->lE_hhtoken->clear();
        ui->lE_hhuid->clear();
        ui->lE_idResume->clear();
        ui->lE_xsrf->clear();
    });

    /// Log Menu
    mLogMenu = new QMenu(tr("&Log"),this);

    QAction* clearLog = new QAction(tr("&Clear"),this);

    mLogMenu->addAction(clearLog);

    menuBar()->addMenu(mLogMenu);

    QObject::connect(clearLog, &QAction::triggered, this, [this](){
        ui->tE_logInfo->clear();
    });

    /// 14400000 milliseconds equals 4 hours
    mTimer->setInterval(14400000);

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

    connect(mManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseFromServer(QNetworkReply*)));

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

        ui->pB_startAutoUpdate->setEnabled(true);
        ui->pB_stopAutoUpdate->setEnabled(false);
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
    sendRequest("https://hh.ru/applicant/resumes/touch");
}

void MainWindow::responseFromServer(QNetworkReply *reply){

    QString httpStatusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toString();
    QString httpReasonPhrase = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();

    QString color;

    if(httpStatusCode >= "200" && httpStatusCode <= "299"){
        color = "green";
        mNotification->setPopupText("Resume is updated!");
    }else if(httpStatusCode >= "300" && httpStatusCode <= "399"){
        color = "orange";
        mNotification->setPopupText("Resume is updated!");
    }else{
        mNotification->setPopupText("Resume not updated!\nCode: " + httpStatusCode);
        color = "red";
    }

    ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">>  ============================================= <br><br>[" + getCurrentDateTime() + "]: </strong> Update resume<br>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: " + color + ";\">Answer from server: </strong>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: " + color + ";\">> </strong> <span style=\"color: " + color + ";\">Code: " + httpStatusCode + "</span>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: " + color + ";\">> </strong> <span style=\"color: " + color + ";\">Status: " + httpReasonPhrase + "</span><br><br>");

    if(httpStatusCode == "200"){
        QDateTime nextUpdate = QDateTime::currentDateTime().addMSecs(14400000);
        ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">>The next update will be available:</strong> " + nextUpdate.toString(Qt::DefaultLocaleShortDate) + "<br><br>");
    }

    playSound("resources/sounds/notify.mp3");
    mNotification->show();
    MoveCursorToEnd();
}

void MainWindow::RequestNextUpdateFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QString message;

    if (reply->error() == QNetworkReply::NoError) {
        QString htmlContent = reply->readAll();

        QRegularExpression re("<div\\s+class=\"resume-sidebar-text\"\\s+data-qa=\"resume-update-message\">(.*?)</div>");
        QRegularExpressionMatch match = re.match(htmlContent);
        if (match.hasMatch()) {
            message = match.captured(1);
            message = message.left(message.indexOf("«")).trimmed();

        }else{
            message = "The update is available";
        }
    }else {
        message = "Error when receiving HTML content";
    }

    ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">>  ============================================= <br><br>[" + getCurrentDateTime() + "]: </strong>Checking for updates<br><br>");
    ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">>Information: </strong>" + message + " <br><br>");

    reply->deleteLater();

    playSound("resources/sounds/notify.mp3");
    mNotification->setPopupText("Information: " + message);
    mNotification->show();
    MoveCursorToEnd();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason){
    switch (reason){
    case QSystemTrayIcon::Trigger:
        !this->isVisible() ? this->show() : this->hide();
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

void MainWindow::sendRequest(const QString& url){
    setSettings();
    QNetworkRequest request = getRequest(url);
    QByteArray postData = "resume=" + Settings::instance().getIDResume().toUtf8() + "&undirectable=" + "true";
    mManager->post(request, postData);
}

QString MainWindow::getCurrentDateTime(){
    return QDateTime::currentDateTime().toString(Qt::DefaultLocaleShortDate);
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

void MainWindow::playSound(const QString &path){
    QMediaPlayer *player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(path));
    player->setVolume(70);
    player->play();
}

QNetworkRequest MainWindow::getRequest(const QString &url)
{
    QNetworkRequest request((QUrl(url)));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    request.setRawHeader("X-Xsrftoken", Settings::instance().getXsrf().toUtf8());

    request.setRawHeader("Cookie", "hhtoken=" + Settings::instance().getHhtoken().toUtf8() + "; " +
                         "hhuid=" + Settings::instance().getHhuid().toUtf8() + "; " +
                         "_xsrf=" + Settings::instance().getXsrf().toUtf8() + "; "
                         );

    return request;
}

void MainWindow::on_pB_updateResume_clicked(){
    fieldsIsEmpty() ? showMessageBox("Fill in all the fields!\t\t\n") : sendRequest("https://hh.ru/applicant/resumes/touch");
}

void MainWindow::on_pB_startAutoUpdate_clicked(){
    if(fieldsIsEmpty()){
        showMessageBox("Fill in all the fields!\t\t\n");
    }else{
        ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">>  ============================================= <br><br>[" + getCurrentDateTime() + "]: </strong>Auto Update started<br><br>");

        sendRequest("https://hh.ru/applicant/resumes/touch");

        mTimer->start();

        ui->pB_startAutoUpdate->setEnabled(false);
        ui->pB_stopAutoUpdate->setEnabled(true);
    }
}


void MainWindow::on_pB_stopAutoUpdate_clicked(){
    ui->tE_logInfo->insertHtml("<strong style=\"font-size: 12px; color: #0d6efd;\">>  ============================================= <br><br>[" + getCurrentDateTime() + "]: </strong>Auto Update stoped<br><br>");

    mTimer->stop();

    ui->pB_startAutoUpdate->setEnabled(true);
    ui->pB_stopAutoUpdate->setEnabled(false);

    MoveCursorToEnd();
}


void MainWindow::on_pB_checkUpdate_clicked(){
    if(fieldsIsEmpty()){
        showMessageBox("Fill in all the fields!\t\t\n");
    }else{
        setSettings();

        QNetworkRequest request = getRequest("https://tver.hh.ru/resume/" + Settings::instance().getIDResume().toUtf8());
        QNetworkAccessManager* managerNextUpdate = new QNetworkAccessManager;
        QNetworkReply *reply = managerNextUpdate->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(RequestNextUpdateFinished()));
    }
}

