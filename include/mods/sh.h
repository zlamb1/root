#ifndef ROOT_SH_H
#define ROOT_SH_H 1

void root_shell_task (void);

void __attribute__ ((format (printf, 1, 2)))
root_sh_cmd_error (const char *fmt, ...);

#endif
