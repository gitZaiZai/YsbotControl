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

#ifndef LUABRIDGEDEMO_TESTHOST_HEADER
#define LUABRIDGEDEMO_TESTHOST_HEADER

#include <string>
//#include "../Lua53/lua.hpp"
extern "C" 
{
#include "../LuaBind/lua-5.1.3/src/lua.h"
#include "../LuaBind/lua-5.1.3/src/lauxlib.h"
#include "../LuaBind/lua-5.1.3/src/lualib.h"
}

// struct lua_State;

/**
  Host interface.

  The tests call into a host-provided instance of this interface.
*/
class TestHost
{
protected:
  virtual ~TestHost () { }

public:
  /**
    Called to create a fresh environment for performing a test.

    The standard libraries are automatically opened.
  */
  virtual lua_State* createTestEnvironment () = 0;

  /**
    Called to destroy the environment when it is no longer needed.
  */
  virtual void destroyTestEnvironment (lua_State* L) = 0;

  /**
    Prints output
  */
  virtual void print (std::string text) = 0;
};

#endif
