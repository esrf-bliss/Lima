#ifndef MISCUTILS_H
#define MISCUTILS_H

#include <map>
#include <algorithm>

namespace lima
{

#define C_LIST_SIZE(list)	(sizeof(list) / sizeof((list)[0]))
#define C_LIST_END(list)	((list) + C_LIST_SIZE(list))
#define C_LIST_ITERS(list)	list, C_LIST_END(list)

// Map search helpers

template <class V>
class SecondIs
{
 public:
	SecondIs(V v) : m_val(v) 
	{}

	template <class K>
	bool operator () (const std::pair<K, V>& p) 
	{ return (p.second == m_val); }

 private:
	V m_val;
};

template <class M, class V>
typename M::const_iterator FindMapValue(const M& m, V v)
{
	return find_if(m.begin(), m.end(), SecondIs<V>(v));
}

} // namespace lima


#endif // MISCUTILS_H
