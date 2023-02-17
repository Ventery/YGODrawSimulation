#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <map>

#include <QString>
#include <QFile>
#include <QTextStream>

#include "Singleton.h"

#define HistoryMgrIns HistoryManager::GetInstance()

class HistoryManager : public Singleton<HistoryManager>
{
public:
    HistoryManager();
    const QString GetHistory(const QString key);
    void SetHistory(const QString key , const QString value);

private:
    void SaveHistory();

    std::map<QString ,QString> mapKeyValue_;
    QFile *file_;
    QTextStream  *fileSteam_;
};

#endif // HISTORYMANAGER_H
