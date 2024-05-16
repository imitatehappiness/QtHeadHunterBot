#pragma once

#include <vacancymanager.h>

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

using namespace hh_manager;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:

    /**
    * @brief Конструктор для класса MainWindow.
    *
    * @param parent Родительский виджет (по умолчанию: nullptr).
    */
    MainWindow(QWidget *parent = nullptr);
    /**
    * @brief Деструктор для класса MainWindow.
    */
    ~MainWindow();
protected:
    /**
    * @brief Обработчик события закрытия главного окна.
    *
    * @param event Событие закрытия.
    */
    void closeEvent(QCloseEvent * event);
public slots:
    /**
    * @brief Показать главное окно.
    */
    void show();
private slots:
    /**
    * @brief Загрузка настроек из JSON файла.
    */
    void loadSetting();
    /**
    * @brief Сохранение настроек в JSON файл.
    */
    void saveSetting();
    /**
    * @brief Запрос настроек по таймеру.
    */
    void requestOnTimer();
    /**
    * @brief Обработка ответа от сервера после отправки запроса.
    *
    * @param reply Ответ от сервера.
    */
    void responseFromServer(QNetworkReply *reply);
    /**
    * @brief Обработка завершения запроса следующего обновления.
    */
    void RequestmNextUpdateFinished();
    /**
    * @brief Обработка активации значка в системном трее.
    *
    * @param reason Причина активации значка в трее.
    */
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    /**
    * @brief Обновить таблицу вакансий.
    */
    void updateVacancyTable();
    /**
    * @brief Обновить комбобокс с регионами.
    */
    void updateRegionCb();

    void on_pB_turn_clicked();
    void on_pB_getVanacy_clicked();
    void on_pB_load_resume_params_clicked();
    void on_pB_save_resume_params_clicked();
    void on_pB_clear_resume_params_clicked();
    void on_pB_clear_resume_log_clicked();
    void on_cB_sort_vacancies_currentIndexChanged(int index);
    void on_cB_area_currentIndexChanged(int index);
    void on_dE_period_userDateChanged(const QDate &date);
    void on_pB_exportToXML_clicked();
private:
    /**
    * @brief Установка настроек.
    */
    void setSettings();
    /**
    * @brief Отправка сетевого запроса к серверу hh_manager.
    */
    void sendRequest();
    /**
    * @brief Получение текущей даты и времени в виде форматированной строки.
    *
    * @return QString с текущей датой и временем.
    */
    QString getCurrentDateTime();
    /**
    * @brief Отображение сообщения в message box.
    *
    * @param message Сообщение для отображения.
    */
    void showMessageBox(const QString& message);
    /**
    * @brief Перемещение курсора в конец текстового браузера.
    */
    void MoveCursorToEnd();
    /**
    * @brief Проигрывание звукового уведомления.
    *
    * @param path Путь к звуковому файлу.
    */
    void playSound(const QString& path);
    /**
    * @brief Создание и возвращение объекта QNetworkRequest для выполнения GET запроса по указанному URL.
    *
    * @param url URL для запроса.
    */
    QNetworkRequest getRequest(const QString& url);
    /**
    * @brief Проверка корректности полей ввода.
    *
    * @return true, если поля ввода корректны, иначе false.
    */
    bool checkCorrectlyFields();
    /**
    * @brief Проверка совпадения текста с регулярным выражением.
    *
    * @param re Шаблон регулярного выражения.
    * @param text Текст для поиска совпадений.
    *
    * @return true, если есть хотя бы одно совпадение, иначе false.
    */
    bool hasMatch(const QString& re, const QString& text);
    /**
    * @brief Инициализация меню настроек резюме.
    */
    void initResumeSettingMenu();
    /**
    * @brief Инициализация меню системного трея.
    */
    void initTrayMenu();
    /**
    * @brief Парсинг строки следующего обновления для извлечения информации.
    *
    * @param nextUpdate Строка, содержащая информацию о следующем обновлении.
    */
    void parseStrNextUpdate(QString& nextUpdate);
    /**
    * @brief Обновление текста и состояния кнопки "turn".
    *
    * @param text Новый текст для отображения на кнопке.
    * @param value Новое состояние кнопки (true для включения, false для отключения).
    */
    void updateTurnBtn(QString text, bool value);
    /**
    * @brief Отображение сообщения пользователю.
    *
    * @param message Сообщение для отображения.
    */
    void displayMessage(QString message);
    /**
    * @brief Экспорт таблицы в XML.
    */
    void exportTableToXml();
private:
    Ui::MainWindow *ui;
    QMenu *mResumeSettingMenu;                      /// Меню для управления настройками резюме
    QMenu *mParsingSettingMenu;                     /// Меню для управления настройками парсинга
    QMenu *mTrayMenu;                               /// Меню для значка системного трея
    QTimer *mTimer;                                 /// Таймер для периодических задач
    PopUp* mNotification;                           /// Всплывающее уведомление
    hh_manager::VacancyManager *mVacancyManager;    /// Менеджер вакансий
    hh_manager::Data mHHDictData;                   /// Данные словаря hh_manager

    QStringList headers;
};
