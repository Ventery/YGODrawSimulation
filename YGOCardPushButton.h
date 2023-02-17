#ifndef YGOCARDPUSHBUTTON_H
#define YGOCARDPUSHBUTTON_H
#include <QPushButton>
#include <string>

#include "Common.h"
using std::string;

class DeckWidget;
class YGOCardPushButton : public QPushButton
{
    Q_OBJECT

public:

    YGOCardPushButton(QString picPath ,QString code ,QWidget * parent = nullptr);

    const QString &GetPicPath() const { return picPath_;}
    const QString &GetCardCode() const { return code_;}
    const QString &GetTrueCardCode() const { return trueCode_;}
    const int &GetNum() const { return num_;};
    LocationInfo GetLocation(){return location_;}
    void SetLocation(LocationInfo location){location_ = location;}

private:
    int static AddCounter() {return ++counter_;}

    static int counter_;

    QString picPath_;
    QString code_;
    QString trueCode_;
    int num_;
    LocationInfo location_;
};

#endif // YGOCARDPUSHBUTTON_H
