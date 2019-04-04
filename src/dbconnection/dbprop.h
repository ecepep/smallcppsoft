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
 * @brief The DBProp class
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
    std::shared_ptr<PropTransactionBuffer> buffer;
    PropTransactionWorker transactionWorker;
    QThread transactionThread;

signals:
    void requireUpdate() const;
    void requireFetch();

private:
    unsigned int maxTrial;
    unsigned int sleepTime;
};

#endif // DBPROP_H
