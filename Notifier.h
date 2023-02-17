#ifndef NOTIFIER_H
#define NOTIFIER_H
#include <map>
#include <set>

enum EventType
{
    Et_AddCard = 0,         //卡组增加卡片
    Et_DeleteCard = 1,      //卡组删除卡片
    Et_ImportDeck = 2,      //导入卡组
    Et_NewDeck = 3,         //新建卡组
    Et_ResetDeck = 4,       //重置卡组
};

class Observer
{
public:
    virtual void OnNotify(EventType type) = 0;
};

class Notifier
{
public:
    virtual void InitNotifier() = 0;
    virtual void Attach(EventType type, Observer* observer) = 0;
    virtual void Detach(EventType type, Observer* observer) = 0;
    virtual void Notify(EventType type) = 0;

private:
    typedef std::set<Observer*> ObserverSet;
    typedef std::map<int,ObserverSet> NotifyList;
    NotifyList notifyList_;
};


#endif // NOTIFIER_H
