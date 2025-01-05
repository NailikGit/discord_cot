#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char** argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "clang");
  nob_cmd_append(&cmd, "src/nutte.c", "-o", "build/nutte");
  nob_cmd_append(&cmd, "-g", "-Wall", "-Wextra", "-DCCORD_SIGINTCATCH");
  nob_cmd_append(&cmd, "-pthread", "-ldiscord", "-lcurl");
  if(!nob_cmd_run_sync_and_reset(&cmd)) return 1;

  nob_cmd_append(&cmd, "build/ping_pong");
  if(!nob_cmd_run_sync(cmd)) return 1;

  return 0;
}
