#ifndef COMMON_H
#define COMMON_H

#include <QString>

#include <algorithm>
#include <random>
#include <chrono>
typedef std::function<void()> CallBack;

//global control const
#define DECK_CARD_WIDTH 59
#define DECK_CARD_HEIGHT 86
#define RIGHT_FRAME_CARD_WIDTH 118
#define RIGHT_FRAME_CARD_HEIGHT 172
#define RIGHT_FRAME_MINMUM_HEIGHT 250
#define MAXNUM_DISPLAY_SEARCH_RESULT 50
#define DECK_FRAME_COLUMN 10
#define TAB2_DECK_COLUMN 6
#define TAB2_EDIT_DECK_COLUMN 7
#define TEST_TIME 100000


//types
#define TYPE_MONSTER		0x1
#define TYPE_SPELL			0x2
#define TYPE_TRAP			0x4
#define TYPE_NORMAL		0x10
#define TYPE_EFFECT		0x20
#define TYPE_FUSION		0x40
#define TYPE_RITUAL		0x80
#define TYPE_TRAPMONSTER	0x100
#define TYPE_SPIRIT		0x200
#define TYPE_UNION			0x400
#define TYPE_DUAL			0x800
#define TYPE_TUNER			0x1000
#define TYPE_SYNCHRO		0x2000
#define TYPE_TOKEN			0x4000
#define TYPE_QUICKPLAY		0x10000
#define TYPE_CONTINUOUS	0x20000
#define TYPE_EQUIP			0x40000
#define TYPE_FIELD			0x80000
#define TYPE_COUNTER		0x100000
#define TYPE_FLIP			0x200000
#define TYPE_TOON			0x400000
#define TYPE_XYZ			0x800000
#define TYPE_PENDULUM		0x1000000
#define TYPE_SPSUMMON		0x2000000
#define TYPE_LINK			0x4000000

struct CardData {
    QString Code;
    QString Alias;
    unsigned long long Setcode;
    unsigned int Type;
    unsigned int Level;
    unsigned int Attribute;
    unsigned int Race;
    int Attack;
    int Defense;
    unsigned int Lscale;
    unsigned int Rscale;
    unsigned int LinkMarker;
    unsigned int Ot;
    unsigned int Category;

    QString Name;
    QString Text;
    QString Desc[16];

    QString GetTrueCode() const
    {
        if (Alias.compare("0") == 0)
            return Code;
        return Alias;
    }

    QString GetContent() const
    {
        QString temp;
        temp = Code+"\n";
        /*temp += to_string(Alias)+"\n";
        temp += to_string(Setcode)+"\n";
        temp += to_string(Type)+"\n";
        temp += to_string(Level)+"\n";
        temp += to_string(Attribute)+"\n";
        temp += to_string(Race)+"\n";
        temp += to_string(Attack)+"\n";
        temp += to_string(Defense)+"\n";
        temp += to_string(Lscale)+"\n";
        temp += to_string(Rscale)+"\n";
        temp += to_string(LinkMarker)+"\n";
        temp += to_string(Ot)+"\n";
        temp += to_string(Category)+"\n";*/
        temp += Name+"\n";
        temp += Text+"\n";
        /*for (int i = 0 ; i < 16 ;++i)
        {
            temp += Desc[i]+" ";
        }*/
        return temp;
    }
};

enum LocationInfo
{
    Unknown = 0,
    Tab1MainDeck = 1,
    Tab1ExtraDeck = 2,
    Tab1SideDeck = 3,
    SearchWidget = 4,
    Tab2MainDeck = 5,
    Tab2EditDeck = 6,
};

#endif // COMMON_H
