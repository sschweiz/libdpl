#include <dpl.h>

int main(int argc, char **argv)
{
	dpl_t d;
	
	if (dpl_get_rank(&d) == DPL_RANK_MASTER)
	{
		dpl_error(&d, DPL_ERROR,
			"uh oh something happened here %s", argv[0]);
	}
}
