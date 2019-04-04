#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSqlDatabase>

/*
START-STOP Mysql local server
https://dev.mysql.com/doc/mysql-getting-started/en/
From https://dev.mysql.com/doc/mysql-apt-repo-quick-guide/en/
shell> sudo service mysql status
shell> sudo service mysql stop
shell> sudo service mysql start

connect from cmd line:
shell> mysql -u root -p
PASSWORD: "password"
 */

/**
 * @brief The DBConnection class
 *
 * @details
 * Generate QSqlDatabase object with appropriate init for the database connection.
 *
 * @warning not thread-safe @todo
 */
class DBConnection
{
public:
    DBConnection(QString const& hostname, QString const& dbname,
                 QString const& user, QString const& password);
    ~DBConnection();

    QSqlDatabase getQSqlDatabase();
protected:
    const QString hostname;
    const QString dbname;
    const QString user;
    const QString password;


};

#endif // DBCONNECTION_H
