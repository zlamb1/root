#include "kern/env.h"
#include "kern/errno.h"
#include "kern/malloc.h"
#include "kern/string.h"
#include "kern/types.h"

static root_env_var_t *vars = NULL;

static void
root_free_env_var (root_env_var_t *prev, root_env_var_t *var)
{
  if (prev == NULL)
    vars = var->next;
  else
    prev->next = var->next;
  root_free (var->name);
  root_free (var->value);
  root_free (var);
}

root_env_var_t *
root_iterate_env_vars (root_env_var_t *var)
{
  return var == NULL ? vars : var->next;
}

root_env_var_t *
root_get_env_var (const char *name)
{
  root_env_var_t *v;
  if (name == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return NULL;
    }
  v = vars;
  while (v != NULL)
    {
      int cmp = root_strcmp (name, v->name);
      if (cmp == 0)
        return v;
      else if (cmp > 0)
        break;
      v = v->next;
    }
  root_seterrno (ROOT_ENOENT);
  return NULL;
}

void
root_unset_env_var (const char *name)
{
  root_env_var_t *prev = NULL, *var;
  if (name == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return;
    }
  var = vars;
  while (var != NULL)
    {
      int cmp = root_strcmp (name, var->name);
      if (cmp == 0)
        {
          root_free_env_var (prev, var);
          return;
        }
      else if (cmp > 0)
        break;
      prev = var;
      var = var->next;
    }
  root_seterrno (ROOT_ENOENT);
}

root_env_var_t *
root_set_env_var (const char *name, char *value)
{
  root_env_var_t *p = NULL, *v, *var = NULL;
  if (name == NULL || value == NULL)
    {
      root_seterrno (ROOT_EINVAL);
      return NULL;
    }
  v = vars;
  while (v != NULL)
    {
      int cmp = root_strcmp (name, v->name);
      if (cmp == 0)
        {
          var = v;
          break;
        }
      else if (cmp < 0)
        break;
      p = v;
      v = v->next;
    }
  if (var == NULL)
    {
      var = root_zalloc (sizeof (root_env_var_t));
      if (var == NULL)
        goto fail;
      var->name = root_strdup (name);
      var->value = root_strdup (value);
      if (var->name == NULL || var->value == NULL)
        goto fail;
      if (p == NULL)
        {
          var->next = vars;
          vars = var;
        }
      else
        {
          var->next = p->next;
          p->next = var;
        }
    }
  else
    {
      char *nvalue = root_strdup (value);
      if (nvalue == NULL)
        return NULL;
      root_free (var->value);
      var->value = nvalue;
    }
  return var;
fail:
  if (var == NULL)
    return NULL;
  if (var->name != NULL)
    root_free (var->name);
  if (var->value != NULL)
    root_free (var->value);
  root_free (var);
  return NULL;
}

void
root_unset_all_env_vars (void)
{
  if (vars == NULL)
    {
      root_seterrno (ROOT_ENOENT);
      return;
    }
  while (vars != NULL)
    root_free_env_var (NULL, vars);
}
