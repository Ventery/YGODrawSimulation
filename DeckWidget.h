#ifndef DECKWIDGET_H
#define DECKWIDGET_H

#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>
#include <QWidget>

#include "YGOCardPushButton.h"
#include "YGODataManager.h"
#include "Common.h"

class DeckWidget : public QFrame
{
    Q_OBJECT

public:
    DeckWidget(CardSet * deckPtr , QWidget * eventFilterHub ,LocationInfo location ,CallBack &&callBack);   //用于跟datamanager中的数据绑定
    ~DeckWidget();
    void Refresh();
    void Clear();
    LocationInfo GetLocationInfo(){return location_;}
    const QWidget *GetEventFilterHub(){return eventFilterHub_;}
    friend class YGODrawSimulation;

signals:
    void SendCardInfo(QString cardCode);

public slots:
    void AddCard(QString cardCode);

protected :
    bool eventFilter(QObject *target, QEvent *event)
    {
        if (event->type() == QKeyEvent::MouseButtonPress)
        {
            QMouseEvent *eventPtr = dynamic_cast<QMouseEvent*>(event);
            if (eventPtr->button() == Qt::RightButton)
            {
                YGOCardPushButton * btnPtr = dynamic_cast<YGOCardPushButton*>(target);
                if (btnPtr != nullptr)
                {
                    if (location_ <= LocationInfo::Tab1SideDeck)
                    {
                        deckPtr_->erase(btnPtr ,true);
                        delete btnPtr;
                        Refresh();
                        return true;
                    } else if (location_ == LocationInfo::Tab2MainDeck)
                    {
                        emit SendCardInfo(btnPtr->GetTrueCardCode());
                        return true;
                    }else if (location_ == LocationInfo::Tab2EditDeck)
                    {
                        deckPtr_->erase(btnPtr ,true);
                        delete btnPtr;
                        Refresh();
                        return true;
                    }
                }
            }
        }
        return QWidget::eventFilter(target, event);
    }

private:
    void InnerRefresh();
    static map<LocationInfo ,set<DeckWidget *>> CreatStaticMap();

    QWidget * eventFilterHub_;
    QGridLayout * layout_;
    CardSet * deckPtr_;
    LocationInfo location_;
    LocationInfo allyLocation_;
    CallBack deckChangeCallBack_;
    static map<LocationInfo ,set<DeckWidget *>> mapWidgetGroup_;
};

#endif // DECKWIDGET_H
