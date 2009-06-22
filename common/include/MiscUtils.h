#ifndef MISCUTILS_H
#define MISCUTILS_H

namespace lima
{

#define C_LIST_SIZE(list)	(sizeof(list) / sizeof((list)[0]))
#define C_LIST_END(list)	((list) + C_LIST_SIZE(list))
#define C_LIST_ITERS(list)	list, C_LIST_END(list)


} // namespace lima


#endif // MISCUTILS_H
