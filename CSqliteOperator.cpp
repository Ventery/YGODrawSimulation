#include "CSqliteOperator.h"


CSqliteOperator::CSqliteOperator()
{
    pDB = NULL;
}


CSqliteOperator::~CSqliteOperator()
{
    Destory();
}


void CSqliteOperator::Destory()
{
    if (pDB)
    {
        sqlite3_close(pDB);
        pDB = NULL;
    }
}

int CSqliteOperator::CreateSqlFile(const string& path)
{
    return sqlite3_open(path.c_str(), &pDB);
}

int CSqliteOperator::CreateTable(const string& sql)
{
    char *szMsg = NULL;
    return sqlite3_exec(pDB, sql.c_str(), NULL, NULL, &szMsg);
}

int CSqliteOperator::Open(const string& path)
{
    return sqlite3_open(path.c_str(), &pDB);
}

int CSqliteOperator::Insert(const string& sql)
{
    if (sql.empty()) return -1;

    char* zErrMsg = NULL;
    int ret = sqlite3_exec(pDB, sql.c_str(), NULL, NULL, &zErrMsg);
    return ret;
}

int CSqliteOperator::Delete(const string& sql)
{
    int nCols = 0;
    int nRows = 0;
    char **azResult = NULL;
    char *errMsg = NULL;
    return sqlite3_get_table(pDB, sql.c_str(), &azResult, &nRows, &nCols, &errMsg);
}

int CSqliteOperator::Update(const string& sql)
{
    char* zErrMsg = NULL;

    int ret = sqlite3_exec(pDB, sql.c_str(), NULL, NULL, &zErrMsg);
    return ret;
}

int CSqliteOperator::FindCurrentTableMaxKey(const string& tableName, const string& strKey, int& nMaxKey)
{
    nMaxKey = -1;
    if (tableName.empty() || strKey.empty()) return -1;
    string sql = "select * from " + tableName;

    int nCol = -1;
    int nRow = -1;
    int index = -1;
    char **azResult = NULL;
    char *errMsg = NULL;

    int result = sqlite3_get_table(pDB, sql.c_str(), &azResult, &nRow, &nCol, &errMsg);

    index = nCol;

    //取出最新的，对比穿进去的主键串，就是主键Max值
    for (int i = 0; i < nRow; i++)
    {
        for (int j = 0; j < nCol; j++)
        {
            string s1 = azResult[j];
            string s2 = azResult[index];
            if (s1 == strKey)
            {
                nMaxKey = atoi(azResult[index]);
            }
            index++;
        }
    }
    return result;
}

int CSqliteOperator::FindAllData(const string& sql, vector<string>& arrKey, vector<vector<string>>& arrValue)
{
    if (sql.empty()) return -1;

    int nCols = 0;
    int nRows = 0;
    char **azResult = NULL;
    char *errMsg = NULL;
    int index = 0;
    const int result = sqlite3_get_table(pDB, sql.c_str(), &azResult, &nRows, &nCols, &errMsg);

    index = nCols;
    arrKey.clear();
    arrKey.reserve(nCols);
    arrValue.clear();
    arrValue.reserve(nRows);

    bool bKeyCaptured = false;
    for (int i = 0; i < nRows; i++)
    {
        vector<string> temp;
        for (int j = 0; j < nCols; j++)
        {
            if (!bKeyCaptured)
            {
                arrKey.push_back(azResult[j]);
            }
            temp.push_back(azResult[index]);
            index++;
        }
        bKeyCaptured = true;
        arrValue.push_back(temp);
    }
    return result;
}
