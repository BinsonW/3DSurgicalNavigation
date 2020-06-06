#include <QSqlError>
#include <QDebug>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QDir>

#include "database.h"

DataBase* DataBase:: m_instance = NULL;

DataBase::DataBase()
{
    m_qSqlDB = QSqlDatabase::addDatabase("QSQLITE");
    m_bConnected = false;
    m_dbTable = "data";
    m_iColumns = 15;
}

DataBase::~DataBase()
{
    closeDB();
}

// 获取数据库对象
DataBase* DataBase::getInstance()
{
    if (m_instance == NULL)
    {
        m_instance = new DataBase;
        return m_instance;
    }
    else
        return m_instance;
}

// 关闭数据库
void DataBase::closeDB()
{
    m_qQuery.clear();
    m_qSqlDB.close();
    qDebug()<<"-----------database go over!";
}

// 连接数据库
bool DataBase::connectDB(QString dbuser, QString dbpassword, QString dbname)
{
    // 参数设置
    m_qSqlDB.setHostName("localhost");
    m_qSqlDB.setDatabaseName(dbname);
    m_qSqlDB.setUserName(dbuser);
    m_qSqlDB.setPassword(dbpassword);

    // 连接
    m_bConnected = m_qSqlDB.open();
    m_qQuery = QSqlQuery(m_qSqlDB);
    if(m_bConnected)
        qDebug()<<"Database Connect OK.";
    else
        qDebug()<<"Database Connect Fail.";

    return m_bConnected;
}

// 选择数据库
bool DataBase::selectDB(QString dbname)
{
    // 检查数据库是否已连接
    if(!m_bConnected)
        return false;

    // 连接数据库
    m_qSqlDB.setDatabaseName(dbname);
    m_bConnected = m_qSqlDB.open();
    m_qQuery = QSqlQuery(m_qSqlDB);

    return m_bConnected;
}

// 执行返回布尔值的SQL语句
bool DataBase::executeSQL_bool(const QString& sql_str)
{
    QMutexLocker locker(&m_mutex);
    if (!m_bConnected) {
        qDebug()<<"No Connection.";
        return false;
    }

    // 释放资源
    m_qQuery.clear();

    // 开始创建
    bool ret = m_qQuery.exec(sql_str);
    if(!ret)
        qDebug()<<"executeSQL: "<<sql_str<<", error: "<<m_qQuery.lastError();
    else
        qDebug()<<"execute SQL OK:"<<sql_str;
    return ret;
}

// 执行SQL语句进行查询
int DataBase::executeSQL(const QString& sql_str)
{
    QMutexLocker locker(&m_mutex);
    if (!m_bConnected) {
        qDebug()<<"No Connection.";
        return -1;
    }

    // 释放资源
    m_qQuery.clear();

    // 开始查询
    bool ret = m_qQuery.exec(sql_str);
    if(!ret) {
        qDebug()<<"executeSQL:"<<sql_str<<", error:"<<m_qQuery.lastError();
        return -1;
    }

    // 返回查询结果数
    return m_qQuery.size();
}

// 连接数据库
bool DataBase::linkDB(QString dbUser, QString dbPasswd, QString dbName, QString dbTable)
{
    m_dbTable = dbTable;

    // 连接
    if(!connectDB(dbUser, dbPasswd, dbName))
        return false;

    // 创建表
    QString sql_str = QString("select * from %1").arg(dbTable);
    if(!executeSQL_bool(sql_str)) // 判断表是否存在
    {
        // 表不存在，创建表
        qDebug()<<"Create Table.";
        sql_str = QString("create table %1 "
                          "(id varchar primary key, name varchar, "
                          "sex varchar, checkpos varchar, age int, "
                          "checkdate date, checkroom varchar, menzhenno varchar, "
                          "zhuyuanno varchar, bingquno varchar, chuangweino varchar, "
                          "tocheckdoctor varchar, checkdoctor varchar, comment varchar, "
                          "savepath varchar)").arg(dbTable);
        return executeSQL_bool(sql_str);
    }
    return true;
}

// 插入
bool DataBase::insertItem(QVector<QString> vecitem)
{
    if(!m_bConnected)
        return false;

    QMutexLocker locker(&m_mutex);
    m_qQuery.clear();

    // 准备字符串
    QString str = QString("insert into %1 values(?").arg(m_dbTable);
    for(int i = 1; i < vecitem.size(); i++)
        str += ", ?";
    str += ")";
    m_qQuery.prepare(str);

    // 填充数据
    for(int i = 0; i < vecitem.size(); i++)
        m_qQuery.bindValue(i, vecitem[i]);

    // 插入数据库
    if(!m_qQuery.exec())
    {
        QSqlError lastError = m_qQuery.lastError();
        qDebug() << lastError.driverText() << " Fail";
        return false;
    }

    qDebug() << "Insert a new item.";
    return true;
}

// 更新
bool DataBase::updateItem(QVector<QString> vecitem)
{
    if(!m_bConnected)
        return false;

    QMutexLocker locker(&m_mutex);
    m_qQuery.clear();

    // 准备字符串
    QString str = QString("update %1 set name = ?, sex = ?, checkpos = ?, age = ?,"
                          "checkdate = ?, checkroom = ?, menzhenno = ?, zhuyuanno = ?,"
                          "bingquno = ?, chuangweino = ?, tocheckdoctor = ?, checkdoctor = ?,"
                          "comment = ?, savepath = ? where id = ?").arg(m_dbTable);
    m_qQuery.prepare(str);

    // 填充数据
    for(int i = 1; i < vecitem.size(); i++)
        m_qQuery.bindValue(i-1, vecitem[i]);
    m_qQuery.bindValue(vecitem.size()-1, vecitem[0]);

    // 插入数据库
    if(!m_qQuery.exec())
    {
        QSqlError lastError = m_qQuery.lastError();
        qDebug() << lastError.driverText() << " Fail";
        return false;
    }

    qDebug() << "Update a item.";
    return true;
}

// 删除数据项
bool DataBase::deleteItem(QString strID)
{
    if(!m_bConnected)
        return false;

    QMutexLocker locker(&m_mutex);    

    // 查看数据项是否存在，存在则删除相应文件夹
    m_qQuery.clear();
    m_qQuery.exec(QString("select savepath from %1 where id = %2").arg(m_dbTable).arg(strID));
    while (m_qQuery.next())
    {
        QVariant qv = m_qQuery.value(0);
        QString strPath(qv.toString());
        QDir dir(strPath);
        if(dir.exists())
            dir.removeRecursively(); // 删除文件夹
    }

    // 从数据库中删除条目
    QString str = QString("delete from %1 where id = %2").arg(m_dbTable).arg(strID);
    m_qQuery.clear();
    m_qQuery.prepare(str);
    if(!m_qQuery.exec())
    {
        QSqlError lastError = m_qQuery.lastError();
        qDebug() << lastError.driverText() << " Fail";
        return false;
    }

    qDebug() << "Delete a item.";
    return true;
}

// 删除一定日期之前的数据项
bool DataBase::deleteItems(QString strDate)
{
    if(!m_bConnected)
        return false;

    QMutexLocker locker(&m_mutex);

    // 查看数据项是否存在，存在则删除相应文件夹
    QString str;
    if(strDate.isEmpty()) // 空的话，全删
        str = QString("select savepath from %1").arg(m_dbTable);
    else
        str = QString("select savepath from %1 where checkdate < '%2'").arg(m_dbTable).arg(strDate);
    m_qQuery.clear();
    m_qQuery.exec(str);
    while (m_qQuery.next())
    {
        QVariant qv = m_qQuery.value(0);
        QString strPath(qv.toString());
        QDir dir(strPath);
        if(dir.exists())
            dir.removeRecursively(); // 删除文件夹
    }

    // 从数据库中删除条目
    if(strDate.isEmpty()) // 空的话，全删
        str = QString("delete from %1").arg(m_dbTable);
    else
        str = QString("delete from %1 where checkdate < '%2'").arg(m_dbTable).arg(strDate);
    m_qQuery.prepare(str);
    if(!m_qQuery.exec())
    {
        QSqlError lastError = m_qQuery.lastError();
        qDebug() << lastError.driverText() << " Fail";
        return false;
    }

    qDebug() << "Delete items.";
    return true;
}

// 查询已有条数
int DataBase::queryCount()
{
    if(!m_bConnected)
        return -1;

    QMutexLocker locker(&m_mutex);
    m_qQuery.clear();
    m_qQuery.exec(QString("select id from %1").arg(m_dbTable));

    int icount = 0;
    while (m_qQuery.next())
        icount ++;
    qDebug()<<"Find "<<icount<<" items";
    return icount;
}

// 查询已有条数和占用空间
bool DataBase::queryMemory(int &icount, qint64 &isize)
{
    if(!m_bConnected)
        return false;

    QMutexLocker locker(&m_mutex);
    m_qQuery.clear();

    // 找路径
    m_qQuery.exec(QString("select savepath from %1").arg(m_dbTable));
    icount = 0;
    isize = 0;
    QStringList filters;
    filters<<QString("*.png");
    while (m_qQuery.next())
    {
        icount ++;

        // 统计图像的大小
        QVariant qv = m_qQuery.value(0);
        QString strPath(qv.toString());
        QDir dir(strPath);
        if(dir.exists())
        {
            dir.setNameFilters(filters);
            dir.setFilter(QDir::Files);
            foreach(QFileInfo mfi, dir.entryInfoList())
                isize += mfi.size();
        }
    }

    return true;
}

// 查询所有信息
void DataBase::queryAllItems(QVector<QVector<QString>> &allitems, QString strname)
{
    allitems.clear();
    if(!m_bConnected)
        return;

    // 查询
    QMutexLocker locker(&m_mutex);
    m_qQuery.clear();
    QString strQuery;
    if(strname.isEmpty())
        strQuery = QString("select * from %1").arg(m_dbTable);
    else
        strQuery = QString("select * from %1 where name like '%2%%' or "
                           "name like '%%%2' or name like '%%%%2%%'").arg(m_dbTable).arg(strname);
    m_qQuery.exec(strQuery);

    // 读出信息
    QVector<QString> vecValue;
    while (m_qQuery.next())
    {
        vecValue.clear();
        for (int i = 0; i < m_iColumns; i++)
        {
            QVariant qv = m_qQuery.value(i);
            QString str_v(qv.toString());
            vecValue.push_back(str_v);
        }
        //allitems.push_back(vecValue);
        allitems.push_front(vecValue);
    }
}

// 根据ID查询是否存在条目
bool DataBase::queryItemByID(QString strID)
{
    if(!m_bConnected)
        return false;

    // 查询
    QMutexLocker locker(&m_mutex);
    m_qQuery.clear();

    QString strQuery;
    strQuery = QString("select name from %1 where id = '%2'").arg(m_dbTable).arg(strID);
    m_qQuery.exec(strQuery);

    // 是否存在条目
    int icount = 0;
    while (m_qQuery.next())
        icount++;
    return icount>0;
}
