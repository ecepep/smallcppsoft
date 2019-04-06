#include <exception>
#include <QThread>
#include <QDebug>

#include "dbconnection/dbconnection.h"


DBConnection::DBConnection(QString const& hostname, QString const& dbname,
                           QString const& user, QString const& password):
    hostname(hostname),
    dbname(dbname),
    user(user),
    password(password)
{}

/**
 * @brief DBConnection::getQSqlDatabase
 * @return QSqlDatabase object initialized thanks to @class DBConnection members @see DBConnection
 */
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
