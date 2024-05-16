#pragma once

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace hh_manager
{

enum sort_type{
    ID = 0,
    NAME,
    EXPERIENS,
    AREA,
    SCHEDULE,
    SALARY,
    PUBLISHED_AT
};

struct Salary{
    int from, to;
    QString currency;
    bool gross;
};

struct Vacancy{
    QString id;
    QString name;
    QString alternateUrl;
    QString areaName;
    QString areaId;
    QString scheduleName;
    QString experienceName;
    QString publishedAt;
    QString employer;
    QString employment;
    Salary salary;
};

struct Data{
    QMap<int, QString> experienceDict{
        {0, ""},
        {1, "noExperience"},                        // Нет опыта
        {2, "between1And3"},                        // От 1 года до 3 лет
        {3, "between3And6"},                        // От 3 до 6 лет
        {4, "moreThan6"}                            // Более 6 лет
    };

    QMap<int, QString> scheduleDict{
        {0, ""},
        {1, "fullDay"},                             // Полный день
        {2, "shift"},                               // Сменный график
        {3, "flexible"},                            // Гибкий график
        {4, "remote"},                              // Удаленная работа
        {5, "flyInFlyOut"}                          // Вахтовый метод
    };

    QMap<int, QString> searchFieldDict{
        {0, "name"},                                 // в названии вакансии
        {1, "company_name"},                         // в названии компании
        {2, "description"},                          // в описании вакансии
    };

    QMap<int, QString> areaDict{
        {0, "113"},                                  // Россия
    };

    QMap<int, QString> currencyDict{
        {0, "RUR"},                                  // Рубли
        {1, "USD"},                                  // Доллары
        {2, "EUR"},                                  // Евро
    };

    QMap<int, QString> orderDict{
        {0, "publication_time"},                      // по дате
        {1, "salary_desc"},                           // по убыванию зарплаты
        {2, "salary_asc"},                            // по возрастанию зарплаты
        {3, "relevance"},                             // по соответствию
    };

    QMap<int, QString> employmentDict{
        {0, ""},
        {1, "full"},                                  // Полная занятость
        {2, "part"},                                  // Частичная занятость
        {3, "project"},                               // Проектная работа
        {4, "volunteer"},                             // Волонтерство
        {5, "probation"},                             // Стажировка
    };
};

struct HHVacancyParams{
    QString text;
    QString perPage;
    QString area;
    QString schedule;
    QString experience;
    QString salary;
    QString currency;
    QString onlyWithSalary;
    QString period;
    QString order;
    QString employment;

    QVector<QString> searchField;

    QString getParamsForUrl(){
        QString result;

        if (area.size() > 0){
            result += "area=" + area + "&";
        }
        if (perPage.size() > 0){
            result += "&per_page=" + perPage + "&";
        }
        if (schedule.size() > 0){
            result += "schedule=" + schedule + "&";
        }
        if (experience.size() > 0){
            result += "experience=" + experience + "&";
        }
        if (text.size() > 0){
            result += "text=" + text + "&";
        }
        if (searchField.size() > 0){
            for (auto& field : searchField){
               result += "search_field=" + field + "&";
            }
        }
        if (salary.size() > 0){
            result += "salary=" + salary + "&";

            if (currency.size() > 0){
                result += "currency=" + currency + "&";
            }
        }
        if (onlyWithSalary.size() > 0){
            result += "only_with_salary=" + onlyWithSalary + "&";
        }
        if (period.size() > 0){
            result += "period=" + period + "&";
        }
        if (order.size() > 0){
            result += "order_by=" + order + "&";
        }
        if (employment.size() > 0){
            result += "employment=" + employment + "&";
        }

        if (result.size() > 0){
            result.chop(1);
        }

        return result;
    }
};

class VacancyManager : public QObject{
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса VacancyManager.
     */
    VacancyManager();
    /**
     * @brief Выполняет запрос на получение вакансий согласно указанным параметрам.
     *
     * @param params Параметры запроса вакансий.
     */
    void getVacancies(HHVacancyParams params);
    /**
     * @brief Выполняет запрос на получение списка областей.
     */
    void getAreas();
signals:
    /**
     * @brief Сигнал, отправляемый при обновлении таблицы вакансий.
     */
    void updateTable();
    /**
     * @brief Сигнал, отправляемый при обновлении списка областей.
     */
    void updateAreas();
private slots:
    /**
     * @brief Обработчик ответа на запрос вакансий.
     *
     * @param reply Ответ на запрос вакансий.
     */
    void responseVacancies(QNetworkReply *reply);
    /**
     * @brief Обработчик ответа на запрос списка областей.
     *
     * @param reply Ответ на запрос списка областей.
     */
    void responseAreas(QNetworkReply *reply);
public:
    QString keywords;                                           // Ключевые слова для поиска вакансий
    QVector<Vacancy> vacancies;                                 // Вектор вакансий
    QMap<QString, QVector<QPair<QString, QString>>> areas;      // Словарь областей
    QMap<QString, QString> regionDict;                          // Словарь регионов
};


}


