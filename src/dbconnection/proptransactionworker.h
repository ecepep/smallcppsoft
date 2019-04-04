#ifndef PROPTRANSACTIONWORKER_H
#define PROPTRANSACTIONWORKER_H

#include <QThread>
#include <QSqlQuery>
#include <QMutex>
#include <deque>
#include "QMapIterator"
#include "QVariant"

#include "dbconnection/dbconnection.h"
#include "GUI/prop.h"
#include "dbconnection/proptransactionbuffer.h"

/**
 * @warning Error in my_thread_global_end(): 1 threads didn't exit --> debug destruction @todo
 * @brief The PropTransactionWorker class
 */
class PropTransactionWorker : public QObject
{
    Q_OBJECT
public:
    explicit PropTransactionWorker(std::shared_ptr<DBConnection> dbco);
    virtual ~PropTransactionWorker();

//signals:
//    void sendBlock(const Block &block);
    void setBuffer(std::shared_ptr<PropTransactionBuffer> buffer);
public slots:
    void init();
    void update();
    void addToBuffer();

protected:
    std::shared_ptr<PropTransactionBuffer> buffer;
    QSqlDatabase db;
    std::shared_ptr<DBConnection> dbco;

    unsigned int iduser;
private:
    void update(Prop const& toUpdate ) const;
    void fetch(QSqlQuery& query, unsigned int const& transactionSize);

    std::deque<unsigned int> idpropCurrentlyLoaded; /**< meant to avoid reload of currently loaded prop (fetched but not yet updated) */

//    /**
//     * for debug
//     * https://stackoverflow.com/questions/5777409/how-to-get-last-prepared-and-executed-query-using-qsqlquery/10641002
//     */
//    static QString getLastExecutedQuery(const QSqlQuery& query)
//    {
//     QString str = query.lastQuery();
//     QMapIterator<QString, QVariant> it(query.boundValues());
//     while (it.hasNext()) {
//      it.next();
//      str.replace(it.key(),it.value().toString());
//     }
//     return str;
//    }
};

#endif // PROPTRANSACTIONWORKER_H
