#include "resumemanager.h"
#include "ru_locale.h"

#include <QNetworkReply>
#include <QRegularExpression>

#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

namespace HHManager{


ResumeSettings::ResumeSettings(QObject *parent)
    : QObject(parent)
    , mIDResume("")
    , mHhtoken("")
    , mHhuid("")
    , mXsrf("")
    , mUrl("")
{

}

void ResumeSettings::loadSettings(const QString& path){
    QString result = "";
    QString temp = "";

    QFile file;
    file.setFileName(path);
    if (file.open(QFile::ReadWrite)){
        QTextStream in(&file);
        while (!in.atEnd()){
            in >> temp;
            result += temp;
        }
    }

    this->parseDataFromJSON(result);
}

void ResumeSettings::saveSettings(const QString &path){
    QString result = parseDataToJSON();

    QFile file;
    file.setFileName(path);
    if (file.open(QFile::ReadWrite)){
        QTextStream out(&file);
        out << result;
    }
    file.close();
}

QString ResumeSettings::parseDataToJSON(){
    QJsonObject data;
    data["IDResume"] = this->mIDResume;
    data["Hhtoken"] = this->mHhtoken;
    data["Hhuid"] = this->mHhuid;
    data["Xsrf"] = this->mXsrf;
    data["Url"] = this->mUrl;

    QJsonDocument doc(data);

    QString result(doc.toJson(QJsonDocument::Indented));

    return result;
}

void ResumeSettings::parseDataFromJSON(const QString &data){
    QJsonDocument document = QJsonDocument::fromJson(data.toUtf8());

    QJsonObject jsonObj = document.object();
    mIDResume = jsonObj["IDResume"].toString();
    mHhtoken = jsonObj["Hhtoken"].toString();
    mHhuid = jsonObj["Hhuid"].toString();
    mXsrf = jsonObj["Xsrf"].toString();
    mUrl = jsonObj["Url"].toString();
}

QString ResumeSettings::getUrl() const{
    return mUrl;
}

QString ResumeSettings::getIDResume() const{
    return mIDResume;
}

QString ResumeSettings::getHhtoken() const{
    return mHhtoken;
}

QString ResumeSettings::getHhuid() const{
    return mHhuid;
}

QString ResumeSettings::getXsrf() const{
    return mXsrf;
}

void ResumeSettings::setIDResume(const QString& IDResume){
    mIDResume = IDResume;
}

void ResumeSettings::setHhtoken(const QString &hhtoken){
    mHhtoken = hhtoken;
}

void ResumeSettings::setHhuid(const QString &hhuid){
    mHhuid = hhuid;
}

void ResumeSettings::setXsrf(const QString &xsrf){
    mXsrf = xsrf;
}

void ResumeSettings::setUrl(const QString &url){
    mUrl = url;
}

ResumeManager::ResumeManager() :
    mTimer(new QTimer(this))
{
    connect(mTimer, SIGNAL(timeout()), this, SLOT(requestOnTimer()));
}

void ResumeManager::sendRequest(){
    QString url = ResumeSettings::instance().getUrl().toUtf8();
    if(url[url.size() - 1] != "/"){
        url += "/";
    }
    QNetworkRequest request = getRequest(url + "resume/" + ResumeSettings::instance().getIDResume().toUtf8());
    QNetworkAccessManager* managermNextUpdate = new QNetworkAccessManager(this);
    QNetworkReply *reply = managermNextUpdate->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(requestResumeUpdate()));
}

void ResumeManager::startTimer(){
    this->mTimer->start();
}

void ResumeManager::setTimerInterval(double value){
    this->mTimer->setInterval(value);
}

void ResumeManager::stopTimer(){
    this->mTimer->stop();
}

void ResumeManager::responseResumeUpdate(QNetworkReply *reply){
    QString httpStatusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toString();
    QString httpReasonPhrase = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();

    QString color;

    emit displayMessage(HHManager::resumeUpdate);

    if(httpStatusCode >= "200" && httpStatusCode <= "299"){
        color = "green";

        QDateTime mNextUpdate = QDateTime::currentDateTime().addMSecs(FOUR_HOUR * 1000);

        emit displayMessage("<strong style=\"color: " + color + ";\">" + HHManager::resumeUpdated + "</strong>");
        emit displayMessage(HHManager::nextUpdateCheck + ": " + STRONG_STYLE + mNextUpdate.toString(Qt::DefaultLocaleShortDate) + "</strong>");

        this->mTimer->start();

    }else
    {
        if(httpStatusCode >= "300" && httpStatusCode <= "399"){
            color = "orange";
        }else{
            color = "red";
        }

        emit displayMessage("<strong style=\"color: " + color + ";\">" + HHManager::updateError + "</strong>");
        emit displayMessage("<strong style=\"color: " + color + ";\">Code: " + httpStatusCode + "</strong>");
        emit displayMessage(HHManager::autoUpdateDisabled);
        emit updateTurnBtn(HHManager::startAutoUpdate, false);
    }

    emit moveCursorToEnd();
    reply->deleteLater();
}

void ResumeManager::requestResumeUpdate(){
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    enum status{error, update_available, update_not_available};
    status update = status::error;
    QString message = HHManager::htmlContentError;

    if (reply->error() == QNetworkReply::NoError) {
        QString htmlContent = reply->readAll();

        QRegularExpression disabled("<button\\s+class=\"bloko-button bloko-button_kind-primary bloko-button_stretched\" type=\"button(.*?)=\"resume-update-button\"><span>Обновить дату </span></button>");
        QRegularExpressionMatch match = disabled.match(htmlContent);
        if (match.hasMatch()) {
            if(!match.captured(1).contains("disabled")){
                update = status::update_available;
                message =  HHManager::updateAvailable;
                this->mTimer->setInterval(FOUR_HOUR * 1000);
            }else{
                QRegularExpression nextUpdate("<div\\s+class=\"resume-sidebar-text\"\\s+data-qa=\"resume-update-message\">(.*?)</div>");
                match = nextUpdate.match(htmlContent);
                if(match.hasMatch()){
                    message = match.captured(1);
                    message = message.left(message.indexOf("«")).trimmed();
                    update = status::update_not_available;
                    this->mTimer->setInterval(FOUR_HOUR / 8 * 1000);
                    this->mTimer->start();
                }
            }
        }
    }

    emit displayMessage(HHManager::checkingUpdate);
    emit displayMessage(message);

    if(update == status::update_not_available){
        QDateTime mNextUpdate = QDateTime::currentDateTime().addMSecs(FOUR_HOUR / 8 * 1000);
        emit displayMessage(HHManager::nextCheck + ": " + STRONG_STYLE + mNextUpdate.toString(Qt::DefaultLocaleShortDate) + "</strong>");
    }

    if(reply->error() == QNetworkReply::NoError && update == status::update_available){
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);
        QNetworkRequest request = getRequest(TOUCH_URL);
        QByteArray postData = "resume=" + ResumeSettings::instance().getIDResume().toUtf8() + "&undirectable=" + "true";\
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseResumeUpdate(QNetworkReply*)));
        manager->post(request, postData);
    }

    if (message == HHManager::htmlContentError){
        emit updateTurnBtn(HHManager::startAutoUpdate, false);
        emit displayMessage(HHManager::autoUpdateDisabled);
    }

    if(update == status::error){
        emit showMessageBox(message + "\n" + HHManager::checkInputParams + "\t\n");
        emit updateTurnBtn(HHManager::startAutoUpdate, false);
    }

    emit moveCursorToEnd();
    reply->deleteLater();
}

void ResumeManager::requestOnTimer(){
    this->sendRequest();
}

QNetworkRequest ResumeManager::getRequest(const QString &url){
    QNetworkRequest request((QUrl(url)));

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);

    request.setRawHeader("X-Xsrftoken", ResumeSettings::instance().getXsrf().toUtf8());

    request.setRawHeader("Cookie", "hhtoken=" + ResumeSettings::instance().getHhtoken().toUtf8() + "; " +
                         "hhuid=" + ResumeSettings::instance().getHhuid().toUtf8() + "; " +
                         "_xsrf=" + ResumeSettings::instance().getXsrf().toUtf8() + "; "
                         );

    return request;
}

}
