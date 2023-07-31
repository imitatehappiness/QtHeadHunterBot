#include "popup.h"
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

PopUp::PopUp(QWidget *parent) : QWidget(parent){
    setWindowFlags(Qt::FramelessWindowHint |        /// Отключаем оформление окна
                   Qt::Tool |                       /// Отменяем показ в качестве отдельного окна
                   Qt::WindowStaysOnTopHint);       /// Устанавливаем поверх всех окон
    setAttribute(Qt::WA_TranslucentBackground);     /// Указываем, что фон будет прозрачным
    setAttribute(Qt::WA_ShowWithoutActivating);     /// При показе, виджет не получается фокуса автоматически

    mAnimation.setTargetObject(this);
    mAnimation.setPropertyName("popupOpacity");
    connect(&mAnimation, &QAbstractAnimation::finished, this, &PopUp::hide);

    mLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mLabel.setStyleSheet("QLabel { color : white; "
                        "margin-top: 6px;"
                        "margin-bottom: 6px;"
                        "margin-left: 10px;"
                        "margin-right: 10px; }");

    mLayout.addWidget(&mLabel, 0, 0);
    setLayout(&mLayout);

    mTimer = new QTimer();
    connect(mTimer, &QTimer::timeout, this, &PopUp::hideAnimation);
}

void PopUp::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);

    painter.setBrush(QBrush(QColor(0,0,0,180)));
    painter.setPen(Qt::NoPen); // Край уведомления не будет выделен

    painter.drawRoundedRect(roundedRect, 10, 10);
}

void PopUp::setPopupText(const QString &text){
    mLabel.setText(text);
    adjustSize();
}

void PopUp::show(){
    setWindowOpacity(0.0);
    mAnimation.setDuration(150);
    mAnimation.setStartValue(0.0);
    mAnimation.setEndValue(1.0);

    setGeometry(QApplication::desktop()->availableGeometry().width() - 36 - width() + QApplication::desktop() -> availableGeometry().x(),
                QApplication::desktop()->availableGeometry().height() - 36 - height() + QApplication::desktop() -> availableGeometry().y(),
                width(),
                height());
    QWidget::show();
    mAnimation.start();
    mTimer->start(3000);
}

void PopUp::hideAnimation(){
    mTimer->stop();
    mAnimation.setDuration(1000);
    mAnimation.setStartValue(1.0);
    mAnimation.setEndValue(0.0);
    mAnimation.start();
}

void PopUp::hide(){
    if(getPopupOpacity() == 0.0){
        QWidget::hide();
    }
}

void PopUp::setPopupOpacity(float opacity){
    mPopupOpacity = opacity;
    setWindowOpacity(mPopupOpacity);
}

float PopUp::getPopupOpacity() const{
    return mPopupOpacity;
}

