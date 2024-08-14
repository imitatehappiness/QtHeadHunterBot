#pragma once

#include <vacancymanager.h>
#include <resumemanager.h>

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace HHManager;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent * event);
public slots:
    void show();
private slots:
    void loadResumeParams();
    void saveResumeParams();
    void updateVacancyTable();
    void updateRegionCb();
    void updateTurnBtn(QString text, bool value);
    void displayMessage(QString message);
    void showMessageBox(const QString& message);
    void moveCursorToEnd();

    void on_pB_turn_clicked();
    void on_pB_getVanacy_clicked();
    void on_pB_load_resume_params_clicked();
    void on_pB_save_resume_params_clicked();
    void on_pB_clear_resume_params_clicked();
    void on_pB_clear_resume_log_clicked();
    void on_dE_period_userDateChanged(const QDate &date);
    void on_pB_exportToXML_clicked();
    void on_pB_clearTable_clicked();

private:
    void setResumeParams();
    void playSound(const QString& path);
    bool checkCorrectlyFields();
    bool hasMatch(const QString& re, const QString& text);
    void initResumeSettingMenu();
    void exportTableToXml();
    QString getCurrentDateTime();
private:
    Ui::MainWindow *ui;
    QMenu *mResumeSettingMenu, *mParsingSettingMenu;
    QStringList mHeaders;
    HHManager::VacancyManager *mVacancyManager;
    HHManager::ResumeManager *mResumeManager;
};
