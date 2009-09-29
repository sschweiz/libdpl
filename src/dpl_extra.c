#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dpl_extra.h"

int dpl_nchoose2(int n)
{
	/* n(n-1)/2 */
	int ret = (n*n - n) / 2;

	return ret;
}

void dpl_generate_from_hostfile(FILE *in, FILE *out)
{
	/* assume f is open */
	/* formats:
	 *   node1:#		mpich
	 *   node2 slots=#	openmpi
	 *   node3		general
	 */

	char buf[255], line[255];
	char *pch;
	int i, j;
	int lines = 0, stage = 0;
	
	while (fgets(buf, 255, in) != NULL) { lines++; }
	rewind(in);

	char **names = (char **)malloc(sizeof(char *) * lines);
	int *slots = (int *)malloc(sizeof(int *) * lines);
	for (i = 0; i < lines; i++)
	{
		names[i] = (char *)malloc(sizeof(char) * 255);
		slots[i] = 0;
	}

	i = 0;
	while (fgets(buf, 255, in) != NULL)
	{
		stage = 0;
		strcpy(line, buf);

		if (buf[0] != '#')
		{
			pch = strtok(buf, " ,;:=\t\n");
			while (pch != NULL)
			{
				if (stage == 0)
				{
					strcpy(names[i], pch);
					stage++;
				} else if (stage == 1) {
					if (line[strlen(names[i])] == ':')
					{
						slots[i] = atoi(pch);
						stage = 4;
					}
					if (strcmp(pch, "slots") == 0) { stage++; }
				} else if (stage == 2) {
					slots[i] = atoi(pch);
				}
				pch = strtok(NULL, " ,;:=\t\n");
			}
			if (stage == 1) { slots[i] = 1; }
		}
		i++;
	}

	for (i = 0; i < lines; i++)
	{
		for (j = (i + 1); j < lines; j++)
		{
			if (strcmp(names[i], names[j]) == 0)
			{
				slots[i] += slots[j];
				strcpy(names[j], "-");
			}
		}
	}

	for (i = 0; i < lines; i++)
	{
		if (strcmp(names[i], "-") != 0)
		{
			for (j = 0; j < slots[i]; j++)
			{
				if ((i == 0) && (j == 0))
				{
					fprintf(out, "%s root 0\n", names[i]);
				} else {
					fprintf(out, "%s node 0\n", names[i]);
				}
			}
		}
	}

	free(slots);
	for (i = 0; i < lines; i++) { free(names[i]); }
	free(names);
}
