#ifndef ROOT_MOD_H
#define ROOT_MOD_H

/**
 * ROOT_MOD_INIT(NAME) - module init, requires ROOT_MOD_FINI()
 * ROOT_MOD_DEP(DEP) - adds module dep
 * ROOT_MOD_FINI() - module fini
 */

typedef struct
{
  const char *name;
  const char *deps;
  void (*init) (void);
  void (*fini) (void);
  unsigned char loaded;
} root_initmod_t;

#if defined(ROOT_MODULE)

static const __attribute ((section (".rootmod.deps"), used,
                           aligned (1))) char root_moddeps_start;

#define ROOT_MOD_INIT(NAME)                                                   \
  static void __attribute__ ((used)) root_mod_init (void);                    \
  static void __attribute__ ((used)) _root_mod_init (void);                   \
  static void __attribute__ ((used)) root_mod_fini (void);                    \
  static void __attribute__ ((used)) _root_mod_fini (void);                   \
  static const char                                                           \
      __attribute__ ((section (".rootmod.name"), used)) modname[]             \
      = #NAME;                                                                \
  root_initmod_t                                                              \
      __attribute__ ((section (".rootmod.desc"), used)) root_##NAME##_initmod \
      = {                                                                     \
          .name = modname,                                                    \
          .deps = &root_moddeps_start + 1,                                    \
          .init = _root_mod_init,                                             \
          .fini = _root_mod_fini,                                             \
        };                                                                    \
  static void _root_mod_init (void)                                           \
  {                                                                           \
    root_##NAME##_initmod.loaded = 1;                                         \
    root_mod_init ();                                                         \
  }                                                                           \
  static void root_mod_init (void)

#define ROOT_MOD_DEP(DEP)                                                     \
  static const char __attribute__ ((section (".rootmod.deps"), used,          \
                                    aligned (1))) moddep_##DEP[]              \
      = #DEP;

#define ROOT_MOD_FINI(NAME)                                                   \
  static void _root_mod_fini (void)                                           \
  {                                                                           \
    root_##NAME##_initmod.loaded = 0;                                         \
    root_mod_fini ();                                                         \
  }                                                                           \
  static void root_mod_fini (void)

#endif

void root_preload_initmod (root_initmod_t *initmod);
void root_load_initmods (void);

#endif
