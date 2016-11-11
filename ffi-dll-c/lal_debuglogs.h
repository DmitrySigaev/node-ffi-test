/*
Copyright (C) 2016

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#ifndef _LAL_DEBUGLOGS_H_
#define _LAL_DEBUGLOGS_H_

#include <stdio.h>

#if defined(_DEBUG)
#define dbg_print printf
#else
#define dbg_print 
#endif 

#endif /* _LAL_DEBUGLOGS_H_ */
