#include "sqlite_wrapper.h"

void monju::sqlite::throwExceptionIfNotEquals(int result, int assumed)
{
	if (result != assumed)
		throw SQLiteException(result);
}
