#pragma once



#include <atomic>

using ID = uint32_t;


ID getUID()
{
	static std::atomic<ID> uid{ 0 };
	return ++uid;
}


//
//template<typename T>
//struct ID
//{
//private:
//	ID(IDValue val)
//	{
//		value = val;
//	}
//
//	IDValue value;
//	
//
//public:
//
//	const IDValue& getID() const
//	{
//		return value;
//	}
//
//	friend bool operator<(const ID& l, const ID& r)
//	{
//		return l.getID()  < r.getID(); // keep the same order
//	}
//
//
//	static ID getUID()
//	{
//		static std::atomic<IDValue> uid{ 0 };
//		return ID(++uid);
//	}
//};


