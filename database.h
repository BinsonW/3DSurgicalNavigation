#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QMutexLocker>
#include <QMutex>
#include <QVector>

class DataBase
{
public:
    DataBase();
    ~DataBase();

public:
    static DataBase*    m_instance;
    QSqlDatabase        m_qSqlDB;
    QSqlQuery           m_qQuery;
    bool                m_bConnected;
    QMutex              m_mutex;

    QString             m_dbTable;
    int                 m_iColumns;

public:
//    bool createConnection();  //创建一个连接
//    bool createTable();       //创建数据库表
//    bool insert();            //出入数据
//    bool queryAll();          //查询所有信息
//    bool updateById(int id);  //更新
//    bool deleteById(int id);  //删除
//    bool sortById();          //排序

    // 底层工具函数
    static DataBase *getInstance();
    bool connectDB(QString dbuser, QString dbpassword, QString dbname);
    void closeDB();
    bool selectDB(QString dbname);
    int executeSQL(const QString &sql_str);
    bool executeSQL_bool(const QString &sql_str);

    // 针对本应用的上层函数
    bool linkDB(QString m_dbUser, QString m_dbPasswd, QString m_dbName, QString m_dbTable);
    int queryCount();
    bool insertItem(QVector<QString> vecitem);
    void queryAllItems(QVector<QVector<QString> > &allitems, QString strname);
    bool deleteItem(QString strID);
    bool deleteItems(QString strDate);
    bool updateItem(QVector<QString> vecitem);
    bool queryItemByID(QString strID);
    bool queryMemory(int &icount, qint64 &isize);
};

#endif // DATABASE_H
