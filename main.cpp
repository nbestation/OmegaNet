#include <stdlib.h>
#include <stdio.h>
#include "Config.h"
#include "Switch.h"

const int omega_map[NUM_OF_NODE] = { 0, 2, 4, 6, 1, 3, 5, 7 };

int GetBase(int num, int base)
{
	return ((num >> base) % 2);
}

void PrintStatus(int st)
{
	switch (st)
	{
	case NO_CONNECTION: printf("no connection"); break;
	case UP_CONNECTION: printf("up connection"); break;
	case DOWN_CONNECTION: printf("down connection"); break;
	case BOTH_CONNECTION: printf("both connection"); break;
	default: printf("error");
	}
}

void Mapping(FILE* infile, int* func)
{
	char c;
	int temp_result[NUM_OF_NODE];
	int ptr = 0, base = 0;
	while ((c = fgetc(infile)) != EOF)
	{
		if ((c >= '0') && (c <= '9'))
		{
			temp_result[ptr] = int(c) - int('0');
			ptr++;
		}
		else if (c == ')')
		{
			for (int i = 0; i < ptr; i++)
			{
				func[temp_result[i]] = temp_result[(i+1)%ptr];
			}
			base = base + ptr;
			ptr = 0;
		}
	}
}

void Connect(const int* map, class Switch* sw)
{
	for (int src = 0; src < NUM_OF_NODE; src++)
	{
		int dst = map[src];
		int dst_ptr = src;
		for (int lv = 0; lv < NUM_OF_LEVEL; lv++)
		{
			dst_ptr = omega_map[dst_ptr];
			int id = dst_ptr / 2;
			int base = GetBase(dst, NUM_OF_LEVEL - lv - 1);
			if ((dst_ptr % 2) == 0)
			{
				if (base == 0)
				{
					if ((sw[lv * NUM_OF_SWITCH + id].upper_ == UP_CONNECTION) || (sw[lv * NUM_OF_SWITCH + id].upper_ == NO_CONNECTION))
					{
						sw[lv * NUM_OF_SWITCH + id].upper_ = UP_CONNECTION;
					}
					else
					{
						sw[lv * NUM_OF_SWITCH + id].upper_ = BOTH_CONNECTION;
						sw[lv * NUM_OF_SWITCH + id].blocked_ = true;
					}
				}
				else
				{
					if ((sw[lv * NUM_OF_SWITCH + id].upper_ == DOWN_CONNECTION) || (sw[lv * NUM_OF_SWITCH + id].upper_ == NO_CONNECTION))
					{
						sw[lv * NUM_OF_SWITCH + id].upper_ = DOWN_CONNECTION;
					}
					else
					{
						sw[lv * NUM_OF_SWITCH + id].upper_ = BOTH_CONNECTION;
						sw[lv * NUM_OF_SWITCH + id].blocked_ = true;
					}
					dst_ptr++;
				}
			}
			else
			{
				if (base == 0)
				{
					if ((sw[lv * NUM_OF_SWITCH + id].lower_ == UP_CONNECTION) || (sw[lv * NUM_OF_SWITCH + id].lower_ == NO_CONNECTION))
					{
						sw[lv * NUM_OF_SWITCH + id].lower_ = UP_CONNECTION;
					}
					else
					{
						sw[lv * NUM_OF_SWITCH + id].lower_ = BOTH_CONNECTION;
						sw[lv * NUM_OF_SWITCH + id].blocked_ = true;
					}
					dst_ptr--;
				}
				else
				{
					if ((sw[lv * NUM_OF_SWITCH + id].lower_ == DOWN_CONNECTION) || (sw[lv * NUM_OF_SWITCH + id].lower_ == NO_CONNECTION))
					{
						sw[lv * NUM_OF_SWITCH + id].lower_ = DOWN_CONNECTION;
					}
					else
					{
						sw[lv * NUM_OF_SWITCH + id].lower_ = BOTH_CONNECTION;
						sw[lv * NUM_OF_SWITCH + id].blocked_ = true;
					}
				}
			}
		}
	}
}

void Judge(FILE* outfile, class Switch* sw)
{
	fprintf(outfile, "%% This is the log file\n");
	fprintf(outfile, "%% 0: No connection\n");
	fprintf(outfile, "%% 1: Up connection\n");
	fprintf(outfile, "%% 2: Down connection\n");
	fprintf(outfile, "%% 3: Both connection\n");
	fprintf(outfile, "\n");
	fprintf(outfile, "\t\tupper\tlower\tstatus\n");

	bool blocked = false;
	for (int lv = 0; lv < NUM_OF_LEVEL; lv++)
	{
		for (int id = 0; id < NUM_OF_SWITCH; id++)
		{
			fprintf(outfile, "Switch[%d][%d]:\t%d\t%d", lv, id, sw[lv * NUM_OF_SWITCH + id].upper_, sw[lv * NUM_OF_SWITCH + id].lower_);
			if (sw[lv * NUM_OF_SWITCH + id].blocked_)
			{
				blocked = true;
				printf("Blocked! Switch[%d][%d]: upper:", lv, id);
				PrintStatus(sw[lv * NUM_OF_SWITCH + id].upper_);
				printf("\tlower:");
				PrintStatus(sw[lv * NUM_OF_SWITCH + id].lower_);
				printf("\n");
			}
			else if (sw[lv * NUM_OF_SWITCH + id].upper_ == sw[lv * 4 + id].lower_)
			{
				blocked = true;
				sw[lv * NUM_OF_SWITCH + id].blocked_ = true;
				printf("Blocked! Switch[%d][%d]: upper:", lv, id);
				PrintStatus(sw[lv * NUM_OF_SWITCH + id].upper_);
				printf("\tlower:");
				PrintStatus(sw[lv * NUM_OF_SWITCH + id].lower_);
				printf("\n");
			}
			if (sw[lv * NUM_OF_SWITCH + id].blocked_)
			{
				fprintf(outfile, "\tblocked\n");
			}
			else
			{
				fprintf(outfile, "\tunblocked\n");
			}
		}
	}
	if (!blocked)
	{
		printf("This Omega-Net isn\'t blocked!\n");
	}
}

int main()
{
	FILE* infile = fopen("map.txt", "r");					//input mapping file
	FILE* outfile = fopen("log.txt", "w");					//output log file
	int map[NUM_OF_NODE];									//map: input --> output
	class Switch sw[NUM_OF_LEVEL * NUM_OF_SWITCH];			//switch status

	Mapping(infile, map);									//mapping from rotated replacement
	Connect(map, sw);										//caculate switch status
	Judge(outfile, sw);										//judge if blocked

	fclose(infile);
	fclose(outfile);

	system("pause");
}

