#ifndef YGODRAWSIMULATION_H
#define YGODRAWSIMULATION_H

#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTabWidget>
#include <QTextEdit>
#include <QThread>
#include <QVBoxLayout>

//#include "Common.h"
#include "DeckWidget.h"
#include "YGOCardPushButton.h"
#include "YGODataManager.h"
#include "SearchWidget.h"

class YGODrawSimulation : public QWidget
{
    Q_OBJECT

public:

    YGODrawSimulation(QWidget *parent = nullptr);
    ~YGODrawSimulation();

public slots:
    void HandleImportDeck();
    void HandleSaveDeck();
    void HandleResetDeck();
    void HandleNewDeck();
    void HandleTabChange(int);
    void HandleTryDraw();
    void HandleNewEditDeck();
    void HandleCal();
    void HandleDelEditDeck();

protected :    
    bool eventFilter(QObject *target, QEvent *event)
    {
        if (event->type() == QKeyEvent::MouseButtonPress)
        {
            QMouseEvent *eventPtr = dynamic_cast<QMouseEvent*>(event);
            DeckWidget * newTarget = dynamic_cast<DeckWidget*>(target);
            if (newTarget == nullptr )
            {
                YGOCardPushButton * btnPtr = dynamic_cast<YGOCardPushButton*>(target);
                if (btnPtr != nullptr)
                {
                    newTarget = dynamic_cast<DeckWidget*>(btnPtr->parent());
                }
            }

            if (newTarget != nullptr)
            {
                if (tabWidget_->currentIndex() == 0)
                {
                    if (eventPtr->button() == Qt::LeftButton)
                    {
                        if (newTarget != currentWidget_)
                        {
                            rightFrame_->disconnect();
                            connect(rightFrame_, SIGNAL(SendCardInfo(QString)) ,newTarget ,SLOT(AddCard(QString)));
                            currentWidget_->setStyleSheet(QString::fromUtf8("border:1px solid black"));
                            newTarget->setStyleSheet(QString::fromUtf8("border:3px solid green"));
                            currentWidget_ = newTarget;
                            return true;
                        } else
                        {
                            YGOCardPushButton * btnPtr = dynamic_cast<YGOCardPushButton*>(target);
                            if (btnPtr != nullptr)
                            {
                                rightFrame_->SearchKeyWord(btnPtr->GetTrueCardCode());
                                return true;
                            }
                        }
                    }
                    else if (eventPtr->button() == Qt::RightButton )
                    {
                        if (newTarget == currentWidget_)
                        {
                            return newTarget->eventFilter(target,event);
                        }
                    }
                } else
                {
                    YGOCardPushButton * btnPtr = dynamic_cast<YGOCardPushButton*>(target);
                    if (eventPtr->button() == Qt::LeftButton)
                    {
                        if (btnPtr != nullptr)
                        {
                            rightFrame_->SearchKeyWord(btnPtr->GetCardCode());
                            return true;
                        } else if (newTarget->GetLocationInfo() == LocationInfo::Tab2EditDeck)
                        {
                            tab2MainDeck_->disconnect();
                            connect(tab2MainDeck_, SIGNAL(SendCardInfo(QString)) ,newTarget ,SLOT(AddCard(QString)));
                            currentEditWidget_ = newTarget;
                            DisplayTab2EditDeck();
                            return true;
                        }

                    } else if (eventPtr->button() == Qt::RightButton )
                    {
                        if ((newTarget->location_ == LocationInfo::Tab2MainDeck || (newTarget->location_ == LocationInfo::Tab2EditDeck && currentEditWidget_ ==newTarget) ) &&(btnPtr != nullptr))
                            return newTarget->eventFilter(target,event);
                    }
                }
            }
        }

         return QWidget::eventFilter(target, event);
    }

    void closeEvent(QCloseEvent *e){
        if (DataMgrIns.IsDeckChanged())
        {
            QMessageBox msgBox;
            msgBox.setText("提示");
            msgBox.setInformativeText("保存修改?");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int ret = msgBox.exec();
            if(ret == QMessageBox::Ok)
                HandleSaveDeck();
        }
         e->accept();
    }
private:

    void InitUI();
    void ConnectSlot();
    void LoadHistory();
    void InitCardData();
    void InitMain();
    void InitTab();
    void InitEventFilter();
    void DisplayDeck();
    void DisplayTab2EditDeck();
    void DeleteEditDeck();
    void ClearEditDeck();
    void RefreshLabel();
    void RefreshDeckFrame(QGridLayout * , const set<YGOCardPushButton *, CompareFunc> &);
    void HandleShow(const YGOCardPushButton *);

    QHBoxLayout *mainLayout_;
    QTabWidget *tabWidget_;

    ////////Tab1
    QFrame *tab1Frame_;
    QHBoxLayout *tab1HBoxLayout_;

    QFrame *tab1LeftFrame_;
    QVBoxLayout *tab1LeftVboxLayout_;

    QPushButton *importDeckButton_;
    QPushButton *saveDeckButton_;
    QPushButton *resetDeckButton_;
    QPushButton *newDeckButton_;
    QPushButton *button5_;
    QPushButton *button6_;
    QSpacerItem *buttonSpacer_;

    QFrame *tab1RightFrame_;
    QVBoxLayout *tab1RightVboxLayout_;
    QLabel *deckInfoLabel_;

    DeckWidget *tab1RightMainDeckWidget_;
    QGridLayout *mainDeckGridLayout_;

    DeckWidget *tab1RightExDeckWidget_;
    QGridLayout *exDeckGridLayout_;

    DeckWidget *tab1RightSideDeckWidget_;
    QGridLayout *sideDeckGridLayout_;

    //tab2
    QFrame *tab2Frame_;
    QHBoxLayout *tab2HBoxLayout_;

    DeckWidget * tab2MainDeck_;

    QFrame *tab2RightFrame_;
    QVBoxLayout *tab2RightVboxLayout_;

    QFrame *tab2RightTopFrame_;
    QVBoxLayout *tab2RightTopVboxLayout_;

    QFrame *tab2BtnFrame_;
    QGridLayout * tab2RightTopGridLayout_;
    QPushButton * tab2BtnAddDeck_;
    QPushButton * tab2BtnDelDeck_;
    QPushButton * tab2BtnCal5_;
    QPushButton * tab2BtnCal6_;
    QPushButton * tab2BtnTryDraw5_;
    QPushButton * tab2BtnTryDraw6_;

    QLineEdit * tab2LineEdit_;

    QFrame *tab2RightBottomFrame_;
    QVBoxLayout *tab2RightBottomVboxLayout_;

    vector<DeckWidget*> vecEditDeck_;

    ////////right frame

    class SearchWidget* rightFrame_;

    //Other
    bool isDeckReady_;

    DeckWidget* currentWidget_;
    DeckWidget* currentEditWidget_;

};
#endif // YGODRAWSIMULATION_H
