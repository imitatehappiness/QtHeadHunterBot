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

class PopUp;

class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    /**
    * @brief Constructor for the MainWindow class.
    *
    * @param parent The parent widget (default: nullptr).
    */
    MainWindow(QWidget *parent = nullptr);
    /**
    * @brief Destructor for the MainWindow class.
    */
    ~MainWindow();
protected:
    /**
    * @brief Event handler for the close event of the main window.
    *
    * @param event The close event.
    */
    void closeEvent(QCloseEvent * event);
public slots:
    /**
    * @brief Shows the main window.
    */
    void show();
private slots:
    /**
    * @brief Loads settings from a JSON file.
    */
    void loadSetting();
    /**
    * @brief Loads settings from a JSON file.
    */
    void saveSetting();
    /**
    * @brief Loads settings from a file.
    */
    void requestOnTimer();
    /**
    * @brief Handles the response from the server after sending a request.
    *
    * @param reply The network reply received from the server.
    */
    void responseFromServer(QNetworkReply *reply);
    /**
    * @brief Handles the activation of the system tray icon.
    *
    * @param reason The activation reason for the system tray icon.
    */
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    /**
    * @brief Handles the click event on the "update resume" button.
    */
    void on_pB_updateResume_clicked();
    /**
    * @brief Handles the click event on the "start auto-update" button.
    */
    void on_pB_startAutoUpdate_clicked();
    /**
    * @brief Handles the click event on the "stop auto-update" button.
    */
    void on_pB_stopAutoUpdate_clicked();
private:
    /**
    * @brief Sets the settings.
    */
    void setSettings();
    /**
    * @brief Sends a network request to the HH server.
    */
    void sendRequest();
    /**
    * @brief Gets the current date and time as a formatted string.
    *
    * @return A QString containing the current date and time.
    */
    QString getCurrentDateTime();
    /**
    * @brief Checks if any input fields are empty.
    *
    * @return true if any input field is empty, false otherwise.
    */
    bool fieldsIsEmpty();
    /**
    * @brief Displays a message box with the given message.
    *
    * @param message The message to be displayed in the message box.
    */
    void showMessageBox(const QString& mesasge);
    /**
    * @brief Move the cursor to the end text browser
    */
    void MoveCursorToEnd();
    /**
    * @brief Set default style for text browser
    */
    void setDefaultStyleTextBrowser();
    /**
    * @brief play notify sound
    */
    void playSound(const QString& path);
private:
    Ui::MainWindow *ui;

    QMenu *mSettingMenu;                /// The menu for managing settings
    QAction *mLoadSetting;              /// Action for loading settings from a file
    QAction *mSaveSetting;              /// Action for saving settings to a file
    QTimer *mTimer;                     /// Timer for periodic tasks

    QMenu *mTrayMenu;                   /// The menu for the system tray icon
    QSystemTrayIcon *mTrayIcon;         /// The system tray icon
    QAction *mViewWindow;               /// Action to show the main window from the system tray
    QAction *mQuitAction;               /// Action to quit the application from the system tray

    QNetworkAccessManager *mManager;    /// Network access manager for handling HTTP requests

    PopUp* mNotification;               /// Popup notification

};
