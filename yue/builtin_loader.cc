// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "yue/builtin_loader.h"

#include <algorithm>
#include <string>
#include <tuple>

#include "yue/api_gui.h"
#include "yue/api_message_loop.h"

namespace yue {

namespace {

// The search table, it must be manually kept in sorted order.
std::tuple<std::string, lua_CFunction> loaders_map[] = {
  std::make_tuple("yue.GUI", luaopen_yue_gui),
  std::make_tuple("yue.MessageLoop", luaopen_yue_message_loop),
};

// Use the first element of tuple as comparing key.
bool TupleCompare(const std::tuple<std::string, lua_CFunction>& element,
                  const std::string& key) {
  return std::get<0>(element) < key;
}

int SearchBuiltin(lua::State* state) {
  DCHECK(std::is_sorted(std::begin(loaders_map), std::end(loaders_map)))
      << "The builtin loaders map must be in sorted order";
  std::string name;
  lua::To(state, 1, &name);
  auto iter = std::lower_bound(std::begin(loaders_map), std::end(loaders_map),
                               name, TupleCompare);
  if (iter == std::end(loaders_map)) {
    lua::PushFormatedString(state, "\n\tno builtin '%s'", name.c_str());
    return 1;
  } else {
    lua::Push(state, lua::CFunction(std::get<1>(*iter)));
    return 1;
  }
}

}  // namespace

void InsertBuiltinModuleLoader(lua::State* state) {
  lua::StackAutoReset reset(state);
  lua_getglobal(state, "package");
  DCHECK_EQ(lua::GetType(state, -1), lua::LuaType::Table)
      << "package should be a table";
  lua::RawGet(state, -1, "searchers");
  DCHECK_EQ(lua::GetType(state, -1), lua::LuaType::Table)
      << "package.searchers should be a table";

  // table.insert(pacakge.searchers, 2, search_builtin)
  int len = static_cast<int>(lua::RawLen(state, -1));
  DCHECK_EQ(len, 4);
  for (int i = len; i >= 2; --i) {
    lua::RawGet(state, -1, i);
    lua_rawseti(state, -2, i + 1);
  }
  lua::RawSet(state, -1, 2, lua::CFunction(&SearchBuiltin));
}

}  // namespace yue