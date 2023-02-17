#include "YGOCardPushButton.h"
#include "YGODataManager.h"

YGOCardPushButton::YGOCardPushButton(QString picPath ,QString code ,QWidget * parent)
    :QPushButton(parent),
      picPath_(picPath),
      code_(code)
{
    num_ = AddCounter();
    trueCode_ = DataMgrIns.GetCardDataByCode(code)->GetTrueCode();
}

int YGOCardPushButton::counter_ = 0;


