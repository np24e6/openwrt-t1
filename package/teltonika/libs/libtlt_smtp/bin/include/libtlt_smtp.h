
#ifndef __LIBTLT_SMTP_H
#define __LIBTLT_SMTP_H

#include <curl/curl.h>

typedef enum {
    LSMTP_SUCCESS,
    LSMTP_ERROR,
} lsmtp_stat;

struct smtp_ctx {
    char *username;
    char *password;
    char *server;
    char *cert;
    int port;
    int tls;
    char *subject;
    char *sender;
    struct curl_slist *to;
    char *message;

    size_t lines_read;
    CURL *curl;
};

/**
 * @brief sends email messges
 * 
 * @param ctx smtp context
 * @return 
 */
lsmtp_stat lsmtp_init(struct smtp_ctx *ctx);

/**
 * @brief add recipient to SMTP context
 * 
 * @param ctx SMTP context
 * @param recipient email 
 * @retval LSMTP_SUCCESS on success
 * @retval LSMTP_ERROR on error
 */
lsmtp_stat lsmtp_add_recipient(struct smtp_ctx *ctx, const char *recipient);

lsmtp_stat lsmtp_send_mail(struct smtp_ctx *ctx);

void lsmtp_clean(struct smtp_ctx *ctx);

#endif //__LIBTLT_SMTP_H