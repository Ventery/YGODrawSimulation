#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H
#include <QEvent>
#include <QFrame>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>

#include "YGODataManager.h"

class SearchWidget : public QFrame
{
    Q_OBJECT

public:
    SearchWidget() ;

    void SearchKeyWord(QString key)
    {
        lineEdit_->setText(key);
        HandleSearch();
    }

public slots:
    void HandleSearch();

signals:
    void SendCardInfo(QString cardCode);


protected :
    bool eventFilter(QObject *target, QEvent *event)
    {
        if (event->type() == QKeyEvent::MouseButtonPress)
        {
            QMouseEvent *eventPtr = dynamic_cast<QMouseEvent*>(event);
            YGOCardPushButton * btnPtr = dynamic_cast<YGOCardPushButton*>(target);
            if (eventPtr->button() == Qt::LeftButton)
            {
                if (btnPtr != nullptr)
                {
                    HandleShow(btnPtr);
                    return true;
                }
            }
            else if (eventPtr->button() == Qt::RightButton)
            {
                if (btnPtr != nullptr)
                {
                    emit SendCardInfo(btnPtr->GetCardCode());
                    return true;
                }
            }
        }
        return QFrame::eventFilter(target, event);
    }

private:
    void HandleShow(const YGOCardPushButton *);

    QFrame *topFrame_;
    QLabel *midLabel_;

    QWidget *bottomWidget_;
    QScrollArea *bottomWidgetScroll_;

    QVBoxLayout *layout_;
    QHBoxLayout *topHBoxLayout_;
    QGridLayout *bottomGridLayout_;

    QLineEdit *lineEdit_;
    QPushButton *searchButton_;

    QTextEdit *textEdit_;
};

#endif // SEARCHWIDGET_H
