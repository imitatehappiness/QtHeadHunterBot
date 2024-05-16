#include "mainwindow.h"
#include "qglobal.h"
#include "ui_mainwindow.h"

#include "settings.h"
#include "PopUp.h"

#include <QCheckBox>
#include <QAction>
#include <QWidgetAction>
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
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopServices>
#include <QIntValidator>
#include <QTableWidget>
#include <QFile>
#include <QXmlStreamWriter>

const uint FOUR_HOUR = 14460;
const QString AUTHOR = "@imitatehappiness";
const QString TOUCH_URL = "https://hh.ru/applicant/resumes/touch";
const QString LOGO_PATH = ":/resources/icons/hh-logo.png";
const QString TITLE = AUTHOR + " | hh-bot";
const QString STRONG_STYLE = "<strong style=\"color: #0d6efd;\">";


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mResumeSettingMenu(new QMenu(tr("&Продвижение"),this))
    , mParsingSettingMenu(new QMenu(tr("&Парсинг резюме"),this))
    , mTrayMenu(new QMenu(this))
    , mTimer(new QTimer(this))
    , mNotification(new PopUp(this))
{
    ui->setupUi(this);

    mVacancyManager = new hh_manager::VacancyManager();
    mVacancyManager->getAreas();

    setWindowTitle(TITLE);

    initResumeSettingMenu();
    initTrayMenu();

    connect(mTimer, SIGNAL(timeout()), this, SLOT(requestOnTimer()));
    connect(mVacancyManager, SIGNAL(updateTable()), this, SLOT(updateVacancyTable()));
    connect(mVacancyManager, SIGNAL(updateAreas()), this, SLOT(updateRegionCb()));

    QDate currentDate = QDate::currentDate();
    QDate period = currentDate.addDays(-5);

    headers = QStringList()
            << trUtf8("")
            << trUtf8("ID")
            << trUtf8("Название")
            << trUtf8("Компания")
            << trUtf8("Требуемый опыт работы")
            << trUtf8("Регион")
            << trUtf8("Формат")
            << trUtf8("Занятость")
            << trUtf8("Зарплата")
            << trUtf8("Дата публикации");

    ui->dE_period->setDate(period);

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

        updateTurnBtn("Запустить автоматическое обновление", false);
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

    displayMessage("Обновление резюме");

    if(httpStatusCode >= "200" && httpStatusCode <= "299"){
        color = "green";

        QDateTime mNextUpdate = QDateTime::currentDateTime().addMSecs(FOUR_HOUR * 1000);

        mNotification->setPopupText("Резюме успешно обновлено");
        displayMessage("<strong style=\"color: " + color + ";\">Резюме успешно обновлено</strong>");
        displayMessage("Следующая проверка обновления: " + STRONG_STYLE + mNextUpdate.toString(Qt::DefaultLocaleShortDate) + "</strong>");

        mTimer->start();

    }else
    {
        if(httpStatusCode >= "300" && httpStatusCode <= "399"){
            color = "orange";
        }else{
            color = "red";
        }

        mNotification->setPopupText("Ошибка обновления\nCode: " + httpStatusCode);

        displayMessage("<strong style=\"color: " + color + ";\">Ошибка обновления</strong>");
        displayMessage("<strong style=\"color: " + color + ";\">Code: " + httpStatusCode + "</strong>");

        displayMessage("Авто-обновление выключено");

        updateTurnBtn("Запустить автоматическое обновление", false);
    }

    reply->deleteLater();
    MoveCursorToEnd();
}

void MainWindow::RequestmNextUpdateFinished(){
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    enum status{error, update_available, update_not_available};
    status update = status::error;
    QString message = "Ошибка при получении HTML-содержимого";

    if (reply->error() == QNetworkReply::NoError) {
        QString htmlContent = reply->readAll();

        QRegularExpression disabled("<button\\s+class=\"bloko-button bloko-button_kind-primary bloko-button_stretched\" type=\"button(.*?)=\"resume-update-button\"><span>Обновить дату </span></button>");
        QRegularExpressionMatch match = disabled.match(htmlContent);
        if (match.hasMatch()) {
            if(!match.captured(1).contains("disabled")){
                update = status::update_available;
                message = "Обновление доступно";
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

    displayMessage("Проверка обновления....");
    displayMessage(message);

    if(update == status::update_not_available){
        QDateTime mNextUpdate = QDateTime::currentDateTime().addMSecs(FOUR_HOUR / 8 * 1000);
        displayMessage("Следующая проверка: " + STRONG_STYLE + mNextUpdate.toString(Qt::DefaultLocaleShortDate) + "</strong>");
    }

    if(reply->error() == QNetworkReply::NoError && update == status::update_available){
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);
        QNetworkRequest request = getRequest(TOUCH_URL);
        QByteArray postData = "resume=" + Settings::instance().getIDResume().toUtf8() + "&undirectable=" + "true";\
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseFromServer(QNetworkReply*)));
        manager->post(request, postData);
    }

    if (message == "Ошибка при получении HTML-содержимого"){
        updateTurnBtn("Запустить автоматическое обновление", false);
        displayMessage("Авто-обновление выключено");
    }

    if(update == status::error){
        showMessageBox(message + "\nПроверьте корректность введённых параметров.\t\n");
        updateTurnBtn("Запустить автоматическое обновление", false);
    }else{
        mNotification->setPopupText(message);
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

void MainWindow::initResumeSettingMenu() {
    // Resume
    QCheckBox *checkBox_resume = new QCheckBox(tr("Скрыть панель с параметрами"));

    QHBoxLayout *layout_resume = new QHBoxLayout();
    layout_resume->addWidget(checkBox_resume);
    layout_resume->setAlignment(Qt::AlignCenter);

    QWidget *widget_resume = new QWidget();
    widget_resume->setLayout(layout_resume);

    QWidgetAction *widgetAction_resume = new QWidgetAction(this);
    widgetAction_resume->setDefaultWidget(widget_resume);

    mResumeSettingMenu->addAction(widgetAction_resume);

    QObject::connect(checkBox_resume, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked) {
            ui->f_resume_menu->hide();
        } else {
            ui->f_resume_menu->show();
        }
    });

    menuBar()->addMenu(mResumeSettingMenu);

    // Parsing
    QCheckBox *checkBox_parsing = new QCheckBox(tr("Скрыть панель с параметрами"));

    QHBoxLayout *layout_parsing = new QHBoxLayout();
    layout_parsing->addWidget(checkBox_parsing);
    layout_parsing->setAlignment(Qt::AlignCenter);

    QWidget *widget_parsing  = new QWidget();
    widget_parsing->setLayout(layout_parsing);

    QWidgetAction *widgetAction_parsing = new QWidgetAction(this);
    widgetAction_parsing->setDefaultWidget(widget_parsing);

    mParsingSettingMenu->addAction(widgetAction_parsing);

    QObject::connect(checkBox_parsing , &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked) {
            ui->f_parsing_menu->hide();
        } else {
            ui->f_parsing_menu->show();
        }
        //updateVacancyTable();
    });

    menuBar()->addMenu(mParsingSettingMenu);
}

void MainWindow::initTrayMenu(){
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    QAction* viewWindow = new QAction("Открыть панель", this);
    QAction* quitAction = new QAction("Выход", this);

    mTrayMenu->addAction(viewWindow);
    mTrayMenu->addAction(quitAction);

    trayIcon->setIcon((QIcon(LOGO_PATH)));
    trayIcon->setContextMenu(mTrayMenu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    QObject::connect(quitAction, &QAction::triggered, this, [this](){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, TITLE, "Вы действительно хотите закрыть приложение?\n", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            show();
        }else if (reply == QMessageBox::Yes) {
            QApplication::quit();
        }
    });
}

void MainWindow::updateTurnBtn(QString text, bool value){
    ui->pB_turn->setProperty("turn", value);
    ui->pB_turn->setText(text);
}

void MainWindow::displayMessage(QString message){
    if (message[message.size() - 1] == "."){
        message.chop(1);
    }
    ui->tE_logInfo->insertHtml("<br/>" + STRONG_STYLE + "[" + getCurrentDateTime() + "]: </strong>" + message);
}

void MainWindow::exportTableToXml(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить как"), "", tr("XML Files (*.xml)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Уведомление"), tr("Не удалось открыть файл для записи: \n%1\n").arg(file.errorString()));
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Table");

    int columnCount = headers.size();

    // Write header
    xmlWriter.writeStartElement("Header");
    for (int column = 1; column < columnCount; ++column) {
        xmlWriter.writeStartElement("Column");
        xmlWriter.writeAttribute("index", QString::number(column));
        xmlWriter.writeCharacters(headers.at(column));
        xmlWriter.writeEndElement(); // Column
    }
    xmlWriter.writeEndElement(); // Header

    int rowCount = ui->tW_vacancy->rowCount();

    // Write rows
    for (int row = 0; row < rowCount; ++row) {
        xmlWriter.writeStartElement("Row");
        for (int column = 1; column < columnCount; ++column) {
            QWidget *widget = ui->tW_vacancy->cellWidget(row, column);
            if (widget) {
                if (QPushButton *button = qobject_cast<QPushButton *>(widget)) {
                    xmlWriter.writeStartElement("Column");
                    xmlWriter.writeAttribute("index", QString::number(column));
                    xmlWriter.writeAttribute("field", "URL");
                    xmlWriter.writeCharacters(button->property("url").toString());
                    xmlWriter.writeEndElement(); // Column
                }
            } else {
                QTableWidgetItem *item = ui->tW_vacancy->item(row, column);
                if (item) {
                    xmlWriter.writeStartElement("Column");
                    xmlWriter.writeAttribute("index", QString::number(column));
                    xmlWriter.writeAttribute("field", headers.at(column));
                    xmlWriter.writeCharacters(item->text());
                    xmlWriter.writeEndElement(); // Column
                }
            }
        }
        xmlWriter.writeEndElement(); // Row
    }

    xmlWriter.writeEndElement(); // Table
    xmlWriter.writeEndDocument();

    file.close();
    QMessageBox::information(this, tr("Уведомление"), tr("Данные таблицы успешно экспортированы:\n%1\n").arg(fileName));
}


void MainWindow::updateRegionCb(){
    QString parent_id = mHHDictData.areaDict[0];
    ui->cB_region->clear();

    if (parent_id.size() > 0){
        ui->cB_region->addItem("Все");
        foreach (auto key, mVacancyManager->areas.keys()) {
            if (key == parent_id) {
                foreach (auto region, mVacancyManager->areas[key]) {
                    ui->cB_region->addItem(region.second);
                }
            }
        }
    }
}

void MainWindow::updateVacancyTable() {
    // Set the number of columns
    ui->tW_vacancy->setColumnCount(headers.size());
    // Enable grid
    ui->tW_vacancy->setShowGrid(true);
    // Disable selection
    ui->tW_vacancy->setSelectionMode(QAbstractItemView::NoSelection);
    // Enable row selection
    ui->tW_vacancy->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Set column headers
    ui->tW_vacancy->setHorizontalHeaderLabels(headers);
    // Hide the first column
    ui->tW_vacancy->hideColumn(0);
    // Disable editing
    ui->tW_vacancy->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Set the number of rows to match the number of vacancies
    ui->tW_vacancy->setRowCount(mVacancyManager->vacancies.size());

    for (int i = 0; i < mVacancyManager->vacancies.size(); ++i) {
        const hh_manager::Vacancy& vacancy = mVacancyManager->vacancies[i];

        QPushButton *button = new QPushButton(vacancy.id);
        button->setStyleSheet("QPushButton {"
                              "    background-color: none;"
                              "    border: 0px solid red;"
                              "    color: #0d6efd;"
                              "}"
                              "QPushButton:hover {"
                              "    color: #242424;"
                              "}");
        button->setProperty("url", vacancy.alternateUrl);
        button->setProperty("id", vacancy.id);
        connect(button, &QPushButton::clicked, this, [this, url = vacancy.alternateUrl](){ QDesktopServices::openUrl(QUrl(url)); });
        ui->tW_vacancy->setCellWidget(i, 1, button);

        QTableWidgetItem *item_name = new QTableWidgetItem(vacancy.name);
        ui->tW_vacancy->setItem(i, 2, item_name);

        QTableWidgetItem *item_employer= new QTableWidgetItem(vacancy.employer);
        ui->tW_vacancy->setItem(i, 3, item_employer);

        QTableWidgetItem *item_schedule = new QTableWidgetItem(vacancy.scheduleName);
        ui->tW_vacancy->setItem(i, 4, item_schedule);

        QTableWidgetItem *item_area = new QTableWidgetItem(vacancy.areaName);
        ui->tW_vacancy->setItem(i, 5, item_area);

        QTableWidgetItem *item_experience = new QTableWidgetItem(vacancy.experienceName);
        ui->tW_vacancy->setItem(i, 6, item_experience);

        QString salary;
        if (vacancy.salary.currency.size() > 0) {
            if (vacancy.salary.from > 0 && vacancy.salary.to > 0) {
                salary = "От " + QString::number(vacancy.salary.from) + " до " + QString::number(vacancy.salary.to);
                salary += " " + vacancy.salary.currency;
            } else if (vacancy.salary.from > 0) {
                salary = "От " + QString::number(vacancy.salary.from);
                salary += " " + vacancy.salary.currency;
            } else if (vacancy.salary.to > 0) {
                salary = "До " + QString::number(vacancy.salary.to);
                salary += " " + vacancy.salary.currency;
            }
            if (vacancy.salary.gross) {
                salary += " до вычета налогов";
            }
        }

        QTableWidgetItem *item_employment = new QTableWidgetItem(vacancy.employment);
        ui->tW_vacancy->setItem(i, 7, item_employment);

        QTableWidgetItem *item_salary = new QTableWidgetItem(salary);
        ui->tW_vacancy->setItem(i, 8, item_salary);

        QTableWidgetItem *item_published_at = new QTableWidgetItem(vacancy.publishedAt);
        ui->tW_vacancy->setItem(i, 9, item_published_at);
    }

    QHeaderView *header = ui->tW_vacancy->horizontalHeader();
    for (int i = 0; i < header->count(); ++i) {
        header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
    header->setSectionResizeMode(headers.size() - 1, QHeaderView::Stretch);

    ui->tW_vacancy->resizeColumnsToContents();
    ui->tW_vacancy->horizontalHeader()->setStretchLastSection(true);
    ui->tW_vacancy->horizontalHeader()->setSectionResizeMode(headers.size() - 1, QHeaderView::Stretch);

}

void MainWindow::on_pB_turn_clicked(){
    if (ui->pB_turn->property("turn") == true){
        displayMessage("Авто-обновление выключено");
        mTimer->stop();
        MoveCursorToEnd();
        updateTurnBtn("Запустить автоматическое обновление", false);
    }else{
        if(!checkCorrectlyFields()){
            showMessageBox("Заполните поля корректными значениями!\t\n");
        }else{
            displayMessage("Авто-обновление включено");
            mTimer->setInterval(FOUR_HOUR * 1000);
            sendRequest();
            updateTurnBtn("Остановить автоматическое обновление", true);
        }
    }
}

void MainWindow::on_pB_getVanacy_clicked(){
    ui->tW_vacancy->clear();

    hh_manager::HHVacancyParams params;
    params.text = ui->lE_keywords->text();
    params.text.replace(" ", "%20");
    params.text.replace("+", "%2B");
    params.text.replace("-", "%2D");
    params.text.replace("(", "%28");
    params.text.replace(")", "%29");
    params.text.replace("/", "%2F");

    if (ui->cB_region->currentIndex() == 0){
        params.area = mHHDictData.areaDict[0];
    }
    else{
        params.area = mVacancyManager->regionDict[ui->cB_region->currentText()];
    }

    params.perPage = QString::number(ui->sB_per_page->value());
    params.schedule = mHHDictData.scheduleDict[ui->cB_schedule->currentIndex()];
    params.experience = mHHDictData.experienceDict[ui->cB_experience->currentIndex()];

    if (ui->cB_only_name->isChecked()){
        params.searchField.append(mHHDictData.searchFieldDict[ui->cB_only_name->property("search_field_id").toInt()]) ;
    }

    if (ui->cB_only_company_name->isChecked()){
        params.searchField.append(mHHDictData.searchFieldDict[ui->cB_only_company_name->property("search_field_id").toInt()]) ;
    }

    if (ui->cB_only_description->isChecked()){
        params.searchField.append(mHHDictData.searchFieldDict[ui->cB_only_description->property("search_field_id").toInt()]) ;
    }

    if (ui->sB_salary->value() > 0){
        params.salary = QString::number(ui->sB_salary->value());
    }
    params.currency = mHHDictData.currencyDict[ui->cB_currency->currentIndex()];

    if (ui->cB_only_with_salary->isChecked()){
        params.onlyWithSalary = "true";
    }

    QDate currentDate = QDate::currentDate();
    QDate chooseDate = ui->dE_period->date();

    if (chooseDate > currentDate) {
        QDate period = currentDate.addDays(-5);
        ui->dE_period->setDate(period);
    } else {
        params.period = QString::number(chooseDate.daysTo(currentDate));
    }

    params.employment = mHHDictData.employmentDict[ui->cB_employment->currentIndex()];
    params.order = mHHDictData.orderDict[ui->cB_order->currentIndex()];
    mVacancyManager->getVacancies(params);
}

void MainWindow::on_pB_clear_resume_params_clicked(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, TITLE, "Вы действительно хотите очистить поля?\n", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->lE_hhtoken->clear();
        ui->lE_hhuid->clear();
        ui->lE_idResume->clear();
        ui->lE_xsrf->clear();
        ui->lE_url->clear();
    }
}

void MainWindow::on_pB_save_resume_params_clicked(){
    saveSetting();
}

void MainWindow::on_pB_load_resume_params_clicked(){
    loadSetting();
}

void MainWindow::on_pB_clear_resume_log_clicked(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, TITLE, "Вы действительно хотите очистить лог?\n", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->tE_logInfo->clear();
    }
}

void MainWindow::on_cB_sort_vacancies_currentIndexChanged(int index){
    this->updateVacancyTable();
}

void MainWindow::on_cB_area_currentIndexChanged(int index){
    updateRegionCb();
}

void MainWindow::on_dE_period_userDateChanged(const QDate &date){
    QDate currentDate = QDate::currentDate();
    QDate prev = currentDate.addDays(-1);
    if (date > prev) {
        ui->dE_period->setDate(prev);
    }
}

void MainWindow::on_pB_exportToXML_clicked(){
    exportTableToXml();
}

