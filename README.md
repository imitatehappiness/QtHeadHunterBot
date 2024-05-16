
<p align="center">
  <a href="https://github.com/imitatehappiness/QtHeadHunterBot">
    <img src="https://cdn-icons-png.flaticon.com/512/5494/5494942.png" alt="Logo" width="200" height="200">  
  </a>

# Auto HeadHunter Resume Update & Vacancies parser


![GitHub stars](https://img.shields.io/github/stars/imitatehappiness/QtHeadHunterBot?style=social)
![GitHub forks](https://img.shields.io/github/forks/imitatehappiness/QtHeadHunterBot?style=social)
![GitHub watchers](https://img.shields.io/github/watchers/imitatehappiness/QtHeadHunterBot?style=social)
![GitHub followers](https://img.shields.io/github/followers/imitatehappiness?style=social)

>**Compiler:**  MinGW 5.8.0 64bit
>
>**Profile Qt:**  5.15.2 MinGW 64bit
>
>**System:**  Win11

## Навигация
- [Предисловие](#предисловие)
- [Описание](#описание)
- [Возможности](#возможности)
- [Сборка](#сборка)
- [Демонстрация](#демонстрация)


<a name="Предисловие"></a> 
# Предисловие

Выдача резюме на hh.ru, которую видит работодатель при поиске соискателей, постоянно пополняется новыми анкетами. В первую очередь отображаются те, что были созданы или обновлены недавно. То есть, если ваше резюме составлено давно и в последнее время вы не вносили в него никаких правок, на первые позиции выдачи не попадёт. Это снижает шансы на трудоустройство.

<a name="Описание"></a>
# Описание

Приложение на C++/Qt с графическим пользовательским интерфейсом, предназначенное для облегчения обновления вашего резюме и парсинга вакансий на HeadHunter. 

### Для правильной работы "обновления резюме" требуются следующие параметры:

+ **id resume:** Уникальный идентификатор вашего резюме на HeadHunter.
+ **hhtoken:** Токен API HeadHunter, необходимый для аутентификации.
+ **hhuid:** Идентификатор пользователя HeadHunter, связанный с вашей учетной записью.
+ **xsrf:** Межсайтовый запрос поддельного токена для безопасной связи с веб-сайтом HeadHunter.
+ **url:** Ваш региональный url HeadHunter. Например "https://moscow.hh.ru".

### Где найти необходимые параметры

+ **hhtoken, hhuid, xsrf** можно увидеть в веб-консоли после нажатия кнопки "Обновить дату". Выберите запрос на публикацию (*hh.ru/applicant/resumes/touch*) и перейдите на вкладку Cookies. В списке можно найти необходимые значения.

  <details>
  <summary>Как перейти</summary>
  <p align="left">
      <img src="https://github.com/imitatehappiness/QtHeadHunterBot/assets/79199956/a459611d-b8c9-48d0-abe5-c62b81816254" />
  </p>
  </details>

+ **resume id, url** можно найти в адресной строке на странице резюме.

<a name="Возможности"></a>
# Возможности

+ **Графический пользовательский интерфейс (GUI):** Удобный графический интерфейс обеспечивает легкую навигацию и взаимодействие с приложением.

<a name="обновление_резюме"></a>
### Обновление резюме:

+ **Фоновый режим:** Приложение работает в фоновом режиме, автоматически обновляя ваше резюме, не мешая другим вашим задачам. Для полного закрытия приложения необходимо выбрать соответствующий пункт в меню, доступный в системном трее.
+ **Управление настройками:** Приложение предоставляет возможность сохранять и загружать ваши параметры.

> :information_source: **Информация:**<br>
> Если обновление недоступно на момент запуска, приложение будет периодически (каждые 30 минут) отправлять запросы, пока резюме не обновится. После успешного обновления, резюме будет автоматически обновляться раз в 4 часа.

> ⚠️ **Предупреждение:**<br>
> При частых запросах к обновлению на сайте может появиться капча. В этом случае необходимо ввести капчу, а затем запустить автообновление.

<a name="парсинг_вакансий"></a>
### Парсинг вакансий:

+ **Выдача основной информации для первичного анализа:** 
	+ ID 
	+ URL 
	+ Название вакансии
	+ Название компании
	+ Требуемый опыт работы 
	+ Регион 
	+ Формат работы
	+ Занятость
	+ Зарплата
	+ Дата публикации
+ **Фильтры:** Аналог фитрам hh.
+ **Простая сортировка результата:** Сортировка таблицы путем нажатия на хедер.
+ **Экспорт в XML:** Возможность экспортировать полученный результат в XML файл для дальнейшего анализа.

<a name="Сборка"></a>
# Сборка
 
Запустите windeployqt в командной строке, указав в качестве аргумента местоположение вашего проекта:
```
windeployqt --compiler-runtime C:\project_folder\HeadHunterBot.exe
```
Теперь HeadHunterBot.exe будет иметь все необходимые библиотеки dll и выполняться.

<a name="Демонстрация"></a>
# Демонстрация
<img src="data/screen/updater_1.png" />
<img src="data/screen/parsing_1.png" />
<img src="data/screen/parsing_2.png" />

### parse_vacancies.xml

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Table>
    <Header>
        <Column index="1">ID</Column>
        <Column index="2">Название</Column>
        <Column index="3">Компания</Column>
        <Column index="4">Требуемый опыт работы</Column>
        <Column index="5">Регион</Column>
        <Column index="6">Формат</Column>
        <Column index="7">Занятость</Column>
        <Column index="8">Зарплата</Column>
        <Column index="9">Дата публикации</Column>
    </Header>
    <Row>
        <Column index="1" field="URL">https://hh.ru/vacancy/97036056</Column>
        <Column index="2" field="Название">Программист С++/Qt</Column>
        <Column index="3" field="Компания">ITGLOBAL.COM</Column>
        <Column index="4" field="Требуемый опыт работы">Полный день</Column>
        <Column index="5" field="Регион">Санкт-Петербург</Column>
        <Column index="6" field="Формат">От 1 года до 3 лет</Column>
        <Column index="7" field="Занятость">Полная занятость</Column>
        <Column index="8" field="Зарплата">От 100000 до 300000 RUR</Column>
        <Column index="9" field="Дата публикации">2024-05-15 16:52:08 +0300</Column>
    </Row>
    <Row>
        <Column index="1" field="URL">https://hh.ru/vacancy/99059414</Column>
        <Column index="2" field="Название">Разработчик C++ (Qt) системы взаимодействия с электронным оборудованием</Column>
        <Column index="3" field="Компания">Крипто-Сервис</Column>
        <Column index="4" field="Требуемый опыт работы">Полный день</Column>
        <Column index="5" field="Регион">Москва</Column>
        <Column index="6" field="Формат">От 3 до 6 лет</Column>
        <Column index="7" field="Занятость">Полная занятость</Column>
        <Column index="8" field="Зарплата">От 180000 до 250000 RUR до вычета налогов</Column>
        <Column index="9" field="Дата публикации">2024-05-15 16:00:11 +0300</Column>
    </Row>
    <Row>
        <Column index="1" field="URL">https://hh.ru/vacancy/98551646</Column>
        <Column index="2" field="Название">Инженер-программист C++/Qt, медоборудование</Column>
        <Column index="3" field="Компания">НИИТФА</Column>
        <Column index="4" field="Требуемый опыт работы">Полный день</Column>
        <Column index="5" field="Регион">Москва</Column>
        <Column index="6" field="Формат">От 3 до 6 лет</Column>
        <Column index="7" field="Занятость">Полная занятость</Column>
        <Column index="8" field="Зарплата">От 150000 RUR</Column>
        <Column index="9" field="Дата публикации">2024-05-15 12:36:47 +0300</Column>
    </Row>
    <Row>
        <Column index="1" field="URL">https://hh.ru/vacancy/98436934</Column>
        <Column index="2" field="Название">Программист / Разработчик С++ (Qt)</Column>
        <Column index="3" field="Компания">Евротех</Column>
        <Column index="4" field="Требуемый опыт работы">Полный день</Column>
        <Column index="5" field="Регион">Москва</Column>
        <Column index="6" field="Формат">Нет опыта</Column>
        <Column index="7" field="Занятость">Полная занятость</Column>
        <Column index="8" field="Зарплата">От 150000 до 220000 RUR</Column>
        <Column index="9" field="Дата публикации">2024-05-15 09:26:23 +0300</Column>
    </Row>
    <Row>
        <Column index="1" field="URL">https://hh.ru/vacancy/91895639</Column>
        <Column index="2" field="Название">Программист С++/Qt, junior</Column>
        <Column index="3" field="Компания">НТЦ Радуга</Column>
        <Column index="4" field="Требуемый опыт работы">Полный день</Column>
        <Column index="5" field="Регион">Москва</Column>
        <Column index="6" field="Формат">От 1 года до 3 лет</Column>
        <Column index="7" field="Занятость">Полная занятость</Column>
        <Column index="8" field="Зарплата">От 80000 до 120000 RUR до вычета налогов</Column>
        <Column index="9" field="Дата публикации">2024-05-14 16:57:09 +0300</Column>
    </Row>
</Table>
```