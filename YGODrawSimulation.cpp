#include <functional>

#include <QApplication>
#include <QFileDialog>
#include <QStandardPaths>

#include "YGODrawSimulation.h"
#include "YGODataManager.h"
#include "HistoryManager.h"

YGODrawSimulation::YGODrawSimulation(QWidget *parent)
    : QWidget(parent),isDeckReady_(false),currentEditWidget_(nullptr)
{
    InitCardData();
    InitUI();
    InitEventFilter();

    ConnectSlot();

    DisplayDeck();
    DisplayTab2EditDeck();
}

YGODrawSimulation::~YGODrawSimulation()
{
}

void YGODrawSimulation::InitCardData()
{
    if (!DataMgrIns.Init())
    {
        QMessageBox::information(this,"错误","数据库加载失败");
        abort();
    }

    isDeckReady_ = DataMgrIns.LoadDeck(HistoryMgrIns.GetHistory("LastOpenedPath")) >= 0;
}

void YGODrawSimulation::InitUI()
{
    InitMain();
    InitTab();
}

void YGODrawSimulation::InitMain()
{
    setWindowTitle("YGO抽卡模拟器");
    //setFixedSize(QSize( 1440, 810 ));
    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Policy::MinimumExpanding);
    policy.setVerticalPolicy(QSizePolicy::Policy::MinimumExpanding);
    setSizePolicy(policy);
    setMinimumSize(QSize( 1440, 950 ));

    //main
    mainLayout_ = new QHBoxLayout(this);

    tabWidget_ = new QTabWidget();

    rightFrame_ = new class SearchWidget();
    rightFrame_->setFrameShape(QFrame::Shape::StyledPanel );

    mainLayout_->addWidget(tabWidget_);
    mainLayout_->addWidget(rightFrame_);
    mainLayout_->setStretch(0,2);
    mainLayout_->setStretch(1,1);
}

void YGODrawSimulation::InitTab()
{
    auto deck = DataMgrIns.GetDeck();

    //tab1 frame
    tab1Frame_ = new QFrame();
    tab1HBoxLayout_ = new QHBoxLayout();
    tab1LeftFrame_ = new QFrame();
    tab1RightFrame_ = new QFrame();
    tab1RightFrame_->setFrameShape(QFrame::Shape::StyledPanel );
    tab1HBoxLayout_->addWidget(tab1LeftFrame_);
    tab1HBoxLayout_->addWidget(tab1RightFrame_);
    tab1HBoxLayout_->setStretch(0,1);
    tab1HBoxLayout_->setStretch(1,3);

    tab1Frame_->setLayout(tab1HBoxLayout_);

    tab1LeftVboxLayout_ = new QVBoxLayout();
    tab1LeftFrame_->setLayout(tab1LeftVboxLayout_);

    newDeckButton_ = new QPushButton("新建卡组");
    newDeckButton_->setMinimumHeight(50);

    importDeckButton_ = new QPushButton("导入卡组");
    importDeckButton_->setMinimumHeight(50);

    resetDeckButton_ = new QPushButton("重置卡组");
    resetDeckButton_->setMinimumHeight(50);

    saveDeckButton_ = new QPushButton("保存卡组");
    saveDeckButton_->setMinimumHeight(50);

    /*button5_ = new QPushButton("按钮5");
    button5_->setMinimumHeight(50);

    button6_ = new QPushButton("按钮6");
    button6_->setMinimumHeight(50);*/

    buttonSpacer_ = new QSpacerItem(0,1000);

    tab1LeftVboxLayout_->addWidget(newDeckButton_);
    tab1LeftVboxLayout_->addWidget(importDeckButton_);
    tab1LeftVboxLayout_->addWidget(resetDeckButton_);
    tab1LeftVboxLayout_->addWidget(saveDeckButton_);
    tab1LeftVboxLayout_->addItem(buttonSpacer_);
    //tab1LeftVboxLayout_->addWidget(button5_);
    //tab1LeftVboxLayout_->addWidget(button6_);

    tab1RightVboxLayout_ = new QVBoxLayout();
    tab1RightFrame_->setLayout(tab1RightVboxLayout_);

    deckInfoLabel_ = new QLabel("");

    tab1RightMainDeckWidget_ = new DeckWidget(&(deck->setMainCards_),this ,LocationInfo::Tab1MainDeck ,std::bind(&YGODrawSimulation::RefreshLabel,this));
    tab1RightExDeckWidget_ = new DeckWidget(&(deck->setExtraCards_),this ,LocationInfo::Tab1ExtraDeck ,std::bind(&YGODrawSimulation::RefreshLabel,this));
    tab1RightSideDeckWidget_ = new DeckWidget(&(deck->setSideCards_),this ,LocationInfo::Tab1SideDeck ,std::bind(&YGODrawSimulation::RefreshLabel,this));

    deckInfoLabel_->setMaximumHeight(20);
    tab1RightMainDeckWidget_ ->setMinimumHeight(380);
    tab1RightExDeckWidget_ ->setMinimumHeight(155);
    tab1RightExDeckWidget_ ->setMaximumHeight(200);
    tab1RightSideDeckWidget_ ->setMinimumHeight(155);
    tab1RightSideDeckWidget_ ->setMaximumHeight(200);

    tab1RightMainDeckWidget_->setFixedWidth(668);
    tab1RightExDeckWidget_->setFixedWidth(668);
    tab1RightSideDeckWidget_->setFixedWidth(668);


    tab1RightMainDeckWidget_->setStyleSheet(QString::fromUtf8("border:3px solid green"));
    tab1RightExDeckWidget_->setStyleSheet(QString::fromUtf8("border:1px solid black"));
    tab1RightSideDeckWidget_->setStyleSheet(QString::fromUtf8("border:1px solid black"));

    tab1RightVboxLayout_->addWidget(deckInfoLabel_);
    tab1RightVboxLayout_->addWidget(tab1RightMainDeckWidget_);
    tab1RightVboxLayout_->addWidget(tab1RightExDeckWidget_);
    tab1RightVboxLayout_->addWidget(tab1RightSideDeckWidget_);

    tab1RightVboxLayout_->setStretch(0,1);

    ////tab2 frame
    tab2Frame_ = new QFrame();

    tab2HBoxLayout_ = new QHBoxLayout();
    tab2HBoxLayout_->setStretch(0,1);
    tab2HBoxLayout_->setStretch(1,1);
    tab2MainDeck_ = new DeckWidget(&(deck->setMainCards_),this ,LocationInfo::Tab2MainDeck ,std::bind(&YGODrawSimulation::RefreshLabel,this));
    tab2MainDeck_->setFrameShape(QFrame::Shape::StyledPanel );

    tab2RightFrame_ = new QFrame();
    tab2RightVboxLayout_ = new QVBoxLayout();
    tab2RightVboxLayout_->setStretch(0,1);
    tab2RightVboxLayout_->setStretch(1,10);

    tab2RightTopFrame_ = new QFrame();
    tab2RightTopVboxLayout_ = new QVBoxLayout();
    tab2RightTopFrame_->setLayout(tab2RightTopVboxLayout_);

    tab2BtnFrame_ = new QFrame();
    tab2RightTopGridLayout_ = new QGridLayout();
    tab2BtnFrame_->setLayout(tab2RightTopGridLayout_);
    tab2BtnAddDeck_ = new QPushButton("增加候选组合");
    tab2BtnDelDeck_ = new QPushButton("删除候选组合");
    tab2BtnCal5_ = new QPushButton("计算起手5张概率");
    tab2BtnCal6_ = new QPushButton("计算起手6张概率");
    tab2BtnTryDraw5_ = new QPushButton("试抽5张");
    tab2BtnTryDraw6_ = new QPushButton("试抽6张");
    tab2RightTopGridLayout_->addWidget(tab2BtnAddDeck_ ,0 ,0);
    tab2RightTopGridLayout_->addWidget(tab2BtnDelDeck_ ,0 ,1);
    tab2RightTopGridLayout_->addWidget(tab2BtnCal5_ ,1 ,0);
    tab2RightTopGridLayout_->addWidget(tab2BtnCal6_ ,1 ,1);
    tab2RightTopGridLayout_->addWidget(tab2BtnTryDraw5_ ,2 ,0);
    tab2RightTopGridLayout_->addWidget(tab2BtnTryDraw6_ ,2 ,1);


    tab2LineEdit_ = new QLineEdit();
    tab2LineEdit_->setReadOnly(true);

    tab2RightTopVboxLayout_->addWidget(tab2LineEdit_);
    tab2RightTopVboxLayout_->addWidget(tab2BtnFrame_);

    tab2RightTopFrame_->setFrameShape(QFrame::Shape::StyledPanel );

    tab2RightBottomFrame_ = new QFrame();
    tab2RightBottomVboxLayout_ = new QVBoxLayout();
    tab2RightBottomFrame_->setLayout(tab2RightBottomVboxLayout_);
    tab2RightBottomFrame_->setFrameShape(QFrame::Shape::StyledPanel );

    tab2RightFrame_->setLayout(tab2RightVboxLayout_);
    tab2RightVboxLayout_->addWidget(tab2RightTopFrame_);
    tab2RightVboxLayout_->addWidget(tab2RightBottomFrame_);
    tab2RightVboxLayout_->setStretch(0,1);
    tab2RightVboxLayout_->setStretch(1,20);

    tab2HBoxLayout_->addWidget(tab2MainDeck_);
    tab2HBoxLayout_->addWidget(tab2RightFrame_);

    tab2Frame_->setLayout(tab2HBoxLayout_);

    tabWidget_->addTab(tab1Frame_ ,"卡组编辑");
    tabWidget_->addTab(tab2Frame_ ,"抽卡模拟");

    currentWidget_ = tab1RightMainDeckWidget_;
}

void YGODrawSimulation::InitEventFilter()
{
    tab1RightMainDeckWidget_->installEventFilter(this);
    tab1RightExDeckWidget_->installEventFilter(this);
    tab1RightSideDeckWidget_->installEventFilter(this);
}

void YGODrawSimulation::ConnectSlot()
{
    connect(rightFrame_, SIGNAL(SendCardInfo(QString)) ,currentWidget_ ,SLOT(AddCard(QString)));
    connect(importDeckButton_,SIGNAL(clicked(bool)),this, SLOT(HandleImportDeck()));
    connect(saveDeckButton_,SIGNAL(clicked(bool)),this, SLOT(HandleSaveDeck()));
    connect(resetDeckButton_,SIGNAL(clicked(bool)),this, SLOT(HandleResetDeck()));
    connect(newDeckButton_,SIGNAL(clicked(bool)),this, SLOT(HandleNewDeck()));
    connect(tabWidget_,SIGNAL(currentChanged(int)),this,SLOT(HandleTabChange(int)));
    connect(tab2BtnTryDraw5_,SIGNAL(clicked(bool)),this,SLOT(HandleTryDraw()));
    connect(tab2BtnTryDraw6_,SIGNAL(clicked(bool)),this,SLOT(HandleTryDraw()));
    connect(tab2BtnAddDeck_,SIGNAL(clicked(bool)),this,SLOT(HandleNewEditDeck()));
    connect(tab2BtnCal5_ ,SIGNAL(clicked(bool)),this,SLOT(HandleCal()));
    connect(tab2BtnCal6_ ,SIGNAL(clicked(bool)),this,SLOT(HandleCal()));
    connect(tab2BtnDelDeck_ ,SIGNAL(clicked(bool)),this,SLOT(HandleDelEditDeck()));
}

void YGODrawSimulation::HandleDelEditDeck()
{
    if (currentEditWidget_ == nullptr)
        return;

    for (int i = 0; i < vecEditDeck_.size() ;++ i)
        if (vecEditDeck_[i] == currentEditWidget_)
        {
            DataMgrIns.DeleteEditDeck(i);
            delete vecEditDeck_[i];
            vecEditDeck_.erase(vecEditDeck_.begin()+i);
            break;
        }

    if (vecEditDeck_.size() > 0)
        currentEditWidget_ = vecEditDeck_[0];

    DisplayTab2EditDeck();
}


void YGODrawSimulation::HandleNewEditDeck()
{
    auto *newDeck = DataMgrIns.GetNewDeck();
    DeckWidget * newWidget = new  DeckWidget(newDeck ,this ,LocationInfo::Tab2EditDeck ,nullptr);
    newWidget->setFixedWidth(469);
    newWidget->setFixedHeight(100);
    newWidget->installEventFilter(this);
    vecEditDeck_.push_back(newWidget);
    tab2MainDeck_->disconnect();
    connect(tab2MainDeck_, SIGNAL(SendCardInfo(QString)) ,newWidget ,SLOT(AddCard(QString)));
    currentEditWidget_ = newWidget;
    DisplayTab2EditDeck();
}

void YGODrawSimulation::HandleCal()
{
    QPushButton *senderObj = (QPushButton *)sender();
    int drawNum =senderObj == tab2BtnCal5_?5:6;
    tab2LineEdit_->setText(DataMgrIns.CalRate(drawNum));
}

void YGODrawSimulation::HandleTabChange(int index)
{
    if (index == 0)
    {
        disconnect(rightFrame_, SIGNAL(SendCardInfo(QString)) ,tab2MainDeck_ ,SLOT(AddCard(QString)));
        connect(rightFrame_, SIGNAL(SendCardInfo(QString)) ,currentWidget_ ,SLOT(AddCard(QString)));
        tab2MainDeck_->Clear();
        tab1RightMainDeckWidget_->Refresh();
    }
    else if (index == 1)
    {
        disconnect(rightFrame_, SIGNAL(SendCardInfo(QString)) ,currentWidget_ ,SLOT(AddCard(QString)));
        connect(rightFrame_, SIGNAL(SendCardInfo(QString)) ,tab2MainDeck_ ,SLOT(AddCard(QString)));
        tab1RightMainDeckWidget_->Clear();
        tab2MainDeck_->Refresh();
    }
}

void YGODrawSimulation::HandleTryDraw()
{
    DeleteEditDeck();

    auto *deckPtr = tab2MainDeck_->deckPtr_;
    QPushButton *senderObj = (QPushButton *)sender();
    int drawNum =senderObj == tab2BtnTryDraw5_?5:6;
    auto *newDeck = DataMgrIns.GetRandomDeck(deckPtr ,drawNum);
    DeckWidget * newWidget = new  DeckWidget(newDeck ,this ,LocationInfo::Tab2EditDeck ,nullptr);
    newWidget->setFixedWidth(469);
    newWidget->setFixedHeight(100);
    newWidget->installEventFilter(this);
    connect(tab2MainDeck_, SIGNAL(SendCardInfo(QString)) ,newWidget ,SLOT(AddCard(QString)));
    currentEditWidget_ = newWidget;
    vecEditDeck_.push_back(newWidget);
    DisplayTab2EditDeck();
}

void YGODrawSimulation::HandleImportDeck()
{
    if (DataMgrIns.IsDeckChanged())
    {
        QMessageBox::warning(this, "通知", "先前的修改请先保存或重置卡组");
        return;
    }

    QString defaultPath = HistoryMgrIns.GetHistory("LastOpenedPath");
    if (defaultPath.compare("") == 0)
        defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    QString fileName = QFileDialog::getOpenFileName(
        this,
        QString("打开一个ygo卡组文件"),
        defaultPath,
        QString("ydk卡组文件(*.ydk);;All files(*.*)"));
    if (fileName.isEmpty()) {
        return ;
    }

    HistoryMgrIns.SetHistory("LastOpenedPath",fileName);

    int ret = DataMgrIns.LoadDeck(fileName);
    if (ret < 0)
    {
        QMessageBox::warning(this, "通知", "卡组解析失败");
        return ;
    } else if (ret == 0)
    {
        QMessageBox::warning(this, "通知", "卡组张数不规范（主卡组40~60张，额外卡组、副卡组0~15张）");
    }
    isDeckReady_ = true;

    DisplayDeck();
}

void YGODrawSimulation::HandleSaveDeck()
{
    int ret = DataMgrIns.SaveDeck();
    if (ret < 0)
        QMessageBox::warning(this, "通知", "卡组张数不规范（主卡组40~60张，额外卡组、副卡组0~15张）");
    else if (ret == 0)
        QMessageBox::warning(this, "通知", "文件路径不存在");
    else
    {
        QMessageBox::warning(this, "通知", "保存成功");
        setWindowTitle("YGO抽卡模拟器");
    }
}

void YGODrawSimulation::HandleResetDeck()
{
    QString fileName;
    fileName = HistoryMgrIns.GetHistory("LastOpenedPath");

    int ret = DataMgrIns.LoadDeck(fileName);
    if (ret < 0)
    {
        QMessageBox::warning(this, "通知", "卡组解析失败");
        return ;
    } else if (ret == 0)
    {
        QMessageBox::warning(this, "通知", "卡组张数不规范（主卡组40~60张，额外卡组、副卡组0~15张）");
    }
    isDeckReady_ = true;

    DisplayDeck();
}


void YGODrawSimulation::HandleNewDeck()
{
    if (DataMgrIns.IsDeckChanged())
    {
        QMessageBox::warning(this, "通知", "先前的修改请先保存或复原");
        return;
    }

    QString defaultPath = HistoryMgrIns.GetHistory("LastOpenedPath");
    if (defaultPath.compare("") == 0)
        defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    else
    {
        int lastSymbolIndex = defaultPath.lastIndexOf("/");
        defaultPath = defaultPath.mid(0 ,lastSymbolIndex + 1);
    }

    QString fileName = QFileDialog::getSaveFileName(
        this,
        QString("新建一个ygo卡组文件"),
        defaultPath,
        QString("ydk卡组文件(*.ydk);;All files(*.*)"));
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "通知", "文件名称不存在");
        return ;
    }

    QFile file(fileName);
    if ( file.exists())
    {
        QMessageBox::warning(this, "通知", "文件已存在");
        return;
    }

    file.open( QIODevice::WriteOnly );
    file.write(" ");
    file.close();
    HistoryMgrIns.SetHistory("LastOpenedPath",fileName);

    DataMgrIns.LoadDeck(fileName);
    DisplayDeck();
}

void YGODrawSimulation::DisplayDeck()
{
    //if (!isDeckReady_) return ;

    RefreshLabel();
    tab1RightMainDeckWidget_->Refresh();
    tab1RightExDeckWidget_->Refresh();
    tab1RightSideDeckWidget_->Refresh();
}

void YGODrawSimulation::ClearEditDeck()
{
    while (auto item = tab2RightBottomVboxLayout_->itemAt(0))
    {
        tab2RightBottomVboxLayout_->takeAt(0);
        auto widget = item->widget();

        if (widget == nullptr)
            continue;
        widget->disconnect();
        widget->setParent(nullptr);
    }
}

void YGODrawSimulation::DeleteEditDeck()
{
    DataMgrIns.DeleteAllEditDeck();

    for (int i = 0; i < vecEditDeck_.size();++i)
        delete vecEditDeck_[i];
    vecEditDeck_.clear();
}


void YGODrawSimulation::DisplayTab2EditDeck()
{
    ClearEditDeck();

    for (int i = 0 ;i < vecEditDeck_.size();++i)
    {
        tab2RightBottomVboxLayout_->addWidget(vecEditDeck_[i]);
        vecEditDeck_[i]->Refresh();

        if (currentEditWidget_ == vecEditDeck_[i])
            vecEditDeck_[i]->setStyleSheet(QString::fromUtf8("border:3px solid green"));
        else
            vecEditDeck_[i]->setStyleSheet(QString::fromUtf8("border:1px solid black"));
    }

    if (vecEditDeck_.size() < 7)
        tab2RightBottomVboxLayout_->addSpacerItem(new QSpacerItem(0,(7 - vecEditDeck_.size()) * 105));

}

void YGODrawSimulation::RefreshLabel()
{
    QString defaultPath = HistoryMgrIns.GetHistory("LastOpenedPath");
    int lastSymbolIndex = defaultPath.lastIndexOf("/");
    QString fileName = defaultPath.mid(lastSymbolIndex + 1,defaultPath.size()-lastSymbolIndex - 5);

    auto deck = DataMgrIns.GetDeck();
    deckInfoLabel_->setText("卡组 : "+fileName+" "+"主卡组 "+QString::number(deck->setMainCards_.size())+" 张，"+
                            "额外卡组 "+QString::number(deck->setExtraCards_.size())+" 张，"+
                            "副卡组 "+QString::number(deck->setSideCards_.size())+" 张，");

    if (DataMgrIns.IsDeckChanged())
        setWindowTitle("YGO抽卡模拟器*");
    else
        setWindowTitle("YGO抽卡模拟器");
}

void YGODrawSimulation::HandleShow(const YGOCardPushButton * btn)
{
    QDialog dialog;
    dialog.setWindowTitle("卡片原图");
    dialog.setStyleSheet("border-image:url("+ btn->GetPicPath()+")");
    QPixmap pixmap(btn->GetPicPath());

    dialog.setFixedHeight(pixmap.height()*1.4);
    dialog.setFixedWidth(pixmap.width()*1.4);
    dialog.exec();
}
