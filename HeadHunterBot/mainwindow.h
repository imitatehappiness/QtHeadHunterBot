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
    * @brief Handles the response from the server after sending a request.
    *
    * @param reply The network reply received from the server.
    */
    void RequestmNextUpdateFinished();
    /**
    * @brief Handles the activation of the system tray icon.
    *
    * @param reason The activation reason for the system tray icon.
    */
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
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
    * @brief Play notify sound
    */
    void playSound(const QString& path);
    /**
    * @brief Сreates and returns a QNetworkRequest object to execute a GET request to the specified URL.
    *
    * @param url The url
    */
    QNetworkRequest getRequest(const QString& url);
    /**
    * @brief Checks if any input fields are correctly.
    *
    * @return true if any input field is correctly, false otherwise.
    */
    bool checkCorrectlyFields();
    /**
    * @brief Checks if there is a match between a regular expression and a given text.
    *
    * @param re The regular expression pattern to be matched against the `text`.
    * @param text The input text in which the regular expression will be searched for matches.
    *
    * @return true if there is at least one match between the regular expression and the `text`, false otherwise.
    */
    bool hasMatch(const QString& re, const QString& text);
    /**
    * @brief initialization Setting Menu
    */
    void initSettingMenu();
    /**
    * @brief initialization Tray Menu
    */
    void initTrayMenu();
    /**
    * @brief initialization Log Menu
    */
    void initLogMenu();

    void parseStrNextUpdate(QString& nextUpdate);
private:
    Ui::MainWindow *ui;
    QMenu *mSettingMenu;                /// The menu for managing settings
    QMenu *mLogMenu;                    /// The menu for managing logs
    QMenu *mTrayMenu;                   /// The menu for the system tray icon
    QTimer *mTimer;                     /// Timer for periodic tasks
    PopUp* mNotification;               /// Popup notification
};
