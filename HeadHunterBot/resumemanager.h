#pragma once

#include <QObject>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace HHManager{

const QString TOUCH_URL = "https://hh.ru/applicant/resumes/touch";
const QString STRONG_STYLE = "<strong style=\"color: #0d6efd;\">";
const uint FOUR_HOUR = 14460;

class ResumeSettings : public QObject {
    Q_OBJECT
public:
    static ResumeSettings& instance(){
        static ResumeSettings setting;
        return setting;
    }
    void loadSettings(const QString& path);
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
    explicit ResumeSettings(QObject *parent = nullptr);
    ResumeSettings(const ResumeSettings&);
    ResumeSettings & operator=(const ResumeSettings&);
    QString parseDataToJSON();
    void parseDataFromJSON(const QString& data);

private:
    QString mIDResume;
    QString mHhtoken;
    QString mHhuid;
    QString mXsrf;
    QString mUrl;
};

class ResumeManager : public QObject{
    Q_OBJECT
public:
    ResumeManager();
    void sendRequest();
    void startTimer();
    void setTimerInterval(double value);
    void stopTimer();
signals:
    void moveCursorToEnd();
    void displayMessage(QString);
    void updateTurnBtn(QString, bool);
    void showMessageBox(QString);
private slots:
    void responseResumeUpdate(QNetworkReply *reply);
    void requestResumeUpdate();
    void requestOnTimer();
private:
    QNetworkRequest getRequest(const QString& url);
private:
    QTimer *mTimer;
};

}
