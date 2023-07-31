#pragma once

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent * event);\
public slots:
    void show();
private slots:
    void loadSetting();
    void saveSetting();
    void requestOnTimer();
    void responseFromServer(QNetworkReply *reply);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void on_pB_updateResume_clicked();
    void on_pB_startAutoUpdate_clicked();
    void on_pB_stopAutoUpdate_clicked();

private:
    void setSettings();
    void sendRequest();
    QString getCurrentDateTime();
    bool fieldsIsEmpty();
private:
    Ui::MainWindow *ui;

    QMenu *mSettingMenu;
    QAction *mLoadSetting;
    QAction *mSaveSetting;
    QTimer *mTimer;

    QMenu                 *trayMenu;
    QSystemTrayIcon       *trayIcon;
    QAction               *viewWindow;
    QAction               *quitAction;

    QNetworkAccessManager *mManager;
};
