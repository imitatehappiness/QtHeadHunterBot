
<p align="center">
  <a href="https://github.com/imitatehappiness/QtHeadHunterBot">
    <img src="https://cdn-icons-png.flaticon.com/512/5494/5494942.png" alt="Logo" width="200" height="200">  
  </a>

# Auto HeadHunter Resume Update


![GitHub stars](https://img.shields.io/github/stars/imitatehappiness/QtHeadHunterBot?style=social)
![GitHub forks](https://img.shields.io/github/forks/imitatehappiness/QtHeadHunterBot?style=social)
![GitHub watchers](https://img.shields.io/github/watchers/imitatehappiness/QtHeadHunterBot?style=social)
![GitHub followers](https://img.shields.io/github/followers/imitatehappiness?style=social)

>**Compiler:**  MinGW 5.3.0 32bit
>
>**Profile Qt:**  5.11.0 MinGW 32bit
>
>**System:**  Win11

## Предисловие

Выдача резюме на hh.ru, которую видит работодатель при поиске соискателей, постоянно пополняется новыми анкетами. В первую очередь отображаются те, что были созданы или обновлены недавно. То есть, если ваше резюме составлено давно и в последнее время вы не вносили в него никаких правок, на первые позиции выдачи не попадёт. Это снижает шансы на трудоустройство.

## Описание

**AutoHeadHunterUpdate** - это приложение на C++ QT с графическим пользовательским интерфейсом, предназначенное для облегчения обновления вашего резюме на портале вакансий HeadHunter. 

### Для правильной работы приложения требуются следующие параметры:

+ **id resume:** Уникальный идентификатор вашего резюме на HeadHunter.
+ **hhtoken:** Токен API HeadHunter, необходимый для аутентификации.
+ **hhuid:** Идентификатор пользователя HeadHunter, связанный с вашей учетной записью.
+ **xsrf:** Межсайтовый запрос поддельного токена для безопасной связи с веб-сайтом HeadHunter.
+ **url:** Ваш региональный url HeadHunter. Например "https://moscow.hh.ru".

### Где найти необходимые параметры

+ **hhtoken, hhuid, xsrf** можно увидеть в веб-консоли после нажатия кнопки "Обновить дату". Выберите запрос на публикацию (*hh.ru/applicant/resumes/touch*) и перейдите на вкладку Cookies. В списке можно найти необходимые значения.

  <details>
  <summary>Пример</summary>
  <p align="center">
      <img src="https://github.com/imitatehappiness/QtHeadHunterBot/assets/79199956/a459611d-b8c9-48d0-abe5-c62b81816254" />
  </p>
  </details>

+ **resume id, url** можно найти в адресной строке на странице резюме.


## Возможности
+ **Графический пользовательский интерфейс (GUI):** Удобный графический интерфейс обеспечивает легкую навигацию и взаимодействие с приложением.
+ **Фоновый режим:** Приложение работает в фоновом режиме, автоматически обновляя ваше резюме, не мешая другим вашим задачам. Для полного закрытия приложения необходимо выбрать соответствующий пункт в меню, доступный в системном трее.
+ **Управление настройками:** Приложение предоставляет возможность сохранять и загружать ваши настройки конфигурации с помощью файлов JSON.

> :information_source: **Информация:**<br>
> Если обновление недоступно на момент запуска, приложение будет периодически (каждые 30 минут) отправлять запросы, пока резюме не обновится. После успешного обновления, резюме будет автоматически обновляться раз в 4 часа.

> ⚠️ **Предупреждение:**<br>
> При частых запросах к обновлению на сайте может появиться капча. В этом случае необходимо ввести капчу, а затем запустить автообновление.

## Сборка
Запустите windeployqt в командной строке, указав в качестве аргумента местоположение вашего проекта:
```
windeployqt --compiler-runtime C:\project_folder\my_project.exe
```
Теперь my_project.exe будет иметь все необходимые библиотеки dll и выполняться.

## Демонстрация
<img src="https://github.com/imitatehappiness/QtHeadHunterBot/assets/79199956/b8544300-65e4-4d42-8c26-e402d6315de5" />



