#include "dbconnection/proptransactionbuffer.h"

PropTransactionBuffer::PropTransactionBuffer(unsigned int fetchedMinSize, unsigned int fetchedMaxSize):
    fetchedMinSize(fetchedMinSize),
    fetchedMaxSize(fetchedMaxSize),
    noMoreFetch(false)
{
}
