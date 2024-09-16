#ifndef PASSWD_SHA512CRYPT_H__
#define PASSWD_SHA512CRYPT_H__

#define CH_ZERO		'\0'

typedef enum {
	HASH_MD5,
	HASH_SHA512,
	HASH_NULL
} hash_type;

int make_sha512_salt(char **salt_p);
char *hash_sha512_with_salt(char *password, char *salt);
char *hash_sha512(char *password);
char *extract_salt(char *hash);
hash_type get_hash_type(char *hash);

#endif /* passwd_sha512crypt.h */
