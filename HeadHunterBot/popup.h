#ifndef POPUP_H
#define POPUP_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

class PopUp : public QWidget{
    Q_OBJECT
    /**
    * @brief Translucency property
    */
    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)
    /**
    * @brief Setting the transparency value
    */
    void setPopupOpacity(float opacity);
    /**
    * @brief Getting the transparency value
    */
    float getPopupOpacity() const;
public:
    /**
    * @brief Constructor.
    *
    * @param parent The parent widget.
    */
    explicit PopUp(QWidget *parent = 0);
protected:
    /**
    * @brief The background will be rendered using the redraw method
    */
    void paintEvent(QPaintEvent *event);
public slots:
    /**
    * @brief Setting the text in the notification
    */
    void setPopupText(const QString& text);
    /**
    * @brief Setting the text in the notificationthe proper method of displaying the widget
    *
    * @details Necessary for the preliminary adjustment of the animation
    */
    void show();
private slots:
    /**
    * @brief Starting the hide animation
    */
    void hideAnimation();
    /**
    * @brief At the end of the animation, a check is made in this slot, is the widget visible, or does it need to be hidden
    */
    void hide();
private:
    QLabel mLabel;                      /// message
    QGridLayout mLayout;                /// Placement for the label
    QPropertyAnimation mAnimation;      /// Animation property for pop-up message
    float mPopupOpacity;                /// Transparency level
    QTimer *mTimer;                     /// Timer by which the widget will be hidden
};

#endif // POPUP_H
