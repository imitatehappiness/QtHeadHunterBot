#include "mainwindow.h"
#include "qglobal.h"
#include "ui_mainwindow.h"

#include "ru_locale.h"

#include <QCheckBox>
#include <QAction>
#include <QWidgetAction>
#include <QFileDialog>
#include <QCloseEvent>
#include <QIcon>
#include <QDateTime>
#include <QMessageBox>
#include <QTextCursor>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QIntValidator>
#include <QTableWidget>
#include <QXmlStreamWriter>

const QString AUTHOR = "@imitatehappiness";
const QString TOUCH_URL = "https://hh.ru/applicant/resumes/touch";
const QString LOGO_PATH = ":/resources/icons/hh-logo.png";
const QString TITLE = AUTHOR + " | hh-bot";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mResumeSettingMenu(new QMenu(HHManager::promotion,this))
    , mParsingSettingMenu(new QMenu(HHManager::parsingResume,this))
    , mTrayMenu(new QMenu(this))
{
    ui->setupUi(this);
    setWindowTitle(TITLE);

    this->mVacancyManager = new HHManager::VacancyManager();
    this->mVacancyManager->getAreas();
    this->mResumeManager = new HHManager::ResumeManager();

    this->initResumeSettingMenu();
    this->initTrayMenu();

    connect(this->mVacancyManager, SIGNAL(updateTable()), this, SLOT(updateVacancyTable()));
    connect(this->mVacancyManager, SIGNAL(updateAreas()), this, SLOT(updateRegionCb()));

    connect(this->mResumeManager, SIGNAL(moveCursorToEnd()), this, SLOT(moveCursorToEnd()));
    connect(this->mResumeManager, SIGNAL(displayMessage(QString)), this, SLOT(displayMessage(QString)));
    connect(this->mResumeManager, SIGNAL(updateTurnBtn(QString, bool)), this, SLOT(updateTurnBtn(QString, bool)));
    connect(this->mResumeManager, SIGNAL(showMessageBox(QString)), this, SLOT(showMessageBox(QString)));

    QDate currentDate = QDate::currentDate();
    QDate period = currentDate.addDays(-5);

    this->headers = QStringList()
            << ""
            << HHManager::idHeader
            << HHManager::nameHeader
            << HHManager::companyHeader
            << HHManager::experienceHeader
            << HHManager::regionHeader
            << HHManager::scheduleHeader
            << HHManager::employmentHeader
            << HHManager::salaryHeader
            << HHManager::publicationDateHeader;

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

void MainWindow::loadResumeParams(){
    QString fileName = QFileDialog::getOpenFileName(this, "", "", tr("Settings (*.json);;All Files (*)"));
    if("" != fileName){
        HHManager::ResumeSettings::instance().loadSettings(fileName);

        ui->lE_idResume->setText(HHManager::ResumeSettings::instance().getIDResume());
        ui->lE_hhtoken->setText(HHManager::ResumeSettings::instance().getHhtoken());
        ui->lE_hhuid->setText(HHManager::ResumeSettings::instance().getHhuid());
        ui->lE_xsrf->setText(HHManager::ResumeSettings::instance().getXsrf());
        ui->lE_url->setText(HHManager::ResumeSettings::instance().getUrl());

        this->updateTurnBtn(HHManager::startAutoUpdate, false);
    }
}

void MainWindow::saveResumeParams(){
    QString fileName = QFileDialog::getSaveFileName(this, "", "", tr("Settings (*.json);;All Files (*)"));
    if("" != fileName){
        this->setResumeParams();
        HHManager::ResumeSettings::instance().saveSettings(fileName);
    }
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

void MainWindow::setResumeParams(){
    HHManager::ResumeSettings::instance().setIDResume(ui->lE_idResume->text());
    HHManager::ResumeSettings::instance().setHhtoken(ui->lE_hhtoken->text());
    HHManager::ResumeSettings::instance().setHhuid(ui->lE_hhuid->text());
    HHManager::ResumeSettings::instance().setXsrf(ui->lE_xsrf->text());
    HHManager::ResumeSettings::instance().setUrl(ui->lE_url->text());
}

QString MainWindow::getCurrentDateTime(){
    QLocale locale;
    return locale.toString(QDateTime::currentDateTime(), QLocale::ShortFormat);
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

void MainWindow::moveCursorToEnd(){
    QTextCursor cursor = ui->tE_logInfo->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->tE_logInfo->setTextCursor(cursor);
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
    QCheckBox *checkBox_resume = new QCheckBox(HHManager::hideParamsPanel);

    QHBoxLayout *layout_resume = new QHBoxLayout();
    layout_resume->addWidget(checkBox_resume);
    layout_resume->setAlignment(Qt::AlignCenter);

    QWidget *widget_resume = new QWidget();
    widget_resume->setLayout(layout_resume);

    QWidgetAction *widgetAction_resume = new QWidgetAction(this);
    widgetAction_resume->setDefaultWidget(widget_resume);

    this->mResumeSettingMenu->addAction(widgetAction_resume);

    QObject::connect(checkBox_resume, &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked) {
            ui->f_resume_menu->hide();
        } else {
            ui->f_resume_menu->show();
        }
    });

    menuBar()->addMenu(mResumeSettingMenu);

    // Parsing
    QCheckBox *checkBox_parsing = new QCheckBox(HHManager::hideParamsPanel);

    QHBoxLayout *layout_parsing = new QHBoxLayout();
    layout_parsing->addWidget(checkBox_parsing);
    layout_parsing->setAlignment(Qt::AlignCenter);

    QWidget *widget_parsing  = new QWidget();
    widget_parsing->setLayout(layout_parsing);

    QWidgetAction *widgetAction_parsing = new QWidgetAction(this);
    widgetAction_parsing->setDefaultWidget(widget_parsing);

    this->mParsingSettingMenu->addAction(widgetAction_parsing);

    QObject::connect(checkBox_parsing , &QCheckBox::stateChanged, this, [this](int state) {
        if (state == Qt::Checked) {
            ui->f_parsing_menu->hide();
        } else {
            ui->f_parsing_menu->show();
        }
    });

    menuBar()->addMenu(mParsingSettingMenu);
}

void MainWindow::initTrayMenu(){
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    QAction* viewWindow = new QAction(HHManager::openParamsPanel, this);
    QAction* quitAction = new QAction(HHManager::exitApp, this);

    this->mTrayMenu->addAction(viewWindow);
    this->mTrayMenu->addAction(quitAction);

    trayIcon->setIcon((QIcon(LOGO_PATH)));
    trayIcon->setContextMenu(mTrayMenu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));

    QObject::connect(quitAction, &QAction::triggered, this, [this](){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, TITLE, HHManager::confirmExit + "\n", QMessageBox::Yes | QMessageBox::No);
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
    QString fileName = QFileDialog::getSaveFileName(this, HHManager::saveAs, "", tr("XML Files (*.xml)"));
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, HHManager::notification, HHManager::fileOpenError + (": \n" + file.errorString() + "\n"));
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
    QMessageBox::information(this, HHManager::notification, HHManager::tableDataExported + ":\n" + fileName + "\n");
}


void MainWindow::updateRegionCb(){
    QString parent_id = areaDict[0];
    ui->cB_region->clear();

    if (parent_id.size() > 0){
        ui->cB_region->addItem("Все");
        foreach (auto key, this->mVacancyManager->areas.keys()) {
            if (key == parent_id) {
                foreach (auto region, this->mVacancyManager->areas[key]) {
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
    ui->tW_vacancy->setRowCount(this->mVacancyManager->vacancies.size());

    for (int i = 0; i < this->mVacancyManager->vacancies.size(); ++i) {
        const HHManager::Vacancy& vacancy = this->mVacancyManager->vacancies[i];

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
        connect(button, &QPushButton::clicked, this, [ url = vacancy.alternateUrl](){
            QDesktopServices::openUrl(QUrl(url));
        });

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
                salary = HHManager::from + " " + QString::number(vacancy.salary.from) + " " + HHManager::to + " " + QString::number(vacancy.salary.to);
                salary += " " + vacancy.salary.currency;
            } else if (vacancy.salary.from > 0) {
                salary = HHManager::from + " " + QString::number(vacancy.salary.from);
                salary += " " + vacancy.salary.currency;
            } else if (vacancy.salary.to > 0) {
                salary = HHManager::to + " " + QString::number(vacancy.salary.to);
                salary += " " + vacancy.salary.currency;
            }
            if (vacancy.salary.gross) {
                salary += " " + HHManager::beforeTax;
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
    setResumeParams();
    if (ui->pB_turn->property("turn") == true){
        displayMessage(HHManager::autoUpdateDisabled);
        mResumeManager->stopTimer();
        moveCursorToEnd();
        updateTurnBtn(HHManager::startAutoUpdate, false);
    }else{
        if(!checkCorrectlyFields()){
            showMessageBox(HHManager::fillCorrectValues + "!\t\n");
        }else{
            displayMessage(HHManager::autoUpdateEnabled);
            mResumeManager->setTimerInterval(FOUR_HOUR * 1000);
            mResumeManager->sendRequest();
            updateTurnBtn(HHManager::stopAutoUpdate, true);
        }
    }
}

void MainWindow::on_pB_getVanacy_clicked(){
    ui->tW_vacancy->clear();

    HHManager::HHVacancyParams params;

    params.text = ui->lE_keywords->text();
    params.text.replace(" ", "%20");
    params.text.replace("+", "%2B");
    params.text.replace("-", "%2D");
    params.text.replace("(", "%28");
    params.text.replace(")", "%29");
    params.text.replace("/", "%2F");

    if (ui->cB_region->currentIndex() == 0){
        params.area = areaDict[0];
    }
    else{
        params.area = this->mVacancyManager->regionDict[ui->cB_region->currentText()];
    }

    params.perPage = QString::number(ui->sB_per_page->value());
    params.schedule = scheduleDict[ui->cB_schedule->currentIndex()];
    params.experience = experienceDict[ui->cB_experience->currentIndex()];

    if (ui->cB_only_name->isChecked()){
        params.searchField.append(searchFieldDict[ui->cB_only_name->property("search_field_id").toInt()]) ;
    }

    if (ui->cB_only_company_name->isChecked()){
        params.searchField.append(searchFieldDict[ui->cB_only_company_name->property("search_field_id").toInt()]) ;
    }

    if (ui->cB_only_description->isChecked()){
        params.searchField.append(searchFieldDict[ui->cB_only_description->property("search_field_id").toInt()]) ;
    }

    if (ui->sB_salary->value() > 0){
        params.salary = QString::number(ui->sB_salary->value());
    }
    params.currency = currencyDict[ui->cB_currency->currentIndex()];

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

    params.employment = employmentDict[ui->cB_employment->currentIndex()];
    params.order = orderDict[ui->cB_order->currentIndex()];
    this->mVacancyManager->getVacancies(params);
}

void MainWindow::on_pB_clear_resume_params_clicked(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, TITLE, HHManager::clearFields + "\n", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->lE_hhtoken->clear();
        ui->lE_hhuid->clear();
        ui->lE_idResume->clear();
        ui->lE_xsrf->clear();
        ui->lE_url->clear();
    }
}

void MainWindow::on_pB_save_resume_params_clicked(){
    this->saveResumeParams();
}

void MainWindow::on_pB_load_resume_params_clicked(){
    this->loadResumeParams();
}

void MainWindow::on_pB_clear_resume_log_clicked(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, TITLE, HHManager::clearLogs + "\n", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        ui->tE_logInfo->clear();
    }
}

void MainWindow::on_dE_period_userDateChanged(const QDate &date){
    QDate currentDate = QDate::currentDate();
    QDate prev = currentDate.addDays(-1);
    if (date > prev) {
        ui->dE_period->setDate(prev);
    }
}

void MainWindow::on_pB_exportToXML_clicked(){
    this->exportTableToXml();
}


