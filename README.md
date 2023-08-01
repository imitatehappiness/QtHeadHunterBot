
<p align="center">
  <a href="https://github.com/imitatehappiness/QtHeadHunterBot">
    <img src="https://cdn-icons-png.flaticon.com/512/5494/5494942.png" alt="Logo" width="200" height="200">  
  </a>

# Auto HeadHunter Resume Update

## Description
AutoHeadHunterUpdate is a C++ QT application with a graphical user interface designed to facilitate the automatic and manual updating of your resume on the HeadHunter job portal. The application requires the following fields to function properly:

+ **id resume:** The unique identifier of your resume on HeadHunter.
+ **hhtoken:** HeadHunter API token required for authentication.
+ **hhuid:** HeadHunter user ID associated with your account.
+ **xsrf:** Cross-Site Request Forgery token for secure communication with the HeadHunter website.

The application offers two main modes of updating:

+ **One-Time Update:** Allows you to manually update your resume on the HeadHunter website instantly.
+ **Auto-Update:** Automatically updates your resume every 4 hours.

## Where to find the necessary parameters

+ hhtoken, hhuid, xsrf can be seen in the Web Console after clicking the "Update Date" button. Select our POST request (hh.ru/applicant/resumes/touch) and go to the Cookies tab . In the list you can find the necessary values.
+ resume id can be found in the address bar on the resume page

>The data must be entered once and then saved to a file: Setting->Save

## Features
+ **Background Mode:** The application can run in the background, silently updating your resume without interfering with your other tasks.
+ **Settings Management:** The application provides the option to save and load your configuration settings using JSON files. This feature allows you to persistently store your credentials and update preferences, so you don't need to re-enter them each time you start the application.
+ **Graphical User Interface (GUI):** The user-friendly GUI allows easy navigation and interaction with the application.
+ **Automated Updates:** Enable the auto-update feature to keep your resume current without manual intervention.

## Example
<p align="center">
  <img src="https://github.com/imitatehappiness/QtHeadHunterBot/assets/79199956/e5e8af35-b7ba-4804-b887-b642516aa0b0" />
</p>

