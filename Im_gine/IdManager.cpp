
#include "IdManager.h"

ID getUID()
{
	static std::atomic<ID> uid{ 0 };
	return ++uid;
}

