#ifndef STUB_UCI_H
#define STUB_UCI_H

#define uci_alloc_context	 uci_alloc_context_orig
#define uci_free_context	 uci_free_context_orig
#define uci_load		 uci_load_orig
#define uci_unload		 uci_unload_orig
#define uci_get_errorstr	 uci_get_errorstr_orig
#define uci_lookup_package	 uci_lookup_package_orig
#define uci_lookup_section	 uci_lookup_section_orig
#define uci_lookup_option	 uci_lookup_option_orig
#define uci_lookup_option_string uci_lookup_option_string_orig
#define uci_lookup_ptr		 uci_lookup_ptr_orig
#define uci_set			 uci_set_orig
#define uci_commit		 uci_commit_orig
#include <uci.h>
#undef uci_alloc_context
#undef uci_free_context
#undef uci_load
#undef uci_unload
#undef uci_get_errorstr
#undef uci_lookup_package
#undef uci_lookup_section
#undef uci_lookup_option_string
#undef uci_lookup_ptr
#undef uci_set
#undef uci_commit
#undef uci_lookup_option

struct uci_context *uci_alloc_context(void);
void uci_free_context(struct uci_context *ctx);
int uci_load(struct uci_context *ctx, const char *name, struct uci_package **package);
int uci_unload(struct uci_context *ctx, struct uci_package *p);
void uci_get_errorstr(struct uci_context *ctx, char **dest, const char *str);
struct uci_package *uci_lookup_package(struct uci_context *ctx, const char *name);
const char *uci_lookup_option_string(struct uci_context *ctx, struct uci_section *s, const char *name);
extern int uci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *str, bool extended);
extern int uci_set(struct uci_context *ctx, struct uci_ptr *ptr);
extern int uci_commit(struct uci_context *ctx, struct uci_package **p, bool overwrite);
struct uci_section *uci_lookup_section(struct uci_context *ctx, struct uci_package *p, const char *name);
struct uci_option *uci_lookup_option(struct uci_context *ctx, struct uci_section *s, const char *name);

#endif // STUB_UCI_H
