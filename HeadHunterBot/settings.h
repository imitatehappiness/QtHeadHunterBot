#pragma once

#include <QObject>

/**
 * @brief The Settings class provides a singleton object for managing application settings.
 *
 * @details The Settings class is designed to manage various application settings as a singleton,
 * ensuring that there is only one instance throughout the application's lifecycle. It
 * allows you to store and retrieve settings data, such as the resume ID, HeadHunter token,
 * HeadHunter user ID, and Cross-Site Request Forgery (Xsrf) token.
 *
 * The class follows the Singleton design pattern, which means that it provides a static
 * method to access the single instance, ensuring there is only one Settings object that
 * can be accessed globally in the application.
 *
 */
class Settings : public QObject{
    Q_OBJECT
public:
    /**
     * @brief Returns a reference to the single instance of the Settings class.
     */
    static Settings& instance(){
        static Settings setting;
        return setting;
    }

    /**
     * @brief Loads settings data from the specified file path.
     *
     * @param path The file path from which settings data will be loaded.
     */
    void loadSettings(const QString& path);

    /**
     * @briefSaves the current settings data to the specified file path.
     *
     * @param path The file path to which settings data will be saved.
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

private:
    /**
     * @brief Private constructor to enforce singleton pattern.
     *
     * @param parent The parent QObject. Default is nullptr.
     */
    explicit Settings(QObject *parent = nullptr);

    /**
     * @brief Copy constructor and assignment operator are made private to prevent accidental copies of the singleton instance.
     */
    Settings(const Settings&);
    Settings & operator=(const Settings&);

    /**
     * @brief Converts the current settings data to JSON format.
     *
     * @return The JSON representation of the settings data as a QString.
     */
    QString parseDataToJSON();
    /**
     * @brief Parses the provided JSON data and updates the settings accordingly.
     *
     * @param data The JSON data to parse as a QString.
     */
    void parseDataFromJSON(const QString& data);

private:
    QString mIDResume;   /// Stores the ID of the resume.
    QString mHhtoken;    /// Stores the HeadHunter token.
    QString mHhuid;      /// Stores the HeadHunter user ID.
    QString mXsrf;       /// Stores the Cross-Site Request Forgery token.
};

