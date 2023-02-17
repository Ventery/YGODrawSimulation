#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include "YGODataManager.h"
#include "HistoryManager.h"

unsigned RandomSeed = std::chrono::system_clock::now().time_since_epoch().count();
auto RandomDevice = std::default_random_engine(RandomSeed);

YGODataManager::YGODataManager():isInited_(false),isDBInited_(false),isPicInited_(false),isChanged_(false)
{
    defaultCardPic_ = QDir::currentPath() + BUILD_PREFIX + "/ygoData/DefaultSide.jpg";
    picPathPrefix_ = QDir::currentPath() + BUILD_PREFIX + "/ygoData/picture/card/";

    deck_ = new DeckWidgetCache(isChanged_);
}

YGODataManager::~YGODataManager()
{

}

bool YGODataManager::Init()
{
    if (isInited_) return true;
    if (!isDBInited_) isDBInited_ = InitDB();
    if (!isPicInited_) isPicInited_ = InitPicIndex();
    isInited_ = isDBInited_ && isPicInited_;
    return isInited_;
}

bool YGODataManager::InitDB()
{
    QString path = QDir::currentPath() + BUILD_PREFIX + "/ygoData/cdb/cards.cdb";
    CSqliteOperator operatorr;


    QFileInfo file(path);
    if (file.isFile())
    {
        int result = operatorr.CreateSqlFile(path.toStdString());

        if (result != SQLITE_OK)
        {
            return false;
        }
        else
        {
            return LoadCardDatas(operatorr);
        }
    } else {
        return false;
    }

    return true;
}

bool YGODataManager::InitPicIndex()
{
    return true;
}

bool YGODataManager::LoadCardDatas(CSqliteOperator &operatorr)
{
    string strSql = "select * from datas,texts where datas.id=texts.id";
    vector<string> vecKey;
    vector<vector<string>> vecValue;

    int result = operatorr.FindAllData(strSql, vecKey, vecValue);
    mapCardDatas_.clear();

    if (result == SQLITE_OK && !vecKey.empty() && !vecValue.empty())
    {
        for (int i = 0; i < vecValue.size();++i)
        {
            auto &fileds = vecValue[i];
            auto primaryKey = QString::fromStdString(fileds[0]);
            if (mapCardDatas_.find(primaryKey) != mapCardDatas_.end())
                return false;
            auto &newCard = mapCardDatas_[primaryKey];

            newCard.Code = primaryKey;
            newCard.Ot = stoul(fileds[1]);
            newCard.Alias = QString::fromStdString(fileds[2]);
            newCard.Setcode = stoull(fileds[3]);
            newCard.Type = stoul(fileds[4]);
            newCard.Attack = stoul(fileds[5]);
            if (newCard.Type & TYPE_LINK)
            {
                newCard.LinkMarker = stoul(fileds[6]);
                newCard.Defense = 0;
            }else
            {
                newCard.Defense = stoul(fileds[6]);
                newCard.LinkMarker = 0;
            }

            unsigned int level = stoul(fileds[7]);
            newCard.Level = level & 0xff;
            newCard.Lscale = ( level >> 24 ) & 0xff;
            newCard.Rscale = ( level >> 16 ) & 0xff;

            newCard.Race = stoul(fileds[8]);
            newCard.Attribute = stoul(fileds[9]);
            newCard.Category = stoul(fileds[10]);
            newCard.Name = QString::fromStdString(fileds[12]);
            newCard.Text = QString::fromStdString(fileds[13]);

            for (int j = 0 ; j< 16; ++j)
                newCard.Desc[j] = QString::fromStdString(fileds[j + 14]);
        }
    }

    return true;
}

int YGODataManager::GetCardInfoByKeyWord(const QString key ,map<QString, WidgetCache* >& map)
{
    int count = 0;
    for (auto & card : mapCardDatas_)
    {
        if (card.first .compare( key ) == 0
                ||card.second.Alias.compare(key) == 0
                || card.second.Name.contains(key)
                || card.second.Text.contains((key)))
        {
            count++;
            if (count <= MAXNUM_DISPLAY_SEARCH_RESULT)
                map[card.first] = GetWidgetByCardData(card.second);
        }
    }
    return count;
}

QString YGODataManager::GetCardPicPathByCardCode(const QString & code)
{
    return picPathPrefix_ + code +".jpg";
}

WidgetCache* YGODataManager::GetWidgetByCardData(const CardData & card)
{
    if (mapWidgetCache_.find(card.Code) == mapWidgetCache_.end())
    {
        auto &newCache = mapWidgetCache_[card.Code];
        FillWidget(&newCache , card.Code ,GetCardPicPathByCardCode(card.Code),card.GetContent());
    }

    return &mapWidgetCache_[card.Code];
}

void YGODataManager::FillWidget(WidgetCache * widget, const QString code,const QString  picPath, const QString text)
{    
    QFileInfo file(picPath);
    if (!file.isFile())
    {
        if (defaultWidget_->Button == nullptr)
            FillWidget(defaultWidget_ ,"DefaultSide",defaultCardPic_, "默认卡背");

        widget->Button = defaultWidget_->Button;
        widget->TextEdit = defaultWidget_->TextEdit;
        return;
    }

    widget->Button = new YGOCardPushButton(picPath,code);
    YGOCardPushButton *ptn = widget->Button;
    ptn->setFixedHeight(RIGHT_FRAME_CARD_HEIGHT);
    ptn->setFixedWidth(RIGHT_FRAME_CARD_WIDTH);
    QPixmap pixmap(picPath);
    QPixmap fitpixmap = pixmap.scaled(RIGHT_FRAME_CARD_WIDTH, RIGHT_FRAME_CARD_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ptn->setIcon(QIcon(fitpixmap));
    ptn->setIconSize(QSize(500, 500));
    ptn->setFlat(true);
    //ptn->setStyleSheet("border: 0px");

    widget->TextEdit = new QTextEdit();
    QTextEdit *textEdit = widget->TextEdit;
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Policy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Policy::MinimumExpanding);
    textEdit->setMinimumHeight(RIGHT_FRAME_MINMUM_HEIGHT);
    textEdit->setText(text);
    textEdit->setReadOnly(true);
}

int YGODataManager::LoadDeck(const QString & filePath)
{
    QFileInfo fileinfo(filePath);
    QFile file(filePath);
    if (!fileinfo.isFile() || !file.open(QIODevice::ReadOnly))
        return -1;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Encoding::Utf8);

    QString temp;
    LocationInfo curDeck = Unknown; //for 1 : main deck;2 :ex deck; 3 :side deck;

    map<QString ,int> mapCheckRepeat;
    int mainCardNum = 0;
    int extraCardNum = 0;
    int sideCardNum = 0;

    deck_->clear();

    while (!in.atEnd())
    {
        in>>temp;
        bool ok;
        temp.toInt(&ok ,10);
        if (ok)
        {
            if (GetCardDataByCode(temp) == nullptr) return false;

            QString picPath = GetCardPicPathByCardCode(temp);
            QFileInfo file(picPath);
            if (!file.isFile())
            {
                picPath = defaultCardPic_;
            }

            YGOCardPushButton * btn = new YGOCardPushButton(picPath, temp);
            btn->setFixedHeight(DECK_CARD_HEIGHT);
            btn->setFixedWidth(DECK_CARD_WIDTH);
            QPixmap pixmap(picPath);
            QPixmap fitpixmap = pixmap.scaled(DECK_CARD_WIDTH, DECK_CARD_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            btn->setIcon(QIcon(fitpixmap));
            btn->setIconSize(QSize(500, 500));
            btn->setFlat(true);

            if (curDeck == Tab1MainDeck)
            {
                deck_->setMainCards_.insert(btn);
                ++mainCardNum;
                btn->SetLocation(Tab1MainDeck);
            }
            else if (curDeck == Tab1ExtraDeck)
            {
                deck_->setExtraCards_.insert(btn);
                ++extraCardNum;
                btn->SetLocation(Tab1ExtraDeck);
            }
            else if (curDeck == Tab1SideDeck)
            {
                deck_->setSideCards_.insert(btn);
                ++sideCardNum;
                btn->SetLocation(Tab1SideDeck);
            }
        }
        else if (QString::compare(temp , "#main") == 0)
        {
            curDeck = Tab1MainDeck;
        }
        else if(QString::compare(temp , "#extra") == 0)
        {
            curDeck = Tab1ExtraDeck;
        }
        else if(QString::compare(temp , "!side") == 0)
        {
            curDeck = Tab1SideDeck;
        }
    }

    if (mainCardNum < 40 || mainCardNum > 60)
        return 0;

    if (extraCardNum > 15)
        return 0;

    if (sideCardNum > 15)
        return 0;

    return 1;
}

int YGODataManager::SaveDeck()
{
    if (deck_->setMainCards_.size() > 60 ||
            deck_->setMainCards_.size() < 40 ||
            deck_->setExtraCards_.size() > 15 ||
            deck_->setSideCards_.size() > 15)
        return -1;

    QString fileName = HistoryMgrIns.GetHistory("LastOpenedPath");
    if (fileName.compare("") == 0)
        fileName = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QString("DefaultDeck.ydk");

    QFileInfo fileinfo(fileName);
    QFile file(fileName);
    if (!fileinfo.isFile() || !file.open(QIODevice::WriteOnly))
        return 0;

    QTextStream out(&file);
    WriteDeckToFile(out,deck_);

    HistoryMgrIns.SetHistory("LastOpenedPath",fileName);

    isChanged_ = false;
    return 1;
}


const CardData *YGODataManager::GetCardDataByCode(const QString code)
{
    if (mapCardDatas_.find(code) != mapCardDatas_.end())
        return &mapCardDatas_[code];
    return nullptr;
}

LocationInfo YGODataManager::GetCardLocationByCode(const QString code)
{
    const CardData *ptr = GetCardDataByCode(code);
    if (ptr == nullptr)
        return LocationInfo::Unknown;

    if ((ptr->Type & TYPE_FUSION)||
            (ptr->Type & TYPE_SYNCHRO)||
            (ptr->Type & TYPE_XYZ)||
            (ptr->Type & TYPE_LINK))
        return LocationInfo::Tab1ExtraDeck;
    return LocationInfo::Tab1MainDeck;
}

void YGODataManager::WriteDeckToFile(QTextStream &out,DeckWidgetCache *deck)
{
    out<<"#main\n";
    for (auto ptr : deck->setMainCards_.GetInnerSet())
        out<<ptr->GetCardCode()<<"\n";

    out<<"#extra\n";
    for (auto ptr : deck->setExtraCards_.GetInnerSet())
        out<<ptr->GetCardCode()<<"\n";

    out<<"!side\n";
    for (auto ptr : deck->setSideCards_.GetInnerSet())
        out<<ptr->GetCardCode()<<"\n";

    isChanged_ = false;
}

CardSet* YGODataManager::GetNewDeck()
{
    CardSet * newDeck = new CardSet(extraBool_);
    vecEditDeck_.push_back(newDeck);
    return newDeck;
}

CardSet* YGODataManager::GetRandomDeck(CardSet * catSet ,int num)
{
    CardSet * newDeck = new CardSet(extraBool_);
    int deckCardNum =  catSet->size();
    vector<int> vec;
    vec.reserve(deckCardNum);
    for (int i =0 ;i < deckCardNum ;i++)
        vec.push_back(i);

    std::shuffle(vec.begin(), vec.end(), RandomDevice);

    vector<QString> vecString;
    for (auto it : catSet->GetInnerSet())
    {
        vecString.push_back(it->GetCardCode());
    }

    for (int i = 0; i< num ;i++)
    {
        QString cardCode = vecString[vec[i]];
        QString picPath = GetCardPicPathByCardCode(vecString[vec[i]]);
        QFileInfo file(picPath);
        if (!file.isFile())
        {
            picPath = defaultCardPic_;
        }

        YGOCardPushButton * btn = new YGOCardPushButton(picPath, cardCode);
        btn->setFixedHeight(DECK_CARD_HEIGHT);
        btn->setFixedWidth(DECK_CARD_WIDTH);
        QPixmap pixmap(picPath);
        QPixmap fitpixmap = pixmap.scaled(DECK_CARD_WIDTH, DECK_CARD_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        btn->setIcon(QIcon(fitpixmap));
        btn->setIconSize(QSize(500, 500));
        btn->setFlat(true);
        newDeck->insert(btn);
    }

    vecEditDeck_.push_back(newDeck);
    return newDeck;
}

void YGODataManager::DeleteEditDeck(int index)
{
    if (vecEditDeck_.size() <= index )
        return  ;

    CardSet * ptr = vecEditDeck_[index];
    delete ptr;
    vecEditDeck_.erase(vecEditDeck_.begin() + index);
}

void YGODataManager::DeleteAllEditDeck()
{
    for (int i = 0; i < vecEditDeck_.size() ; ++i)
        delete vecEditDeck_[i];
    vecEditDeck_.clear();
}

map<QString ,int> YGODataManager::GetDeckMapRecord(CardSet * set)
{
    map<QString ,int> tempMap;
    for (auto j : set->GetInnerSet())
    {
        tempMap[j->GetTrueCardCode()]++;
    }
    return tempMap;
}


QString YGODataManager::CalRate(int drawNum)
{
    double rate = 0;
    int deckNum = vecEditDeck_.size();
    if (deckNum == 0)
        return "";

    vector<map<QString ,int>> vecEditDeckRecorder ;

    //预处理
    for (int i = 0; i< deckNum ;++i)
    {
        vecEditDeckRecorder.push_back(GetDeckMapRecord(vecEditDeck_[i]));
    }

    int mainDeckCardNum = deck_->setMainCards_.size();
    vector<int> vec;
    vec.reserve(mainDeckCardNum);
    for (int i =0 ;i < mainDeckCardNum ;i++)
        vec.push_back(i);

    vector<QString> vecString;
    for (auto it : deck_->setMainCards_.GetInnerSet())
    {
        vecString.push_back(it->GetTrueCardCode());
    }

    for (int i = 0 ;i < TEST_TIME; ++i)
    {
        std::shuffle(vec.begin(), vec.end(), RandomDevice);

        map<QString ,int> tempMap;
        for (int j = 0 ; j < drawNum ;++j)
            ++tempMap[vecString[vec[j]]];

        for ( int j = 0; j < deckNum ;++j)
        {
            bool isFound = true;
            if (vecEditDeckRecorder.size() == 0)
                continue;
            for (auto &it : vecEditDeckRecorder[j])
            {
                if (it.second > tempMap[it.first])
                {
                    isFound = false;
                }
            }
            if (isFound)
            {
                ++rate;
                break;
            }
        }
    }
    int intPart,decimalPart;
    intPart = (int)((rate/TEST_TIME)*100);
    QString decimal;
    decimalPart =((int)((rate/TEST_TIME)*1000000))%10000;
    if (decimalPart < 1000)
        decimal+="0";
    if (decimalPart < 100)
        decimal+="0";
    if (decimalPart < 10)
        decimal+="0";
    decimal+= QString::number(decimalPart);
    return QString::number(intPart)+"."+decimal+"%";
}




