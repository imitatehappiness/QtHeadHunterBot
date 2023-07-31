# AutoHeadHunterUpdate

## Description
AutoHeadHunterUpdate is a C++ QT application with a graphical user interface designed to facilitate the automatic and manual updating of your resume on the HeadHunter job portal. The application requires the following fields to function properly:

+ **id resume:** The unique identifier of your resume on HeadHunter.
+ **hhtoken:** HeadHunter API token required for authentication.
+ **hhuid:** HeadHunter user ID associated with your account.
+ **xsrf:** Cross-Site Request Forgery token for secure communication with the HeadHunter website.

The application offers two main modes of updating:

+ **One-Time Update:** Allows you to manually update your resume on the HeadHunter website instantly.
+ **Auto-Update:** Automatically updates your resume every 4 hours to ensure that your information is always up-to-date.

## Features
+ **Background Mode:** The application can run in the background, silently updating your resume without interfering with your other tasks.
+ **Settings Management:** The application saves your configuration settings, so you don't need to re-enter your credentials each time you start it.
+ **Graphical User Interface (GUI):** The user-friendly GUI allows easy navigation and interaction with the application.
+ **Automated Updates:** Enable the auto-update feature to keep your resume current without manual intervention.

## Usage
1. Launch the AutoHeadHunterUpdate application.
2. In the settings section, provide your id resume, hhtoken, hhuid, and xsrf credentials.
3. Choose the desired update mode: one-time update or auto-update.
4. If you opt for auto-update, set the update interval to 4 hours.
5. Click on the "Save Settings" button to store your credentials for future use.
6. If you enabled auto-update, the application will silently update your resume every 4 hours.
7. To perform a manual one-time update, click on the "Update Now" button.
8. The application will display status messages indicating the success or failure of the update process.

## Example