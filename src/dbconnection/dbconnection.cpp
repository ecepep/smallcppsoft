#include <exception>
#include <QThread>
#include <QDebug>

#include "dbconnection/dbconnection.h"


DBConnection::DBConnection(QString hostname, QString dbname, QString user, QString password):
    hostname(hostname),
    dbname(dbname),
    user(user),
    password(password)
{
}

QSqlDatabase DBConnection::getQSqlDatabase(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(hostname);
    db.setDatabaseName(dbname);
    db.setUserName(user);
    db.setPassword(password);
    if (!db.open()){
        throw std::runtime_error("cannot open database.");
    }
    return db;
}

DBConnection::~DBConnection()
{}
