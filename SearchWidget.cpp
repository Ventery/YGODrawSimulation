#include <QDialog>

#include "SearchWidget.h"

SearchWidget::SearchWidget()
{
    layout_ = new QVBoxLayout();
    this->setLayout(layout_);

    topFrame_ = new QFrame();
    topHBoxLayout_ = new QHBoxLayout();
    topFrame_->setLayout(topHBoxLayout_);

    lineEdit_ = new QLineEdit();
    lineEdit_->setPlaceholderText("搜索卡片关键字/编号/描述");
    searchButton_ = new QPushButton("搜索");
    topHBoxLayout_->addWidget(lineEdit_);
    topHBoxLayout_->addWidget(searchButton_);

    midLabel_ = new QLabel();
    bottomWidget_ = new QWidget();
    bottomGridLayout_ = new QGridLayout();
    bottomWidget_->setLayout(bottomGridLayout_);

    lineEdit_->setText("电子龙");

    layout_->addWidget(topFrame_);
    layout_->addWidget(midLabel_);
    layout_->addWidget(bottomWidget_);

    bottomWidgetScroll_ = new QScrollArea();
    bottomWidgetScroll_->setWidget(bottomWidget_);
    bottomWidgetScroll_->setWidgetResizable(true);
    layout_->addWidget(bottomWidgetScroll_);

    layout_->setStretch(0,2);
    layout_->setStretch(1,1);
    layout_->setStretch(2,37);

    HandleSearch();
    connect(searchButton_,SIGNAL(clicked(bool)) , this,SLOT(HandleSearch()));
}

void SearchWidget::HandleSearch()
{
    QString indexContent = lineEdit_->text();
    if (indexContent.isEmpty())
        return ;

    map<QString, WidgetCache* > searchResults;
    int totalNum = DataMgrIns.GetCardInfoByKeyWord(lineEdit_->text(),searchResults);

    while (auto item = bottomGridLayout_->itemAt(0))
    {
        bottomGridLayout_->takeAt(0);
        item->widget()->setParent(nullptr);
        item->widget()->disconnect();
    }

    int index = 0;
    for (auto &cardInfo : searchResults)
    {
        bottomGridLayout_->addWidget(cardInfo.second->Button,index,0);
        bottomGridLayout_->addWidget(cardInfo.second->TextEdit,index++,1);
        cardInfo.second->Button->installEventFilter(this);
    }

    QString showResult = "共找到" + QString::number(totalNum) + "个结果，显示"+QString::number(searchResults.size()) +"条。" ;
    midLabel_->setText(showResult);
}

void SearchWidget::HandleShow(const YGOCardPushButton * btn)
{
    QDialog dialog;
    dialog.setWindowTitle("卡片原图");
    dialog.setStyleSheet("border-image:url("+ btn->GetPicPath()+")");
    QPixmap pixmap(btn->GetPicPath());

    dialog.setFixedHeight(pixmap.height()*1.4);
    dialog.setFixedWidth(pixmap.width()*1.4);
    dialog.exec();
}

