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

/**
 * @brief PropTransactionWorker::init init this in thread
 * @details
 * The constructor is initializing in the the thread of parent object aka DBProp's thread.
 * Hence init is call through the qt::connect system after the call to this->moveToThread(otherThread)
 * This is necessary since QSqlDatabase's objects cannot work on multiple threads call.
 */
void PropTransactionWorker::init() {
    // db should remain in the thread of this.
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

/**
 * @brief PropTransactionWorker::updateExec generate and execute the update query
 * @param toUpdate
 * @note throw if unsuccessful @todo deal with it
 */
void PropTransactionWorker::updateExec(Prop const& toUpdate) const{
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

/**
 * @brief PropTransactionWorker::update update all prop in buffer to db.
 * @details
 * Safely pop all the element of buffer->toUpdate and update db with their value.
 * @see updateExec
 * @see signal DBProp::requireUpdate
 */
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

            updateExec(toUpdate);
        }
    }
}

/**
 * @brief PropTransactionWorker::fetch fetch transactionSize next Prop from db
 * @param query, prepare and execute query
 * @param transactionSize, limit transaction to transactionSize
 * @todo clean synthax of NOT IN and its values' binding
 * @see addToBuffer
 * @note throw if unsuccessful @todo deal with it
 */
void PropTransactionWorker::fetch(QSqlQuery& query, unsigned int const& transactionSize) {
    //create the string inside the 'NOT IN()'
    QStringList idLoadedStrList;
    for(unsigned int id: idpropCurrentlyLoaded){
        // QString::number(id); error in qstring to int conversion somewhere
        // hence id binded later
        idLoadedStrList << QString(":loaded_") + QString::number(id);
    }
    QString idLoadedStr = idLoadedStrList.join(",");

    // prepare query with 'idprop NOT IT()' if necessary
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

    // bind the values in the 'NOT IN()'
    int i = 0;
    for(std::deque<unsigned int>::iterator it = idpropCurrentlyLoaded.begin();
        it != idpropCurrentlyLoaded.end(); it++,i++ )    {
        query.bindValue(QString(":loaded_") + QString::number(*it), *it);
    }

    // exec query
    bool success = query.exec();
    //    qDebug() << PropTransactionWorker::getLastExecutedQuery(query);
    if (!success) throw std::runtime_error("could not fetch db.");
}

/**
 * @brief PropTransactionWorker::addToBuffer query the db to refill the buffer
 * @details
 * Safely access buffer->fetched and append to it the necessary number of new Prop.
 * If prop is fetched for the first time init nDay to 0 (later reassigned in PropWindow::evaluateProp).
 *
 * @see PropTransactionBuffer::noMoreFetch
 * @see PropTransactionBuffer::fetchedMaxSize
 * @see PropTransactionWorker::fetch
 * @see signal DBProp::requireFetch
 */
void PropTransactionWorker::addToBuffer() {
    // find necessary transaction's size
    buffer->ftcMutex.lock();
    unsigned int size = static_cast<unsigned int>(buffer->fetched.size());
    buffer->ftcMutex.unlock();
    unsigned int transactionSize = buffer->fetchedMaxSize - size;

    if (transactionSize  < 1 ) return;

    // exec query
    QSqlQuery query(db);
    fetch(query, transactionSize);

    // resolve index in query
    int iidprop = query.record().indexOf("idprop");
    int inDay = query.record().indexOf("up.nDay");
    int inextReview = query.record().indexOf("up.nextReview");
    int iname = query.record().indexOf("p.name");
    int idef = query.record().indexOf("p.def");

    // if query is empty
    if (!query.next()) {
        buffer->noMoreFetch = true;
        return;
    }

    // push each rows of query as a Prop in buffer
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
