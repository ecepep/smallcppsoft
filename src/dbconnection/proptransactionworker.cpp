#include "QVariant"
#include <QSqlRecord>
#include <QMutexLocker>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <QSqlError>

#include "dbconnection/proptransactionworker.h"

PropTransactionWorker::PropTransactionWorker(std::shared_ptr<DBConnection> dbco):
    dbco(dbco)
{
}

void PropTransactionWorker::setBuffer(std::shared_ptr<PropTransactionBuffer> buffer) {
    this->buffer = buffer;
}

void PropTransactionWorker::init() {
    // db should remain in thread of this.
    db = dbco->getQSqlDatabase(); // std::unique_ptr<QSqlDatabase>();

    // Use of user in the app is not properly define in its design, hence this curious select
    QSqlQuery query("SELECT iduser FROM user WHERE username = 'Pep'", db);
    query.exec();
    if (query.next()) {
        iduser = query.value(0).toUInt();
    } else {
        throw std::runtime_error("no user");
    }
}

PropTransactionWorker::~PropTransactionWorker() {}

void PropTransactionWorker::update(Prop const& toUpdate ) const{
    QSqlQuery query(db);
    query.prepare("UPDATE userprop \
                  SET nDay = :nDay, nextReview= :nextReview \
            WHERE idprop = :idprop AND iduser = :iduser; ");
            query.bindValue(":nDay", toUpdate.nDay);

    query.bindValue(":nextReview", toUpdate.nextReview);
    query.bindValue(":idprop", toUpdate.idprop);
    query.bindValue(":iduser", iduser);

    bool success = query.exec();
    if (!success) throw std::runtime_error("could not update db.");
}

void PropTransactionWorker::update()
{
    while(true){
        buffer->tupMutex.lock();
        if (buffer->toUpdate.empty()){
            buffer->tupMutex.unlock();
            break;
        } else {
            Prop toUpdate = buffer->toUpdate.front();
            buffer->toUpdate.pop_front();
            buffer->tupMutex.unlock();

            unsigned int idprop = idpropCurrentlyLoaded.front();
            idpropCurrentlyLoaded.pop_front();
            assert(idprop == toUpdate.idprop); // Safety check

            update(toUpdate);
        }
    }
}

/**
 * @brief PropTransactionWorker::fetch
 * @param query
 * @param transactionSize
 * @todo clean synthax of NOT IN and its values' binding
 */
void PropTransactionWorker::fetch(QSqlQuery& query, unsigned int const& transactionSize) {
    QStringList idLoadedStrList;
    for(unsigned int id: idpropCurrentlyLoaded){
        // QString::number(id); error in qstring to int conversion somewhere
        // hence id binded later
        idLoadedStrList << QString(":loaded_") + QString::number(id);
    }
    QString idLoadedStr = idLoadedStrList.join(",");

    query.prepare(QString("SELECT * \
                  FROM userprop AS up \
                  INNER JOIN prop AS p  ON p.idprop = up.idprop \
            INNER JOIN user AS u  ON u.iduser = up.iduser \
            WHERE u.iduser = :iduser  \
            AND (up.nextReview <= :now  OR up.nextReview is NULL) ")
            + ((!idpropCurrentlyLoaded.empty()) ?
                QString("AND up.idprop NOT IN("+idLoadedStr+") ") : QString("")) +
            QString("ORDER BY up.nextReview ASC LIMIT :transactionSize;"));

    query.bindValue(":iduser", iduser);
    query.bindValue(":now", QDateTime::currentDateTime());
    query.bindValue(":transactionSize", transactionSize);

    int i = 0;
    for(std::deque<unsigned int>::iterator it = idpropCurrentlyLoaded.begin();
        it != idpropCurrentlyLoaded.end(); it++,i++ )    {
        query.bindValue(QString(":loaded_") + QString::number(*it), *it); // bind value of the NOT IN (), if not empty
    }

    bool success = query.exec();
//    qDebug() << PropTransactionWorker::getLastExecutedQuery(query);
    if (!success) throw std::runtime_error("could not fetch db.");
}

void PropTransactionWorker::addToBuffer() {
    buffer->ftcMutex.lock();
    unsigned int size = static_cast<unsigned int>(buffer->fetched.size());
    buffer->ftcMutex.unlock();
    unsigned int transactionSize = buffer->fetchedMaxSize - size;

    if (transactionSize  < 1 ) return;

    QSqlQuery query(db);
    fetch(query, transactionSize);

    int iidprop = query.record().indexOf("idprop");
    int inDay = query.record().indexOf("up.nDay");
    int inextReview = query.record().indexOf("up.nextReview");
    int iname = query.record().indexOf("p.name");
    int idef = query.record().indexOf("p.def");

    if (!query.next()) {
        buffer->noMoreFetch = true;
        return;
    }

    QMutexLocker locker(&(buffer->ftcMutex));
    do {
        Prop newProp;
        newProp.idprop = query.value(iidprop).toUInt();
        newProp.name = query.value(iname).toString();
        newProp.def = query.value(idef).toString();
        newProp.nDay = query.value(inDay).toDouble();
        if (newProp.nDay < 0) {
            // init nday and next review if never seen
            newProp.nextReview = QDateTime::currentDateTime(); //useless
            newProp.nDay = 0;
        } else {
            newProp.nextReview = query.value(inextReview).toDateTime();
        }
        idpropCurrentlyLoaded.push_back(newProp.idprop);
        buffer->fetched.push_back(newProp);
    } while (query.next());
}
