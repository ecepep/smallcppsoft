#ifndef PROPTRANSACTIONBUFFER_H
#define PROPTRANSACTIONBUFFER_H

#include <qmutex.h>
#include <deque>

#include <GUI/prop.h>

struct PropTransactionBuffer
{
    PropTransactionBuffer(unsigned int fetchedMinSize = 3, unsigned int fetchedMaxSize = 6);

    std::deque<Prop> fetched; /**< buffer for next Prop,queue: pop_front (in this) _ push_back (in transaction worker)  */
    QMutex ftcMutex;
    // @todo google whether const uint need to be specified atomic?
    const std::atomic_uint fetchedMinSize; /**< For now arbitrary, define the size for which the buffer will emit requireFetch  */
    const std::atomic_uint fetchedMaxSize; /**< For now arbitrary, define the max size of a fetch */
    std::atomic_bool noMoreFetch;

    std::deque<Prop> toUpdate; /**< buffer for next Prop,queue: pop_front (in this) _ push_back (in transaction worker)
@note no size, should always update asap. */
    QMutex tupMutex;
};

#endif // PROPTRANSACTIONBUFFER_H
