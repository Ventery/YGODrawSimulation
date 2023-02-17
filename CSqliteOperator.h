#ifndef CSQLITEOPERATOR_H
#define CSQLITEOPERATOR_H
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "./sqlite/sqlite3.h"
using namespace std;


class CSqliteOperator
{
public:
    CSqliteOperator();
    ~CSqliteOperator();

    int CreateSqlFile(const string& path);
    int CreateTable(const string& sql);
    int Open(const string& path);

    int Insert(const string& sql);
    int Delete(const string& sql);
    int Update(const string& sql);
    int FindCurrentTableMaxKey(const string& tableName, const string& strKey, int& nMaxKey);//查找当前表最大主键
    int FindAllData(const string& sql , vector<string>& arrKey, vector<vector<string>>& arrValue);

private:
    //sqlie对象的销毁放在析构里，不需要用户关心
    void Destory();
private:
    sqlite3 *pDB;
};
#endif // CSQLITEOPERATOR_H
