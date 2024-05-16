#ifndef POPUP_H
#define POPUP_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

class PopUp : public QWidget {
    Q_OBJECT
    /**
    * @brief Свойство прозрачности всплывающего окна
    */
    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)
    /**
    * @brief Установка значения прозрачности
    */
    void setPopupOpacity(float opacity);
    /**
    * @brief Получение значения прозрачности
    */
    float getPopupOpacity() const;
public:
    /**
    * @brief Конструктор.
    *
    * @param parent Родительский виджет.
    */
    explicit PopUp(QWidget *parent = nullptr);
protected:
    /**
    * @brief Фон будет отрисован с использованием метода перерисовки
    */
    void paintEvent(QPaintEvent *event);
public slots:
    /**
    * @brief Установка текста в уведомлении
    */
    void setPopupText(const QString& text);
    /**
    * @brief Метод отображения виджета
    *
    * @details Необходим для предварительной настройки анимации
    */
    void show();
private slots:
    /**
    * @brief Запуск анимации скрытия
    */
    void hideAnimation();
    /**
    * @brief По завершении анимации проверяется видимость виджета и необходимость его скрытия
    */
    void hide();
private:
    QLabel mLabel;                      /// сообщение
    QGridLayout mLayout;                /// расположение для метки
    QPropertyAnimation mAnimation;      /// анимация для всплывающего сообщения
    float mPopupOpacity;                /// уровень прозрачности
    QTimer *mTimer;                     /// таймер, по которому будет скрыт виджет
};

#endif // POPUP_H
