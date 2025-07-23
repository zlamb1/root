#ifndef ROOT_ENV_H
#define ROOT_ENV_H

typedef struct root_env_var_t
{
  const char *name;
  char *value;
  struct root_env_var_t *next;
} root_env_var_t;

root_env_var_t *root_iterate_env_vars (root_env_var_t *var);

root_env_var_t *root_get_env_var (const char *name);

void root_unset_env_var (const char *name);
root_env_var_t *root_set_env_var (const char *name, char *value);

void root_unset_all_env_vars (void);

#endif
