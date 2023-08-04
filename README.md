
<p align="center">
  <a href="https://github.com/imitatehappiness/QtHeadHunterBot">
    <img src="https://cdn-icons-png.flaticon.com/512/5494/5494942.png" alt="Logo" width="200" height="200">  
  </a>

# Auto HeadHunter Resume Update
[![Typing SVG](https://readme-typing-svg.herokuapp.com?color=%2336BCF7&lines=Авто-обновление+резюме+HeadHunter...)](https://git.io/typing-svg)

## Описание

AutoHeadHunterUpdate - это приложение на C++ QT с графическим пользовательским интерфейсом, предназначенное для облегчения автоматического и ручного обновления вашего резюме на портале вакансий HeadHunter. Для правильной работы приложения требуются следующие поля:

+ **id resume:** Уникальный идентификатор вашего резюме на HeadHunter.
+ **hhtoken:** Токен API HeadHunter, необходимый для аутентификации.
+ **hhuid:** Идентификатор пользователя HeadHunter, связанный с вашей учетной записью.
+ **xsrf:** Межсайтовый запрос поддельного токена для безопасной связи с веб-сайтом HeadHunter.
+ **url:** Ваш региональный url HeadHunter. Например "https://moscow.hh.ru ".

> ⚠️ **Предупреждение:**<br>
> При частых запросах к обновлению на сайте может появиться капча, в этом случае необходимо ввести капчу, а затем запустить автообновление.

## Где найти необходимые параметры

+ **hhtoken, hhuid, xsrf** можно увидеть в веб-консоли после нажатия кнопки "Обновить дату". Выберите запрос на публикацию (*hh.ru/applicant/resumes/touch*) и перейдите на вкладку Cookies. В списке можно найти необходимые значения.
+ **resume id, url** можно найти в адресной строке на странице резюме.


## Возможности
+ **Фоновый режим:** Приложение может работать в фоновом режиме, автоматически обновляя ваше резюме, не мешая другим вашим задачам. Для полного закрытия приложения необходимо выбрать соответствующий пункт в меню, доступный в системном трее.
+ **Управление настройками:** Приложение предоставляет возможность сохранять и загружать ваши настройки конфигурации с помощью файлов JSON. Эта функция позволяет вам постоянно сохранять ваши учетные данные и обновлять настройки, поэтому вам не нужно повторно вводить их при каждом запуске приложения.<br><br>
  > :information_source: **Информация:**<br>
  > При первом запуске параметры необходимо ввести всего один раз и сохранить в файл: Настройки->Сохранить. <br>
+ **Графический пользовательский интерфейс (GUI):** Удобный графический интерфейс обеспечивает легкую навигацию и взаимодействие с приложением.
+ **Автоматические обновления:** Включите функцию автоматического обновления, чтобы ваше резюме оставалось актуальным без ручного вмешательства.<br><br>
  > :information_source: **Информация:**<br>
  > Если обновление недоступно на момент запуска "авто-обновления", приложение будет периодически (каждые 30 минут) отправлять запросы, пока резюме не обновится. После успешного обновления, резюме будет автоматически обновляться раз в 4 часа.

## Пример
<p align="center">
    <img src="https://github.com/imitatehappiness/QtHeadHunterBot/assets/79199956/b8544300-65e4-4d42-8c26-e402d6315de5" />
</p>

<h3 align="center">A passionate frontend developer from India</h3>

<p align="left"> <img src="https://komarev.com/ghpvc/?username=imitatehappiness&label=Profile%20views&color=0e75b6&style=flat" alt="imitatehappiness" /> </p>

<p align="left"> <a href="https://twitter.com/" target="blank"><img src="https://img.shields.io/twitter/follow/?logo=twitter&style=for-the-badge" alt="" /></a> </p>

<h3 align="left">Connect with me:</h3>
<p align="left">
<a href="https://linkedin.com/in/imitatehappiness" target="blank"><img align="center" src="https://raw.githubusercontent.com/rahuldkjain/github-profile-readme-generator/master/src/images/icons/Social/linked-in-alt.svg" alt="imitatehappiness" height="30" width="40" /></a>
</p>

<h3 align="left">Languages and Tools:</h3>
<p align="left"> <a href="https://www.w3schools.com/cpp/" target="_blank" rel="noreferrer"> <img src="https://raw.githubusercontent.com/devicons/devicon/master/icons/cplusplus/cplusplus-original.svg" alt="cplusplus" width="40" height="40"/> </a> <a href="https://www.qt.io/" target="_blank" rel="noreferrer"> <img src="https://upload.wikimedia.org/wikipedia/commons/0/0b/Qt_logo_2016.svg" alt="qt" width="40" height="40"/> </a> </p>

<p><img align="left" src="https://github-readme-stats.vercel.app/api/top-langs?username=imitatehappiness&show_icons=true&locale=en&layout=compact" alt="imitatehappiness" /></p>

<p>&nbsp;<img align="center" src="https://github-readme-stats.vercel.app/api?username=imitatehappiness&show_icons=true&locale=en" alt="imitatehappiness" /></p>

<p><img align="center" src="https://github-readme-streak-stats.herokuapp.com/?user=imitatehappiness&" alt="imitatehappiness" /></p>
