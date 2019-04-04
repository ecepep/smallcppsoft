#ifndef DBPROP_H
#define DBPROP_H

#include <QString>
#include <QDate>
#include <vector>
#include <memory>
#include <deque>
#include <QMutex>
#include <QObject>

#include "dbconnection/dbconnection.h"
#include "GUI/prop.h"
#include "dbconnection/proptransactionworker.h"

/**
 * @warning Error in my_thread_global_end(): 1 threads didn't exit --> debug destruction @todo
 * @brief The DBProp class Prop interfacing with mysql db.
 * @details
 * Implement @see save and @see load of @see Prop to interface with mysql database.
 * This is in the same thread as the GUI however query's are executed in @see transactionThread.
 * @see buffer plays the role of the shared memory.
 * @see PropTransactionWorker
 * @see PropTransactionBuffer
 */
class DBProp: public QObject, public Prop
{
    Q_OBJECT
public:
    DBProp(unsigned int maxTrial=10, unsigned int sleepTime=250);
    ~DBProp();

    void save() const;
    bool load();
protected:
    std::shared_ptr<PropTransactionBuffer> buffer;/**< shared memory with the buffers. */
    PropTransactionWorker transactionWorker;/**< worker object. exec the query's */
    QThread transactionThread;/**< QThread of transactionWorker != from thread of this. */

signals:
    void requireUpdate() const;/**< @see slot PropTransactionWorker::update */
    void requireFetch();/**< @see slot PropTransactionWorker::addToBuffer */

private:
    unsigned int maxTrial;/**< @see load. */
    unsigned int sleepTime;/**< @see load. */
};

#endif // DBPROP_H
