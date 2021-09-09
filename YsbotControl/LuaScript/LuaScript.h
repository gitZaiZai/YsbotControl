#ifndef _LuaScript_h
#define _LuaScript_h

#include "LuaState.h"
#include "ScriptCommandInfo.h"
  

class LuaScript
{
public:
	~LuaScript(void);
	static LuaScript* getInstance();

	void addVariableToState(const int, const char*, void* );
 
	void addFunctionToState(void );

	void registerClassToLua(void);

	void print(std::string s) { m_luaState->print(s); }
	int  doString(std::string sc) { return m_luaState->doString(sc); }
	int  load_string(std::string sc) { return m_luaState->load_string(sc); }
	int  luaExecute(int npara=0,int nret=0) { return m_luaState->pcall(npara,nret); }
	void free_lua(void) { m_luaState->free_lua(); }

	lua_State* getState();
	void debug_local_var();

	static int get_int_variable(int*);
	static double get_real_variable(double*);
	static std::string get_string_variable(const char*);
	ScriptCommandInfo* get_script_command_info() { return command_info; }

protected:
	LuaScript(void);
	static LuaScript* singleton;

private:

	LuaState* m_luaState;
	ScriptCommandInfo* command_info;

	class LuaCleaner
	{
	public:
		LuaCleaner() {}
		~LuaCleaner()
		{
			if ( LuaScript::getInstance() )
			{
				delete LuaScript::getInstance();
			}
		}
	};

};

#define LSCRIPT LuaScript::getInstance()
#define LSCINFO LuaScript::getInstance()->get_script_command_info()

#endif
