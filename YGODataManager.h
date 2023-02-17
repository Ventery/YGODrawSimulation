#ifndef YGODATAMANAGER_H
#define YGODATAMANAGER_H

#include <map>
#include <set>

#include <QTextEdit>
#include <QTextStream>

#include "Common.h"
#include "CSqliteOperator.h"
#include "Singleton.h"
#include "YGOCardPushButton.h"

#ifdef _DEBUG
    #define BUILD_PREFIX QString("/debug")
#else
    #define BUILD_PREFIX QString("release")
#endif

#define DataMgrIns YGODataManager::GetInstance()

struct WidgetCache;
class DeckWidgetCache;
class CardSet;

class YGODataManager : public Singleton<YGODataManager>
{
public:
    YGODataManager();
    ~YGODataManager();
    bool Init();
    int LoadDeck(const QString&);
    int SaveDeck();
    void WriteDeckToFile(QTextStream & ,DeckWidgetCache *);

    int GetCardInfoByKeyWord(const QString ,map<QString, WidgetCache* >&);
    QString GetCardPicPathByCardCode(const QString &);
    void FillWidget(WidgetCache * ,const QString, const QString, const QString );
    const CardData * GetCardDataByCode(const QString code);
    LocationInfo GetCardLocationByCode(const QString code);
    DeckWidgetCache* GetDeck(){return deck_;}
    bool IsDeckChanged(){return isChanged_;}
    CardSet* GetRandomDeck(CardSet * ,int num);
    CardSet* GetNewDeck();
    void DeleteEditDeck(int index);
    void DeleteAllEditDeck();
    QString CalRate(int drawNum);

    WidgetCache* defaultWidget_;
    QString defaultCardPic_;

private:
    bool InitDB();
    bool InitPicIndex();
    bool LoadCardDatas(CSqliteOperator &);
    map<QString ,int> GetDeckMapRecord(CardSet *);

    WidgetCache* GetWidgetByCardData(const CardData &);

    bool isInited_;
    bool isDBInited_;
    bool isPicInited_;
    bool isChanged_;
    bool extraBool_;

    std::map<QString ,CardData> mapCardDatas_;
    std::map<QString ,WidgetCache> mapWidgetCache_;

    QString picPathPrefix_;

    DeckWidgetCache *deck_;
    vector<CardSet*> vecEditDeck_;
};

struct CompareFunc
{
    bool operator()(const YGOCardPushButton * left, const YGOCardPushButton * right) const
    {
        if (left == nullptr || right == nullptr) return false;

        const CardData* leftCardDataPtr = DataMgrIns.GetCardDataByCode(left->GetCardCode());
        const CardData* rightCardDataPtr = DataMgrIns.GetCardDataByCode(right->GetCardCode());
        if (leftCardDataPtr == nullptr || rightCardDataPtr == nullptr)
            return false;

        if ((leftCardDataPtr->Type & TYPE_MONSTER) && !(rightCardDataPtr->Type & TYPE_MONSTER))
            return true;

        if (leftCardDataPtr->Type & TYPE_SPELL)
        {
            if (rightCardDataPtr->Type & TYPE_MONSTER)
                return false;
            if (rightCardDataPtr->Type & TYPE_TRAP)
                return true;
        }

        if ((leftCardDataPtr->Type & TYPE_TRAP) && !(rightCardDataPtr->Type & TYPE_TRAP))
            return false;

        if (leftCardDataPtr->GetTrueCode() == rightCardDataPtr->GetTrueCode())
            return left->GetNum() < right->GetNum();

        if ((leftCardDataPtr->Type & TYPE_MONSTER) && (rightCardDataPtr->Type & TYPE_MONSTER) &&
                leftCardDataPtr->Level != rightCardDataPtr->Level)
            return leftCardDataPtr->Level < rightCardDataPtr->Level;

        return leftCardDataPtr->Code.compare(rightCardDataPtr->Code) < 0;
    }
};

class CardSet
{
public:
    CardSet(bool &symbol) :isChanged_(symbol){}
    const std::set<YGOCardPushButton*, CompareFunc>& GetInnerSet() {return innerSet_;}
    void insert(YGOCardPushButton* ptr ,bool isManual = false)
    {
        isChanged_ = isManual;
        QString trueCode = ptr->GetTrueCardCode();
        if (innerCardCountMap_[trueCode] <3)
        {
            ++innerCardCountMap_[trueCode];
            innerSet_.insert(ptr);
        }
    }

    void erase(YGOCardPushButton* ptr ,bool isManual = false)
    {
        isChanged_ = isManual;
        QString trueCode = ptr->GetTrueCardCode();
        if (innerCardCountMap_[trueCode])
        {
            --innerCardCountMap_[trueCode];
            innerSet_.erase(ptr);
        }
    }

    void clear(bool isManual = false)
    {
        isChanged_ = isManual;
        innerCardCountMap_.clear();
        innerSet_.clear();
    }

    size_t size() const
    {
        return innerSet_.size();
    }

private:
    std::set<YGOCardPushButton*, CompareFunc> innerSet_;
    std::map<QString,int>  innerCardCountMap_;
    bool &isChanged_;
};

class DeckWidgetCache
{
public:
    DeckWidgetCache(bool &symbol) : setMainCards_(symbol),setExtraCards_(symbol),setSideCards_(symbol),isChanged(symbol){}
    void clear()
    {
        setMainCards_.clear();
        setExtraCards_.clear();
        setSideCards_.clear();
        isChanged = false;
    }

    CardSet setMainCards_;
    CardSet setExtraCards_;
    CardSet setSideCards_;
    std::map<QString ,int> cardNumLimiter_;
    bool &isChanged;
};

struct WidgetCache
{
    YGOCardPushButton *Button;
    QTextEdit *TextEdit;
};

#endif // YGODATAMANAGER_H
