#include "stdafx.h"

namespace BinaryData
{
 
//================== hook.lua ==================
static const unsigned char hook_script[] =
	"-- hook\r\n"
	"function hookfunc(event,line)\r\n"
	"  local s1 = debug.getinfo(2).source;\r\n"
	"  local s2 = debug.getinfo(2).short_src;\r\n"
	"  CHook(s1,s2,line);\r\n"
	"end\r\n"
	"function printlocal(valname)\r\n"
	"  local i = 1;\r\n"
	"  repeat\r\n"
	"    local name,value = debug.getlocal(4,i);\r\n"
	"    if name == nil then\r\n"
	"      print(\"变量不存在\");\r\n"
	"      return;\r\n"
	"    end\r\n"
	"    if name == valname then\r\n"
	"      print(name .. \":\" .. value);\r\n"
	"      return;\r\n"
	"    end\r\n"
	"    i = i + 1;\r\n"
	"  until false\r\n"
	"end\r\n"
	"print(\"hook register success!\");\r\n"
    "debug.sethook(hookfunc,\"l\");\r\n"; // 执行一行代码，中断一次
	 

const char* hook = (const char*) hook_script;

 
}
