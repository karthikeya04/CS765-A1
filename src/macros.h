
#ifndef _SRC_MACROS_H_
#define _SRC_MACROS_H_

#define DISALLOW_COPY_AND_ASSIGN(ClassName) 		\
	ClassName(const ClassName&) = delete; 			\
	ClassName& operator=(const ClassName&) = delete;

#endif // _SRC_MACROS_H_