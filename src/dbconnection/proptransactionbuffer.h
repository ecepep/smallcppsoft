#ifndef PROPTRANSACTIONBUFFER_H
#define PROPTRANSACTIONBUFFER_H

#include <qmutex.h>
#include <deque>

#include <GUI/prop.h>

/**
 * @brief The PropTransactionBuffer struct shared memory for @see DBProp and @see PropTransactionWorker
 * @note buffer (std::deque) must be access safely thanks to QMutex.
 */
struct PropTransactionBuffer
{
    PropTransactionBuffer(unsigned int fetchedMinSize = 3, unsigned int fetchedMaxSize = 6);

    std::deque<Prop> fetched; /**< buffer for next Prop,queue: pop_front _ push_back */
    QMutex ftcMutex;/**< mutex for @see fetched */
    // @todo google whether const uint need to be specified atomic?
    const std::atomic_uint fetchedMinSize; /**< @see DBProp::load For now arbitrary, define the size for which DBProp::requireFetch will be emited   */
    const std::atomic_uint fetchedMaxSize; /**< @see PropTransactionWorker::addToBuffer, max size of fetched */
    std::atomic_bool noMoreFetch; /**< @see true if a sql query for fetching return empty */

    std::deque<Prop> toUpdate; /**< buffer for Prop waiting to be updated. pop_front  _ push_back
@note no size, should always update asap. */
    QMutex tupMutex;/**< mutex for @see toUpdate */
};

#endif // PROPTRANSACTIONBUFFER_H
