#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>
#include <QSqlError>
#include <QSqlRecord>
#include <exception>
#include <QMessageBox>
#include <QObject>

#include "dbconnection/dbprop.h"
#include "dbconnection/proptransactionworker.h"

// use of database is not meaningful for now
// connection information are hardcoded below:
#define MYSQL_HOSTNAME "localhost"
#define MYSQL_DB_NAME "fun_test"
#define MYSQL_USER "root"
#define MYSQL_PASSWORD "password"

/**
 * @brief DBProp::DBProp
 * @param maxTrial max waiting loop when noMoreFetch = false and buffer->fetched is empty @see load
 * @param sleepTime sleeping time (ms) in the loop @see load
 */
DBProp::DBProp(unsigned int maxTrial, unsigned int sleepTime):
    QObject (),
    Prop(),
    buffer(std::make_shared<PropTransactionBuffer>()),
    transactionWorker(std::make_shared<DBConnection>(
                          MYSQL_HOSTNAME,
                          MYSQL_DB_NAME,
                          MYSQL_USER,
                          MYSQL_PASSWORD
                          )),
    maxTrial(maxTrial),
    sleepTime(sleepTime)
{
    transactionWorker.setBuffer(buffer);

    transactionWorker.moveToThread(&transactionThread);

    connect(this, SIGNAL(requireUpdate()), &transactionWorker, SLOT(update()));
    connect(this, SIGNAL(requireFetch()),&transactionWorker, SLOT(addToBuffer()));
    connect(&transactionThread, SIGNAL(started()), &transactionWorker, SLOT(init()));

    transactionThread.start();
    emit requireFetch();
}

/**
 * @warning Error in my_thread_global_end(): 1 threads didn't exit --> debug destruction @todo
 * @brief DBProp::~DBProp stop transactionThread
 */
DBProp::~DBProp()
{
    transactionThread.quit();
    transactionThread.wait();
}

/**
 * @brief DBProp::save
 * @details append this to buffer->toUpdate and emit requireUpdate
 */
void DBProp::save() const
{
    Prop toUpdate(*this);
    buffer->tupMutex.lock();
    buffer->toUpdate.push_back(toUpdate);
    buffer->tupMutex.unlock();
    emit requireUpdate();
}

/**
 * @brief DBProp::load load next property from buffer in this
 * @details
 * Safely try to access buffer->fetched to pop the next Prop and assign it to this.
 * If buffer is empty try up to maxTrial to wait for querying thread sleepTime, then return false.
 * Emit requireFetch when buffer is too small @see PropTransactionBuffer::fetchedMinSize
 * @return bool, false if no more prop or fail, true otherwise
 */
bool DBProp::load()
{
    unsigned int counter = 0;
    while (true) {
        // while condition (locked)
        buffer->ftcMutex.lock();
        if (!buffer->fetched.empty()) {
            buffer->ftcMutex.unlock();
            break;
        }

        if (buffer->noMoreFetch.load()){
            buffer->ftcMutex.unlock();
            return false;
        } else {
            buffer->ftcMutex.unlock();
            // @todo QThread::wait() here his not functionning as expected find a wait to delete with avoiding the explicit sending of QEvent
            if (counter > maxTrial) return false; // and send a msg to user. "Loading failed"
            counter++;
            QThread::currentThread()->msleep(sleepTime);
        }
    }

    Prop newProp = buffer->fetched.front();
    buffer->fetched.pop_front();
    unsigned long bufferSize = buffer->fetched.size();
    buffer->ftcMutex.unlock();

    if (!buffer->noMoreFetch.load() && bufferSize <= buffer->fetchedMinSize.load()) {
        emit requireFetch();
    }

    idprop = newProp.idprop;
    name = newProp.name;
    def = newProp.def;
    nDay = newProp.nDay;
    nextReview = newProp.nextReview;
    return true;
}
