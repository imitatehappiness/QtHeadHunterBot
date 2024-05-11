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

const uint FOUR_HOUR = 14460;
const QString TOUCH_URL = "https://hh.ru/applicant/resumes/touch";
const QString LOGO_PATH = ":/resources/icons/hh-logo.png";
const QString TITLE = "hh-bot";
const QString STRONG_STYLE = "<strong style=\"font-size: 12px; color: #0d6efd;\">";
const QString LINE = "=============================================";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mSettingMenu(new QMenu(tr("&Настройки"),this))
    , mLogMenu(new QMenu(tr("&История"),this))
    , mTrayMenu(new QMenu(this))
    , mTimer(new QTimer(this))
    , mNotification(new PopUp(this))
{
    ui->setupUi(this);

    setWindowTitle(TITLE);

    initSettingMenu();
    initLogMenu();
    initTrayMenu();

    connect(mTimer, SIGNAL(timeout()), this, SLOT(requestOnTimer()));
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(this->isVisible()){
        event->ignore();
        this->hide();
    }
}

void MainWindow::show(){
    if(!isHidden()){
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
        ui->lE_url->setText(Settings::instance().getUrl());

        ui->pB_turn->setProperty("turn", false);
        ui->pB_turn->setText("Запустить");
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

    if(httpStatusCode >= "200" && httpStatusCode <= "299"){
        color = "green";
        mNotification->setPopupText("Резюме обновлено!");
    }else if(httpStatusCode >= "300" && httpStatusCode <= "399"){
        color = "orange";
        mNotification->setPopupText("Резюме не обновлено!\nCode: " + httpStatusCode);
    }else{
        mNotification->setPopupText("Резюме не обновлено!\nCode: " + httpStatusCode);
        color = "red";
    }

    ui->tE_logInfo->insertHtml(STRONG_STYLE + LINE + "<br><br>[" + getCurrentDateTime() + "]: </strong> Обновление резюме.<br>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: " + color + ";\">Ответ от сервера: </strong>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: " + color + ";\"> </strong> <span style=\"color: " + color + ";\">Code: " + httpStatusCode + "</span>");
    ui->tE_logInfo->insertHtml("<br><strong style=\"font-size: 12px; color: " + color + ";\"> </strong> <span style=\"color: " + color + ";\">Status: " + httpReasonPhrase + "</span><br><br>");

    if(httpStatusCode >= "200" && httpStatusCode <= "299"){
        QDateTime mNextUpdate = QDateTime::currentDateTime().addMSecs(FOUR_HOUR * 1000);
        ui->tE_logInfo->insertHtml(STRONG_STYLE + "Следующее обновление:</strong> " + mNextUpdate.toString(Qt::DefaultLocaleShortDate) + ".<br><br>");
        mTimer->start();

    }
    if(httpStatusCode >= "300" && httpStatusCode <= "399"){
        ui->tE_logInfo->insertHtml(STRONG_STYLE + "Информация:</strong> Введите капчу на сайте.<br><br>");
    }

    reply->deleteLater();
    MoveCursorToEnd();
}

void MainWindow::RequestmNextUpdateFinished(){
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    enum status{error, update_available, update_not_available};
    status update = status::error;
    QString message = "Ошибка при получении HTML-содержимого.";

    if (reply->error() == QNetworkReply::NoError) {
        QString htmlContent = reply->readAll();

        QRegularExpression disabled("<button\\s+class=\"bloko-button bloko-button_kind-primary bloko-button_stretched\" type=\"button(.*?)=\"resume-update-button\"><span>Обновить дату </span></button>");
        QRegularExpressionMatch match = disabled.match(htmlContent);
        if (match.hasMatch()) {
            if(!match.captured(1).contains("disabled")){
                update = status::update_available;
                message = "Обновление доступно.";
                mTimer->setInterval(FOUR_HOUR * 1000);
            }else{
                QRegularExpression nextUpdate("<div\\s+class=\"resume-sidebar-text\"\\s+data-qa=\"resume-update-message\">(.*?)</div>");
                match = nextUpdate.match(htmlContent);
                if(match.hasMatch()){
                    message = match.captured(1);
                    message = message.left(message.indexOf("«")).trimmed();
                    update = status::update_not_available;
                    mTimer->setInterval(FOUR_HOUR / 8 * 1000);
                    mTimer->start();
                }
            }
        }
    }

    ui->tE_logInfo->insertHtml(STRONG_STYLE + LINE + "<br><br>[" + getCurrentDateTime() + "]: </strong>Проверка обновления.<br><br>");
    ui->tE_logInfo->insertHtml(STRONG_STYLE + "Информация: </strong>" + message + " <br><br>");

    if(update == status::update_not_available){
        QDateTime mNextUpdate = QDateTime::currentDateTime().addMSecs(FOUR_HOUR / 8 * 1000);
        ui->tE_logInfo->insertHtml(STRONG_STYLE + "Следующая проверка обновления:</strong> " + mNextUpdate.toString(Qt::DefaultLocaleShortDate) + ".<br><br>");
    }

    if(reply->error() == QNetworkReply::NoError && update == status::update_available){
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);
        QNetworkRequest request = getRequest(TOUCH_URL);
        QByteArray postData = "resume=" + Settings::instance().getIDResume().toUtf8() + "&undirectable=" + "true";\
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseFromServer(QNetworkReply*)));
        manager->post(request, postData);
    }

    if(update == status::error){
        showMessageBox(message + "\nПроверьте корректность введённых параметров.\t\n");
        ui->pB_turn->setProperty("turn", false);
        ui->pB_turn->setText("Запустить");
    }else{
        mNotification->setPopupText("Информация:" + message);
        mNotification->show();
    }

    reply->deleteLater();
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
    Settings::instance().setUrl(ui->lE_url->text());
}

void MainWindow::sendRequest(){
    setSettings();
    {
        QString url = Settings::instance().getUrl().toUtf8();
        if(url[url.size() - 1] != "/"){
            url += "/";
        }
        QNetworkRequest request = getRequest(url + "resume/" + Settings::instance().getIDResume().toUtf8());
        QNetworkAccessManager* managermNextUpdate = new QNetworkAccessManager(this);
        QNetworkReply *reply = managermNextUpdate->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(RequestmNextUpdateFinished()));
    }
}

QString MainWindow::getCurrentDateTime(){
    return QDateTime::currentDateTime().toString(Qt::DefaultLocaleShortDate);
}

void MainWindow::showMessageBox(const QString &mesasge){
    QMessageBox msgBox;
    msgBox.setText(mesasge);
    msgBox.setStandardButtons( QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(QIcon(LOGO_PATH));
    msgBox.setWindowTitle(TITLE);
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

QNetworkRequest MainWindow::getRequest(const QString &url){
    QNetworkRequest request((QUrl(url)));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);

    request.setRawHeader("X-Xsrftoken", Settings::instance().getXsrf().toUtf8());

    request.setRawHeader("Cookie", "hhtoken=" + Settings::instance().getHhtoken().toUtf8() + "; " +
                         "hhuid=" + Settings::instance().getHhuid().toUtf8() + "; " +
                         "_xsrf=" + Settings::instance().getXsrf().toUtf8() + "; "
                         );

    return request;
}

bool MainWindow::checkCorrectlyFields(){
    if(ui->lE_idResume->text() == "" || ui->lE_hhtoken->text() == "" || ui->lE_hhuid->text() == "" || ui->lE_xsrf->text() == "" || ui->lE_url->text() == ""){
        return false;
    }

    if(!hasMatch("\\b[0-9a-fA-F]{30,}\\b", ui->lE_idResume->text())){
        return false;
    }

    if(!hasMatch("\\b[0-9a-fA-F]{30,}\\b", ui->lE_xsrf->text())){
        return false;
    }

    if(!hasMatch("\\bhttps?:\\/\\/\\S+\\b", ui->lE_url->text())){
        return false;
    }

    return true;
}

bool MainWindow::hasMatch(const QString &re, const QString &text){
    QRegularExpression regex(re);
    QRegularExpressionMatch match = regex.match(text);

    return match.hasMatch() ? true : false;
}

void MainWindow::initSettingMenu(){
    QAction* loadSetting = new QAction(tr("&Загрузить"),this);
    QAction* saveSetting = new QAction(tr("&Сохранить"),this);
    QAction* clearSetting = new QAction(tr("&Очистить"),this);

    mSettingMenu->addAction(loadSetting);
    mSettingMenu->addAction(saveSetting);
    mSettingMenu->addAction(clearSetting);

    menuBar()->addMenu(mSettingMenu);

    connect(loadSetting, SIGNAL(triggered()), this, SLOT(loadSetting()));
    connect(saveSetting, SIGNAL(triggered()), this, SLOT(saveSetting()));
    QObject::connect(clearSetting, &QAction::triggered, this, [this](){
        ui->lE_hhtoken->clear();
        ui->lE_hhuid->clear();
        ui->lE_idResume->clear();
        ui->lE_xsrf->clear();
        ui->lE_url->clear();
    });
}

void MainWindow::initTrayMenu(){
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    QAction* viewWindow = new QAction("Открыть " + TITLE, this);
    QAction* quitAction = new QAction("Закрыть " + TITLE, this);

    mTrayMenu->addAction(viewWindow);
    mTrayMenu->addAction(quitAction);

    trayIcon->setIcon((QIcon(LOGO_PATH)));
    trayIcon->setContextMenu(mTrayMenu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    QObject::connect(quitAction, &QAction::triggered, this, [this](){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, TITLE, "Вы действительно хотите закрыть приложение?\n\n", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            show();
        }else if (reply == QMessageBox::Yes) {
            QApplication::quit();
        }
    });
}

void MainWindow::initLogMenu(){
    QAction* clearLog = new QAction(tr("&Очистить"),this);

    mLogMenu->addAction(clearLog);

    menuBar()->addMenu(mLogMenu);

    QObject::connect(clearLog, &QAction::triggered, this, [this](){
        ui->tE_logInfo->clear();
    });
}

void MainWindow::on_pB_turn_clicked(){
    if (ui->pB_turn->property("turn") == true){

        ui->tE_logInfo->insertHtml(STRONG_STYLE + LINE + "<br><br>[" + getCurrentDateTime() + "]: </strong>Авто-обновление выключено.<br><br>");

        mTimer->stop();

        ui->pB_turn->setProperty("turn", false);
        ui->pB_turn->setText("Запустить");

        MoveCursorToEnd();

    }else{

        if(!checkCorrectlyFields()){
            showMessageBox("Заполните поля корректными значениями!\t\n");
        }else{
            ui->tE_logInfo->insertHtml(STRONG_STYLE + LINE + "<br><br>[" + getCurrentDateTime() + "]: </strong>Авто-обновление включено.<br><br>");
            mTimer->setInterval(FOUR_HOUR * 1000);

            sendRequest();

            ui->pB_turn->setText("Остановить");
            ui->pB_turn->setProperty("turn", true);
        }
    }
}
