#pragma once

#include <QObject>

const QString AUTHOR = "@imitatehappiness";
const QString TITLE = AUTHOR + " | hh-bot";
const QString LOGO_PATH = ":/resources/icons/hh-logo.png";

namespace HHManager
{
    const QString readError              = "Ошибка чтения";
    const QString startAutoUpdate        = "Запустить автоматическое обновление";
    const QString stopAutoUpdate         = "Остановить автоматическое обновление";
    const QString resumeUpdate           = "Обновление резюме";
    const QString resumeUpdated          = "Резюме успешно обновлено";
    const QString nextUpdateCheck        = "Следующая проверка обновления";
    const QString updateError            = "Ошибка обновления";
    const QString autoUpdateDisabled     = "Авто-обновление выключено";
    const QString htmlContentError       = "Ошибка при получении HTML-содержимого";
    const QString updateAvailable        = "Обновление доступно";
    const QString checkingUpdate         = "Проверка обновления...";
    const QString nextCheck              = "Следующая проверка";
    const QString checkInputParams       = "Проверьте корректность введённых параметров";
    const QString hideParamsPanel        = "Скрыть панель с параметрами";
    const QString openParamsPanel        = "Открыть панель";
    const QString exitApp                = "Выход";
    const QString confirmExit            = "Вы действительно хотите закрыть приложение?";
    const QString saveAs                 = "Сохранить как";
    const QString notification           = "Уведомление";
    const QString fileOpenError          = "Не удалось открыть файл для записи";
    const QString tableDataExported      = "Данные таблицы успешно экспортированы";
    const QString all                    = "Все";
    const QString from                   = "От";
    const QString to                     = "До";
    const QString beforeTax              = "до вычета налогов";
    const QString fillCorrectValues      = "Заполните поля корректными значениями";
    const QString autoUpdateEnabled      = "Авто-обновление включено";
    const QString clearFields            = "Вы действительно хотите очистить поля?";
    const QString clearLogs              = "Вы действительно хотите очистить лог?";
    const QString promotion              = "Продвижение";
    const QString parsingResume          = "Парсинг резюме";

    const QString idHeader               = "ID (Ссылка)";
    const QString nameHeader             = "Название";
    const QString companyHeader          = "Компания";
    const QString experienceHeader       = "Требуемый опыт работы";
    const QString regionHeader           = "Регион";
    const QString scheduleHeader         = "Формат";
    const QString employmentHeader       = "Занятость";
    const QString salaryHeader           = "Зарплата";
    const QString publicationDateHeader  = "Дата публикации";
}
