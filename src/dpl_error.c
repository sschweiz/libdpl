#include "dpl_error.h"
#include <signal.h>
#include <stdio.h>

int dpl_errno;

void dpl_reset_error()
{
	dpl_errno = DPL_SUCCESS;
}

int dpl_throw_error()
{
	return raise(DPL_SIGNAL_ERROR);
}

void dpl_error(int errno, unsigned char critical)
{
#ifdef DPL_REPORT_ERRORS
	dpl_errno = errno;
	
	switch (critical)
	{
	case DPL_THROW: dpl_throw_error(); break;
	case DPL_EXIT: exit(1); break;
	case DPL_ANNOY:
		fprintf(stderr, "dpl_error[%d]: received generated error #%d\n", __LINE__, errno);
		break;
	}
#else
	/* to surpress warnings */
	critical = (unsigned char)errno;
#endif
}
