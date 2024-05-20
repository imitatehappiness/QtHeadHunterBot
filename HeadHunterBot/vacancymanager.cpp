#include "vacancymanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace HHManager{

VacancyManager::VacancyManager(){

}

void VacancyManager::getVacancies(HHVacancyParams params){
    keywords = params.text;

    keywords.replace("%20", " ");
    keywords.replace("%2B", "+");
    keywords.replace("%2D", "-");
    keywords.replace("%28", "(");
    keywords.replace("%29", ")");
    keywords.replace("%2F", "/");

    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QUrl url(VACANCIES_URL + params.getParamsForUrl());
    QNetworkRequest request(url);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseVacancies(QNetworkReply*)));

    manager->get(request);
}

void VacancyManager::getAreas(){
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QUrl url(AREAS_URL);
    QNetworkRequest request(url);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(responseAreas(QNetworkReply*)));

    manager->get(request);
}

void VacancyManager::responseVacancies(QNetworkReply *reply)
{
    vacancies.clear();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();

        QJsonParseError parseError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "Error parsing JSON:" << parseError.errorString();
            return;
        }

        if (!jsonResponse.isObject()) {
            qDebug() << "JSON response is not an object.";
            return;
        }

        QJsonObject jsonObject = jsonResponse.object();

        if (jsonObject.contains("items") && jsonObject["items"].isArray()) {
            QJsonArray itemsArray = jsonObject["items"].toArray();
            foreach (const QJsonValue &value, itemsArray) {
                QJsonObject itemObject = value.toObject();

                Vacancy vacancy;

                vacancy.id = (itemObject["id"].toString());
                vacancy.name = (itemObject["name"].toString());

                vacancy.alternateUrl = (itemObject["alternate_url"].toString());

                if (itemObject.contains("area") && itemObject["area"].isObject()) {
                    QJsonObject Object = itemObject["area"].toObject();
                    vacancy.areaName = Object["name"].toString();
                }

                if (itemObject.contains("experience") && itemObject["experience"].isObject()) {
                    QJsonObject Object = itemObject["experience"].toObject();
                    vacancy.experienceName = Object["name"].toString();
                }

                if (itemObject.contains("schedule") && itemObject["schedule"].isObject()) {
                    QJsonObject Object = itemObject["schedule"].toObject();
                    vacancy.scheduleName = Object["name"].toString();
                }

                if (itemObject.contains("schedule") && itemObject["schedule"].isObject()) {
                    QJsonObject Object = itemObject["schedule"].toObject();
                    vacancy.scheduleName = Object["name"].toString();
                }

                if (itemObject.contains("salary") && itemObject["salary"].isObject()) {
                    QJsonObject Object = itemObject["salary"].toObject();

                    if (Object["from"].toString() != "null"){
                        vacancy.salary.from = Object["from"].toInt();
                        if (vacancy.salary.from > 10000000) vacancy.salary.from  = 0;
                    }
                    if (Object["to"].toString() != "null"){
                        vacancy.salary.to = Object["to"].toInt();
                        if (vacancy.salary.to > 10000000) vacancy.salary.to  = 0;
                    }
                    vacancy.salary.gross = Object["gross"].toBool();

                    vacancy.salary.currency = Object["currency"].toString();
                }
                if (itemObject.contains("employer") && itemObject["employer"].isObject()) {
                    QJsonObject Object = itemObject["employer"].toObject();
                    vacancy.employer = Object["name"].toString();
                }
                if (itemObject.contains("employment") && itemObject["employment"].isObject()) {
                    QJsonObject Object = itemObject["employment"].toObject();
                    vacancy.employment = Object["name"].toString();
                }

                vacancy.publishedAt = (itemObject["published_at"].toString());
                vacancy.publishedAt.replace("T", " ");
                vacancy.publishedAt.replace("+", " +");

                this->vacancies.push_back(vacancy);
            }
        } else {
            qDebug() << "JSON response does not contain items array.";
        }

        reply->deleteLater();
    } else {
        qDebug() << "Error occurred:" << reply->errorString();
        reply->deleteLater();
    }

    emit updateTable();
}

void VacancyManager::responseAreas(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();

        QJsonParseError parseError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "Error parsing JSON:" << parseError.errorString();
            return;
        }

        if (!jsonResponse.isArray()) {
            qDebug() << "JSON response is not an array.";
            return;
        }

        QJsonArray areasArray = jsonResponse.array();
        foreach (const QJsonValue &value, areasArray) {
            QJsonObject areaObject = value.toObject();

            QString id = areaObject["id"].toString();
            QString name = areaObject["name"].toString();
            QString parentId = areaObject["parent_id"].toString();

            this->areas[parentId].push_back(qMakePair(id, name));

            if (areaObject.contains("areas") && areaObject["areas"].isArray()) {
                QJsonArray subAreasArray = areaObject["areas"].toArray();
                foreach (const QJsonValue &subValue, subAreasArray) {
                    QJsonObject subAreaObject = subValue.toObject();
                    QString subId = subAreaObject["id"].toString();
                    QString subName = subAreaObject["name"].toString();
                    QString subParentId = subAreaObject["parent_id"].toString();

                    this->areas[subParentId].push_back(qMakePair(subId, subName));
                    this->regionDict[subName] = subId;
                }
            }
        }
    } else {
        qDebug() << "Error occurred:" << reply->errorString();
    }

    reply->deleteLater();
    emit updateAreas();
}

}
