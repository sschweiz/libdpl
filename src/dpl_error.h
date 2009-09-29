#ifndef __DPL_ERROR__
#define __DPL_ERROR__

#include "dpl_base.h"

#define DPL_IGNORE	0
#define DPL_THROW		1
#define DPL_EXIT		2
#define DPL_ANNOY		4

#ifndef DPL_DEFAULT_LEVEL
#define DPL_DEFAULT_LEVEL	DPL_IGNORE
#endif



#define DPL_SUCCESS	0

/* memory */
#define DPL_ERROR_MALLOC		0x00a0

/* dpl_stack_t */
#define DPL_ERROR_STACKOVERFLOW		0x0b01
#define DPL_ERROR_STACKEMPTY		0x0b02

/* dpl_list_t */
#define DPL_ERROR_LISTNOTREADY	0x0c01
#define DPL_ERROR_LISTNOKEY		0x0c02


//#define DPL_ERROR_


/** 
   Errors which can cause harmful consequences if not
   attended to will throw their error by raising
   DPL_SIGNAL_ERROR.

   To catch for this signal:

   void error_handler(int signum)
   {
   	switch (dpl_errno)
	{
	...
	}
   }
   signal(DPL_SIGNAL_ERROR, error_handler);
*/
#define DPL_SIGNAL_ERROR		40
extern int dpl_errno;

DPL_BASE_FUNC(reset_error,void,__none)();
DPL_BASE_FUNC(throw_error,int,__none)();
DPL_BASE_FUNC(error,void,__none)(int errno, unsigned char critical);

#endif
