#ifndef __ENVVAR_H__
	#define __ENVVAR_H__
	
	// #define EXCLUDE_SAL // If you wish to not use SAL warnings.
	
	#define RDA_PAGE_SIZE 128

	// Do NOT modify anything below this line.
	
	#ifdef _WIN64
		#define PTRSIZE 8
	#else
		#define PTRSIZE 4
	#endif // _WIN64

	#define IN_RANGE(n, lo, hi) ((lo <= n) && (n <= hi))
	#define ARRAY_LENGTH(_Array) (sizeof(_Array) / sizeof(_Array[0]))
	
	#define IS_LOWERCASE(c) IN_RANGE(c, 'a', 'z')
	#define IS_UPPERCASE(c) IN_RANGE(c, 'A', 'Z')
	#define IS_LETTER(c) (IS_LOWERCASE(c) || IS_UPPERCASE(c))
	#define IS_DIGIT(c) IN_RANGE(c, '0', '9')
	
	#define TO_LOWERCASE(c) (c + '\x20')
	#define TO_UPPERCASE(c) (c - '\x20')
	
	#ifdef EXCLUDE_SAL
		#undef _In_
		#define _In_
		
		#undef _In_opt_
		#define _In_opt_
		
		#undef _Out_
		#define _Out_
		
		#undef _Out_opt_
		#define _Out_opt_
		
		#undef _Out_writes_all_
		#define _Out_writes_all_(size)
	#else
		#include <sal.h>
	#endif

#endif // !__ENVVAR_H__