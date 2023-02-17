#include <QDir>
#include <QFileInfo>

#include "HistoryManager.h"
#include "YGODataManager.h"

HistoryManager::HistoryManager()
{
    QString filePath = QDir::currentPath() + BUILD_PREFIX + "/history";

    file_ = new QFile(filePath);
    file_->open(QIODevice::ReadWrite);

    fileSteam_ = new  QTextStream(file_);
    fileSteam_->setEncoding(QStringConverter::Encoding::Utf8);
    QString key;
    QString value;

    while (!fileSteam_->atEnd())
    {
        *fileSteam_>>key>>value;
        if (key.compare("") != 0)
            mapKeyValue_[key] = value;
    }
}

const QString HistoryManager::GetHistory(const QString key)
{
    if (mapKeyValue_.find(key)!= mapKeyValue_.end())
        return mapKeyValue_[key];
    return "";
}

void HistoryManager::SetHistory(const QString key , const QString value)
{
    mapKeyValue_[key] = value;
    SaveHistory();
}

void HistoryManager::SaveHistory()
{
    QString filePath = QDir::currentPath() + BUILD_PREFIX + "/history";

    QFile file(filePath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Encoding::Utf8);

    for (auto & pair : mapKeyValue_)
        stream<<pair.first.toUtf8()<<' '<<pair.second.toUtf8()<<'\n';
}

