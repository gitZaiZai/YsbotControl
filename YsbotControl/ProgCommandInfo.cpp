#include "stdafx.h"
#include "ProgCommandInfo.h"

ProgCommandInfo::ProgCommandInfo()
{
	nCmdCount = 0;
	nCurCmd   = 0;
	for (int i = 0; i < MAX_PROG_COMMAND; i++)
	{
		dwCmdList[i] = 0;
		index_line_list[i].command_index = index_line_list[i].line_no = 0;
	}
}

const ProgCommandInfo& ProgCommandInfo::operator=(const ProgCommandInfo& rl)
{
	nCmdCount = rl.nCmdCount;
	nCurCmd   = rl.nCurCmd;
	for (int i = 0; i < MAX_PROG_COMMAND; i++)
	{
		dwCmdList[i] = rl.dwCmdList[i];
		index_line_list[i].command_index = rl.index_line_list[i].command_index;
		index_line_list[i].line_no = rl.index_line_list[i].line_no;
	}
	return *this;
}