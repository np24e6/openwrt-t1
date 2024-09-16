#ifndef STUB_ULOOP_H
#define STUB_ULOOP_H

#define uloop_timeout_set uloop_timeout_set_orig
#define uloop_init	  uloop_init_orig
#define uloop_done	  uloop_done_orig
#define uloop_fd_add	  uloop_fd_add_orig
#define uloop_run_timeout uloop_run_timeout_orig
#define uloop_run	  uloop_run_orig
#include <libubox/uloop.h>
#undef uloop_timeout_set
#undef uloop_init
#undef uloop_done
#undef uloop_fd_add
#undef uloop_run_timeout
#undef uloop_run

int uloop_timeout_set(struct uloop_timeout *timeout, int msecs);
int uloop_init(void);
void uloop_done(void);
int uloop_fd_add(struct uloop_fd *sock, unsigned int flags);
int uloop_run_timeout(int timeout);
int uloop_run(void);

#endif // STUB_ULOOP_H
