#include "chi_lua.h"

#include "chi_log.h"
extern ChiLog&  chi_log;

#include <string>
#include <sstream>
#include <map>


/**Posts a generalized error message indicating that the
 * expected amount of arguments don't match the given amount.*/
void LuaPostArgAmountError(const char* func_name,int expected, int given)
{
  chi_log.Log(LOG_ALLERROR)
  << "Incorrect amount of arguments supplied in "
  << std::string(func_name)
  << " expected " << expected << " arguments "
  << " but " << given << " provided";
  exit(EXIT_FAILURE);
}

/**Checks if the lua variable at the stack location indicated by <arg>
 * is a nil value. Throws an error if it is.*/
void LuaCheckNilValue(const char* func_name, lua_State* L, int arg)
{
  if (lua_isnil(L,arg))
  {
    chi_log.Log(LOG_ALLERROR)
      << "Nil -value supplied in "
      << std::string(func_name)
      << " argument " << arg;
    exit(EXIT_FAILURE);
  }
}

/**Checks if the lua variable at the stack location indicated by <arg>
 * is an actual table. Throws an error if it is not.*/
void LuaCheckTableValue(const char* func_name, lua_State* L, int arg)
{
  if (not lua_istable(L,arg))
  {
    chi_log.Log(LOG_ALLERROR)
      << "Non-table value supplied in "
      << std::string(func_name)
      << " argument " << arg;
    exit(EXIT_FAILURE);
  }
}

/**Gets information about an error state.*/
std::string LuaSourceInfo(lua_State* L, const char* func_name)
{
  lua_Debug err_info;
  lua_getstack(L,1,&err_info);
  lua_getinfo(L, "nSl", &err_info);

  std::stringstream ret_str;
  ret_str << func_name
  << " " << err_info.source
  << " line " << err_info.currentline;

  return ret_str.str();
}

/**Performs the necessary checks and converts a lua-table, formatted
 * as a 1D array (i.e. numerical keys) to a std::vector. If the table
 * is not convertable, an error message is posted.*/
void LuaPopulateVectorFrom1DArray(const char* func_name,
                                  lua_State* L,
                                  int table_arg_index,
                                  std::vector<double>& vec)
{
  LuaCheckTableValue(func_name, L, table_arg_index);

  //=================================== Get the table as map
  std::map<int,double> vec_map;
  lua_pushnil(L);
  while (lua_next(L,table_arg_index) != 0)
  {
    if (not lua_isinteger(L,-2)) goto invalid_table;
    if (not lua_isnumber(L,-1))  goto invalid_table;
    int key = lua_tonumber(L,-2);
    double val = lua_tonumber(L,-1);

    vec_map[key] = val;

    lua_pop(L,1);
  }

  //=================================== Populate vector
  {
    size_t vec_size = vec_map.rbegin()->first;
    vec.clear();
    vec.resize(vec_size, 0.0);

    for (auto v_i : vec_map)
      vec[v_i.first - 1] = v_i.second;

    return;
  }

  invalid_table:
    throw std::invalid_argument("Invalid table used in call to " +
                                 std::string(func_name));
}