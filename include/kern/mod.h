#ifndef ROOT_MOD_H
#define ROOT_MOD_H

/**
 * ROOT_MOD_INIT(NAME) - module init, requires ROOT_MOD_FINI()
 * ROOT_MOD_DEP(DEP) - adds dependency, valid only before ROOT_MOD_FINI()
 * ROOT_MOD_FINI() - module fini
 */

typedef struct
{
  const char *name;
  const char *deps;
  void (*init) (void);
  void (*fini) (void);
} __attribute__ ((packed)) root_module_t;

#if defined(ROOT_MODULE)
static const __attribute ((section (".rootmod.deps"), used,
                           aligned (1))) char root_moddeps_start;

#define ROOT_MOD_INIT(NAME)                                                   \
  static void __attribute__ ((used)) root_mod_init (void);                    \
  static void __attribute__ ((used)) root_mod_fini (void);                    \
  static const char                                                           \
      __attribute__ ((section (".rootmod.name"), used)) modname[]             \
      = #NAME;                                                                \
  static root_module_t __attribute__ ((section (".rootmod.desc"), used)) mod  \
      = {                                                                     \
          .name = modname,                                                    \
          .deps = &root_moddeps_start + 1,                                    \
          .init = root_mod_init,                                              \
          .fini = root_mod_fini,                                              \
        };                                                                    \
  static void root_mod_init (void)

#define ROOT_MOD_DEP(DEP)                                                     \
  static const char __attribute__ ((section (".rootmod.deps"), used,          \
                                    aligned (1))) moddep_##DEP[]              \
      = #DEP;

#define ROOT_MOD_FINI()                                                       \
  static const                                                                \
      __attribute__ ((section (".rootmod.deps"), used)) char root_moddeps_end \
      = 0;                                                                    \
  static void root_mod_fini (void)

#endif

void root_initmods (void);

#endif
