#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char** argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  Nob_Cmd cmd = {0};

  nob_cmd_append(&cmd, "mkdir", "-p", "build");
  nob_cmd_run_sync_and_reset(&cmd);
  
  nob_shift_args(&argc, &argv);
  if(argc == 0) {
    nob_log(NOB_ERROR, "no command provided");
    nob_log(NOB_INFO, "usage: ./nob <build|run>");
    return 0;
  }

  if(strcmp(argv[0], "build") == 0) {
    nob_cmd_append(&cmd, "clang");
    nob_cmd_append(&cmd, "src/nutte.c", "-o", "build/nutte");
    nob_cmd_append(&cmd, "-g", "-Wall", "-Wextra", "-DCCORD_SIGINTCATCH");
    nob_cmd_append(&cmd, "-pthread", "-ldiscord", "-lcurl");
    if(!nob_cmd_run_sync(cmd)) return 1;
  } else if(strcmp(argv[0], "run") == 0) {
    nob_cmd_append(&cmd, "build/nutte");
    if(!nob_cmd_run_sync(cmd)) return 1;
  } else {
    nob_log(NOB_ERROR, "invalid command '%s'", argv[0]);
    nob_log(NOB_INFO, "usage: ./nob <build|run>");
  }

  return 0;
}
