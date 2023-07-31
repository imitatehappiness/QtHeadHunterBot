#include "settings.h"

#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

Settings::Settings(QObject *parent)
    : QObject(parent)
    , mIDResume("")
    , mHhtoken("")
    , mHhuid("")
    , mXsrf("")
{

}

void Settings::loadSettings(const QString& path){
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

    parseDataFromJSON(result);
}

void Settings::saveSettings(const QString &path){
    QString result = parseDataToJSON();

    QFile file;
    file.setFileName(path + ".json");
    if (file.open(QFile::ReadWrite)){
        QTextStream out(&file);
        out << result;
    }
    file.close();
}

QString Settings::parseDataToJSON(){
    QJsonObject data;
    data["IDResume"] = this->mIDResume;
    data["Hhtoken"] = this->mHhtoken;
    data["Hhuid"] = this->mHhuid;
    data["Xsrf"] = this->mXsrf;

    QJsonDocument doc(data);

    QString result(doc.toJson(QJsonDocument::Indented));

    return result;
}

void Settings::parseDataFromJSON(const QString &data){
    QJsonDocument document = QJsonDocument::fromJson(data.toUtf8());

    QJsonObject jsonObj = document.object();
    mIDResume = jsonObj["IDResume"].toString();
    mHhtoken = jsonObj["Hhtoken"].toString();
    mHhuid = jsonObj["Hhuid"].toString();
    mXsrf = jsonObj["Xsrf"].toString();
}

QString Settings::getIDResume() const{
    return mIDResume;
}

QString Settings::getHhtoken() const{
    return mHhtoken;
}

QString Settings::getHhuid() const{
    return mHhuid;
}

QString Settings::getXsrf() const{
    return mXsrf;
}

void Settings::setIDResume(const QString& IDResume){
    mIDResume = IDResume;
}

void Settings::setHhtoken(const QString &hhtoken){
    mHhtoken = hhtoken;
}

void Settings::setHhuid(const QString &hhuid){
    mHhuid = hhuid;
}

void Settings::setXsrf(const QString &xsrf){
    mXsrf = xsrf;
}






