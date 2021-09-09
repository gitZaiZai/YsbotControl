// Copyright 2016, 亚思信息科技有限责任公司
// All right reserved.
//
// Author: Liu Run Dan ( lrdcz@163.com )
//
// Yasi Robot Motion Control System - 亚思机器人运动控制系统

#ifndef ProgCommandInfo_h
#define ProgCommandInfo_h

#define MAX_PROG_COMMAND			(30) // YWT_PMC (1024) ????????????

struct PairLineCmdIndex {
	unsigned long long  command_index;    
	unsigned int line_no;
};

struct ProgCommandInfo 
{
	int		nCmdCount;
	int		nCurCmd;
	DWORD	dwCmdList[MAX_PROG_COMMAND];
	// High-order word: MCCL command index
	// Low-order word: line no. in the editor
	PairLineCmdIndex index_line_list[MAX_PROG_COMMAND];

	ProgCommandInfo ();
 	ProgCommandInfo (const ProgCommandInfo& rl) { operator= (rl); }
 	const ProgCommandInfo& operator= (const ProgCommandInfo& rl);
 
};
 
#endif