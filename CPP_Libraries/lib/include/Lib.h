//==================================================>
// Generic Libraries.								|
//--------------------------------------------------|
// Author: Sigma (https://github.com/Sigma0314)		|
//--------------------------------------------------|
// Notes ->											|
//													|
// GetSystemDateTime format specifiers:				|
//													|
// %i : Milliseconds.								|
// %s : Seconds.									|
// %m : Minutes.									|
// %h : Hours.										|
// %d : Days.										|
// %w : Weekday.									|
// %t : Weekday (Text).								|
// %o : Months.										|
// %y : Years.										|
//													|
// ScanPattern code style mask:						|
//													|
// ? : Wildcard.									|
// x : Full byte.									|
// h : High nibble. (Left nibble.)					|
// l : Low nibble. (Right nibble.)					|
//													|
//==================================================>

#include "EnvVar.h"

#include "Collections.h"
#include "ConsoleEx.h"
#include "Const.h"
#include "Error.h"
#include "ExtMem.h"
#include "Key.h"
#include "Logging.h"
#include "Maths.h"
#include "Mem.h"
#include "MemShared.h"
#include "Misc.h"
#include "Pipes.h"