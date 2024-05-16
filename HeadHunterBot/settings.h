#pragma once

#include <QObject>

/**
 * @brief Класс Settings предоставляет единственный объект для управления настройками приложения.
 *
 * @details Класс Settings предназначен для управления различными настройками приложения как синглтоном,
 * обеспечивая единственный экземпляр на протяжении жизненного цикла приложения. Он
 * позволяет сохранять и извлекать данные настроек, такие как идентификатор резюме, токен HeadHunter,
 * идентификатор пользователя HeadHunter и токен межсайтовой подделки запросов (Xsrf).
 *
 * Класс следует шаблону проектирования Singleton, что означает, что он предоставляет статический
 * метод доступа к единственному экземпляру, обеспечивая существование только одного объекта Settings,
 * к которому можно получить доступ глобально в приложении.
 */
class Settings : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Возвращает ссылку на единственный экземпляр класса Settings.
     */
    static Settings& instance(){
        static Settings setting;
        return setting;
    }

    /**
     * @brief Загружает данные настроек из указанного файла.
     *
     * @param path Путь к файлу, из которого будут загружены данные настроек.
     */
    void loadSettings(const QString& path);

    /**
     * @brief Сохраняет текущие данные настроек в указанный файл.
     *
     * @param path Путь к файлу, в который будут сохранены данные настроек.
     */
    void saveSettings(const QString& path);

    QString getIDResume() const;
    void setIDResume(const QString &IDResume);

    QString getHhtoken() const;
    void setHhtoken(const QString &hhtoken);

    QString getHhuid() const;
    void setHhuid(const QString &hhuid);

    QString getXsrf() const;
    void setXsrf(const QString &xsrf);

    QString getUrl() const;
    void setUrl(const QString &url);

private:
    /**
     * @brief Приватный конструктор для обеспечения шаблона синглтона.
     *
     * @param parent Родительский QObject. По умолчанию - nullptr.
     */
    explicit Settings(QObject *parent = nullptr);

    /**
     * @brief Конструктор копирования и оператор присваивания сделаны приватными для предотвращения случайных копий единственного экземпляра синглтона.
     */
    Settings(const Settings&);
    Settings & operator=(const Settings&);

    /**
     * @brief Преобразует текущие данные настроек в формат JSON.
     *
     * @return JSON-представление данных настроек в виде QString.
     */
    QString parseDataToJSON();
    /**
     * @brief Разбирает предоставленные данные JSON и обновляет настройки соответственно.
     *
     * @param data Данные JSON для разбора в виде QString.
     */
    void parseDataFromJSON(const QString& data);

private:
    QString mIDResume;   /// Хранит идентификатор резюме.
    QString mHhtoken;    /// Хранит токен HeadHunter.
    QString mHhuid;      /// Хранит идентификатор пользователя HeadHunter.
    QString mXsrf;       /// Хранит токен межсайтовой подделки запросов (Xsrf).
    QString mUrl;        /// Хранит URL hh.
};
