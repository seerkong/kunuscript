#include <stdio.h>
#include <stdlib.h>
#include "../kon/kon.h"
#include "commander.h"

KonState* kstate;

KonState* InitKonState()
{
    kstate = KON_Init();
    if (kstate == NULL) {
        fprintf(stderr, "initialize konstate failed!\n");
        exit(1);
    }

    return kstate;
}

void ExitFailure(KonState* kstate)
{
    KON_Finish(kstate);
    tb_exit();
}

void ExitSuccess(KonState* kstate)
{
    KON_Finish(kstate);
    tb_exit();
}

static void Repl(command_t *self) {
  printf("start repl: enabled\n");
}

static void EvalFile(command_t *self) {
    printf("eval file: %s\n", self->arg);
    KON_EvalFile(kstate, self->arg);
}

static void PrintHelp(command_t *self)
{
    printf("? help -h\n");
}

int RunMain(int argc, char **argv)
{
    command_t cmd;
    command_init(&cmd, argv[0], "0.0.1");
    command_option(&cmd, "-r", "--repl", "start repl", Repl);
    command_option(&cmd, "-f", "--file <arg>", "script file path arg", EvalFile);
    command_option(&cmd, "-h", "--help [arg]", "help info", PrintHelp);
    command_parse(&cmd, argc, argv);

    // printf("additional args:\n");
    // for (int i = 0; i < cmd.argc; ++i) {
    //     printf("  - '%s'\n", cmd.argv[i]);
    // }

    command_free(&cmd);
    return KON_TRUE;
}

int main(int argc, char const* argv[])
{
    if (!tb_init(tb_null, tb_null)) {
        return NULL;
    }
    KonState* kstate = InitKonState();
    
    if (RunMain(argc, argv)) {
        ExitFailure(kstate);
    }
    else {
        ExitSuccess(kstate);
    }
    
    return 0;
}