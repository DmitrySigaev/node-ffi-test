/*
Copyright (C) 2016 Dmitry Sigaev

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/


#ifndef _LAL_REPORT_H_
#define _LAL_REPORT_H_

#include <stdint.h>

#define report_error(x) printf("lal-report {error}:  %s f:(%s) l:(%d)\n", x, __FILE__, __LINE__)
#define report_warning(x) printf("lal-report {warning}: %s f:(%s) l:(%d)\n", x, __FILE__, __LINE__)

#endif /* _LAL_REPORT_H_ */