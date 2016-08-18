// Copyright 2016 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "base/command_line.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "lua/callback.h"
#include "nativeui/init.h"
#include "nativeui/label.h"
#include "nativeui/window.h"

int main(int argc, const char *argv[]) {
  base::CommandLine::Init(argc, argv);

  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  logging::InitLogging(settings);

  auto* cmd = base::CommandLine::ForCurrentProcess();
  if (cmd->GetArgs().size() != 1) {
    LOG(ERROR) << "Script expected";
    return 1;
  }

  lua::ManagedState state;
  if (!state) {
    LOG(ERROR) << "Failed to create state.";
    return 1;
  }
  luaL_openlibs(state);

  if (luaL_loadfile(state, cmd->GetArgs()[0].c_str()) != LUA_OK ||
      !lua::PCall(state, nullptr)) {
    std::string error;
    lua::Pop(state, &error);
    LOG(ERROR) << "Error run script: " << error;
    return 1;
  }

  nu::Initialize();
  nu::Label label;
  label.SetText("test");
  nu::Window::Options options = { gfx::Rect(400, 400, 100, 100) };
  nu::Window window(options);
  window.SetContentView(&label);
  window.SetVisible(true);

  base::MessageLoop message_loop(base::MessageLoop::TYPE_UI);
  base::RunLoop().Run();

  return 0;
}