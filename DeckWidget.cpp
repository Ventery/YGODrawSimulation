#include <QFileInfo>
#include <QMessageBox>
#include <QSpacerItem>
#include "DeckWidget.h"

map<LocationInfo ,set<DeckWidget *>>DeckWidget::mapWidgetGroup_ = DeckWidget::CreatStaticMap();

DeckWidget::DeckWidget(CardSet * deckPtr , QWidget * eventFilterHub ,LocationInfo location ,CallBack &&callBack)
    :eventFilterHub_(eventFilterHub),
    layout_(new QGridLayout),
    deckPtr_(deckPtr),
    location_(location),
    deckChangeCallBack_(callBack)
{
    this->setLayout(layout_);
    if (location == LocationInfo::Tab2MainDeck)
        allyLocation_ = LocationInfo::Tab1MainDeck;
    else allyLocation_ = location;
    mapWidgetGroup_[allyLocation_].insert(this);
}

DeckWidget::~DeckWidget()
{
    mapWidgetGroup_[allyLocation_].erase(this);
}

map<LocationInfo ,set<DeckWidget *>> DeckWidget::CreatStaticMap()
{
    map<LocationInfo ,set<DeckWidget *>> tempMap;
    return tempMap;
}

void DeckWidget::Refresh()
{
    /*if (allyLocation_ != LocationInfo::Tab2EditDeck)
    {
        for (auto it : mapWidgetGroup_[allyLocation_])
            it->InnerRefresh();
    }else
        InnerRefresh();*/

    InnerRefresh();
    if (deckChangeCallBack_ != nullptr)
        deckChangeCallBack_();
}

void DeckWidget::Clear()
{
    while (auto item = layout_->itemAt(0))
    {
        layout_->takeAt(0);
        auto widget = item->widget();

        if (widget == nullptr)
            continue;
        widget->removeEventFilter(eventFilterHub_);
        widget->disconnect();
        widget->setParent(nullptr);
    }
}

void DeckWidget::AddCard(QString cardCode)
{
    if (DataMgrIns.GetCardDataByCode(cardCode) == nullptr) return;
    LocationInfo cardLocation = DataMgrIns.GetCardLocationByCode(cardCode);
    if ((cardLocation == LocationInfo::Tab1ExtraDeck && allyLocation_ == LocationInfo::Tab1MainDeck)||
            (cardLocation == LocationInfo::Tab1MainDeck && allyLocation_ == LocationInfo::Tab1ExtraDeck))
        return ;

    if ((allyLocation_ == LocationInfo::Tab1MainDeck && deckPtr_->size() > 64)||
          (allyLocation_ == LocationInfo::Tab1ExtraDeck && deckPtr_->size() > 19)||
          (allyLocation_ == LocationInfo::Tab1SideDeck && deckPtr_->size() > 19)||
          (location_ == LocationInfo::Tab2EditDeck && deckPtr_->size() > 6))
        return ;

    QString picPath = DataMgrIns.GetCardPicPathByCardCode(cardCode);
    QFileInfo file(picPath);
    if (!file.isFile())
    {
        picPath = DataMgrIns.defaultCardPic_;
    }

    YGOCardPushButton * btn = new YGOCardPushButton(picPath, cardCode);
    btn->setFixedHeight(DECK_CARD_HEIGHT);
    btn->setFixedWidth(DECK_CARD_WIDTH);
    QPixmap pixmap(picPath);
    QPixmap fitpixmap = pixmap.scaled(DECK_CARD_WIDTH, DECK_CARD_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    btn->setIcon(QIcon(fitpixmap));
    btn->setIconSize(QSize(500, 500));
    btn->setFlat(true);

    deckPtr_->insert(btn ,true);
    Refresh();
}

void DeckWidget::InnerRefresh()
{
    Clear();

    int col = 0;
    int row = 0;

    auto & targetSet = deckPtr_->GetInnerSet();

    int columnLimit = 0;

    if (location_ == LocationInfo::Tab2EditDeck)
        columnLimit = TAB2_EDIT_DECK_COLUMN;
    else if (location_ == LocationInfo::Tab2MainDeck)
        columnLimit = TAB2_DECK_COLUMN;
    else
        columnLimit = DECK_FRAME_COLUMN;

    for (auto ptr : targetSet)
    {
        layout_->addWidget(ptr ,row, col++);
        ptr->installEventFilter(eventFilterHub_);
        if (col == columnLimit)
        {
            col = 0;
            ++row;
        }
    }

    if (targetSet.size() < columnLimit)
    {
        QSpacerItem *spaceItem = new QSpacerItem(668 , 0);
        layout_->addItem(spaceItem,row,col);
    }
}


