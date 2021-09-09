//==============================================================================
/*
  https://github.com/vinniefalco/LuaBridge
  https://github.com/vinniefalco/LuaBridgeDemo
  
  Copyright (C) 2012, Vinnie Falco <vinnie.falco@gmail.com>

  License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//==============================================================================

#ifndef LUABRIDGEDEMO_LUASTATE_HEADER
#define LUABRIDGEDEMO_LUASTATE_HEADER

#include "TestHost.h"


/**
  lua_State wrapper.

  The state hs the standard Lua libraries opened, and an override for print which
  sends the text to observers. There is also a traceback feature installed that
  displays the stack when an error occurs. To get the traceback, use the pcall()
  of this class.
*/

class LuaState : public TestHost
{
public:
 
  static LuaState* New ();
  virtual ~LuaState () = 0;
  
  virtual lua_State* getState () = 0;

  inline operator lua_State* ()  {  return getState ();  }

  virtual int pcall (int numberOfArguments, int numberOfReturnValues) = 0;

  virtual int  doString (std::string chunk) = 0;
  virtual int  load_string(std::string chunk) = 0;
  virtual int  free_lua(void) = 0;

  static int pcall (lua_State* L, int numberOfArguments, int numberOfReturnValues);

};

class LuaStateImp : public LuaState
{
private:
// 	struct State
// 	{
// 		CStringArray lines;
// 	};
// 
// 	State m_state;
	lua_State* L;

public:
	LuaStateImp (); 

	~LuaStateImp ();

	lua_State* getState ();
	static void* getTracebackKey ();

	// traceback function, adapted from lua.c
	// when a runtime error occurs, this will append the call stack to the error message
	//
	static int tracebackFunction (lua_State* L);

	// from luaB_print()
	//
	static int print (lua_State *L);

	lua_State* createEnvironment ();
	
    //----------------------------------------------------------------------------
	//
	// LuaState
	//
	//----------------------------------------------------------------------------
	void print (std::string text);
	int  pcall (int numberOfArguments, int numberOfReturnValues);
	int  doString (std::string text);
	int  load_string(std::string text);
	int  free_lua(void);
	//----------------------------------------------------------------------------
	//
	// TestHost
	//
	//----------------------------------------------------------------------------
	lua_State* createTestEnvironment () 	{ return createEnvironment ();	}
	void destroyTestEnvironment (lua_State* L)	{ 	lua_close (L); 	}

};


#endif
