#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS	/* Disable deprecation warning in VS2005 */

#ifdef _WIN32_WCE
#include "appce.h"
#else /* _WIN32_WCE */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#define	_WIN32_WINNT	0x0400	/* To make it link in VS2005 */
#include <windows.h>
#include <process.h>
#include <direct.h>
#include <io.h>

#if !defined(__MINGW32__)
#pragma comment(lib, "ws2_32.lib")
#endif /* __MINGW32__ */

#endif /* _WIN32_WCE */

/*
 * Visual Studio 6 does not know __func__ or __FUNCTION__
 * The rest of MS compilers use __FUNCTION__, not C99 __func__
 */
#if defined(__BORLANDC__) || (defined(_MSC_VER) && _MSC_VER < 1300)
#define	_VAR2STR(x)		#x
#define	 VAR2STR(x)		_VAR2STR(x)
#define	__func__		"Line #" VAR2STR(__LINE__)
#else
#define	__func__		__FUNCTION__
#endif /* _MSC_VER */

#ifdef __BORLANDC__
#pragma warn -par	/* -W8057: parameter 'foo' is never used */
#pragma warn -aus	/* -W8004: 'foo' is assigned a value that is never used */
#endif /* __BORLANDC__ */

#define	ERRNO			GetLastError()
#define	NO_SOCKLEN_T
#define	DIRSEP			'\\'
#define	IS_DIRSEP_CHAR(c)	((c) == '/' || (c) == '\\')
#define	O_NONBLOCK		0
#define	_POSIX_

#define	SHUT_WR			1
#define	snprintf		_snprintf
#define	vsnprintf		_vsnprintf
#define	sleep(x)		Sleep((x) * 1000)

#define	popen(x, y)		_popen(x, y)
#define	pclose(x)		_pclose(x)
#define	close(x)		_close(x)

#define	fseeko(x, y, z)		fseek((x), (y), (z))
#define	write(x, y, z)		_write((x), (y), (unsigned) z)
#define	read(x, y, z)		_read((x), (y), (unsigned) z)
#define	flockfile(x)		(void) 0
#define	funlockfile(x)		(void) 0

#if !defined(fileno)
#define	fileno(x)		_fileno(x)
#endif /* !fileno MINGW #defines fileno */

#define pid_t HANDLE
#define pthread_self() GetCurrentThreadId()

typedef HANDLE pthread_mutex_t;
typedef HANDLE pthread_cond_t;
typedef DWORD pthread_t;

struct timespec {
	long tv_nsec;
	long tv_sec;
};

static int pthread_mutex_lock(pthread_mutex_t *);
static int pthread_mutex_unlock(pthread_mutex_t *);

#ifndef _STDINT_H
#if defined(HAVE_STDINT)
#include <stdint.h>
#else
typedef unsigned int		uint32_t;
typedef unsigned short		uint16_t;
#endif /* HAVE_STDINT */
#endif /* _STDINT_H */

/*
 * POSIX dirent interface
 */
struct dirent {
	char	d_name[FILENAME_MAX];
};

typedef struct DIR {
	HANDLE			handle;
	WIN32_FIND_DATAW	info;
	struct dirent		result;
} DIR;

#else				/* UNIX  specific	*/
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <pwd.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <pthread.h>

#define	DIRSEP			'/'
#define	IS_DIRSEP_CHAR(c)	((c) == '/')
#define	O_BINARY		0
#define	closesocket(a)		close(a)
#define	mg_fopen(x, y)		fopen(x, y)
#define	mg_mkdir(x, y)		mkdir(x, y)
#define	mg_remove(x)		remove(x)
#define	mg_rename(x, y)		rename(x, y)
#define	mg_getcwd(x, y)		getcwd(x, y)
#define	ERRNO			errno
#define	INVALID_SOCKET		(-1)
typedef int SOCKET;

#endif /* End of Windows and UNIX specific includes */


#if (!defined(DEBUG) && !defined(_DEBUG) && !defined(NDEBUG))
#define NDEBUG
#endif /* some compilers do not define NDEBUG automatically */
#include <assert.h>

#include "appws.h"

#define	PASSWORDS_FILE_NAME	".htpasswd"
#define	CGI_ENVIRONMENT_SIZE	4096
#define	MAX_CGI_ENVIR_VARS	64
#define	MAX_REQUEST_SIZE	8192
#define	MAX_LISTENING_SOCKETS	10
#define	MAX_CALLBACKS		20
#define	ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))
#define	UNKNOWN_CONTENT_LENGTH	((uint32_t) ~0)
#define	DEBUG_MGS_PREFIX	"[DEBUG] "

#if defined(DEBUG) || defined(_DEBUG)
#define	DEBUG_TRACE(x) do {printf x; putchar('\n'); fflush(stdout);} while (0)
#else
#define DEBUG_TRACE(x)
#endif /* DEBUG */

/*
 * Darwin prior to 7.0 and Win32 do not have socklen_t
 */
#ifdef NO_SOCKLEN_T
typedef int socklen_t;
#endif /* NO_SOCKLEN_T */

#if !defined(FALSE)
enum {FALSE, TRUE};
#endif /* !FALSE */

typedef int bool_t;
typedef void * (*mg_thread_func_t)(void *);

static const char *http_500_error = "Internal Server Error";

/*
 * Month names
 */
static const char *month_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/*
 * Unified socket address. For IPv6 support, add IPv6 address structure
 * in the union u.
 */
struct usa_t {
	socklen_t len;
	union {
		struct sockaddr	sa;
		struct sockaddr_in sin;
	} u;
};

/*
 * Specifies a string (chunk of memory).
 * Used to traverse comma separated lists of options.
 */
struct vec_t {
	const char	*ptr;
	size_t		len;
};

/*
 * Structure used by mg_stat() function. Uses 32 bit file length.
 */
struct stat_t {
	bool_t		is_directory;	/* Directory marker		*/
	uint32_t	size;		/* File size			*/
	time_t		mtime;		/* Modification time		*/
};

struct option_t {
	const char	*name;
	const char	*description;
	const char	*default_value;
	int		index;
	bool_t (*setter)(struct aws_context_t *, const char *);
};

/*
 * Numeric indexes for the option values in context, ctx->options
 */
enum OPTION_INDEX_T {
	OPT_ROOT, OPT_INDEX_FILES, OPT_PORTS, OPT_DIR_LIST, OPT_CGI_EXTENSIONS,
	OPT_CGI_INTERPRETER, OPT_CGI_ENV, OPT_SSI_EXTENSIONS, OPT_AUTH_DOMAIN,
	OPT_AUTH_GPASSWD, OPT_AUTH_PUT, OPT_ACCESS_LOG, OPT_ERROR_LOG,
	OPT_ALIASES, OPT_ACL, OPT_UID, OPT_PROTECT,
	OPT_SERVICE, OPT_HIDE, OPT_ADMIN_URI, OPT_MAX_THREADS, OPT_IDLE_TIME,
	OPT_MIME_TYPES, NUM_OPTIONS
};

/*
 * Structure used to describe listening socket, or socket which was
 * accept()-ed by the master thread and queued for future handling
 * by the worker thread.
 */
struct socket_t {
	SOCKET			sock;		/* Listening socket		*/
	struct usa_t	lsa;		/* Local socket address		*/
	struct usa_t	rsa;		/* Remote socket address	*/
};

/*
 * Callback function, and where it is bound to
 */
struct callback_t {
	char		*uri_regex;	/* URI regex to handle		*/
	aws_callback_t	func;		/* user callback		*/
	int		status_code;	/* error code to handle		*/
	void		*user_data;	/* opaque user data		*/
};

/*
 * Mongoose context
 */
struct aws_context_t {
	int		stop_flag;	/* Should we stop event loop	*/

	FILE		*access_log;	/* Opened access log		*/
	FILE		*error_log;	/* Opened error log		*/

	struct socket_t	listeners[MAX_LISTENING_SOCKETS];
	int		num_listeners;

	struct callback_t	callbacks[MAX_CALLBACKS];
	int		num_callbacks;

	char		*options[NUM_OPTIONS];	/* Configured opions	*/
	pthread_mutex_t	opt_mutex[NUM_OPTIONS];	/* Option protector	*/

	int		max_threads;	/* Maximum number of threads	*/
	int		num_threads;	/* Number of threads		*/
	int		num_idle;	/* Number of idle threads	*/
	pthread_mutex_t	thr_mutex;	/* Protects (max|num)_threads	*/
	pthread_cond_t	thr_cond;
	pthread_mutex_t	bind_mutex;	/* Protects bind operations	*/

	struct socket_t	queue[20];	/* Accepted sockets		*/
	int		sq_head;	/* Head of the socket queue	*/
	int		sq_tail;	/* Tail of the socket queue	*/
	pthread_cond_t	empty_cond;	/* Socket queue empty condvar	*/
	pthread_cond_t	full_cond;	/* Socket queue full condvar	*/
};

struct field_t {
	char	*name;		/* field name */
	int		name_len;
	char	*fn;		/* file name in data header */
	int		fn_len;
	char	*mime;		/* mime type for the data */
	int		mime_len;
	char	*data;		/* address of data */
	int		data_len;
};

/*
 * Client connection.
 */
struct aws_connect_t {
	struct aws_request_t  request_info;
	int			is_form_data;		/* flag of form-data */
	int			num_fields;			/* Number of fields in form-data */
	struct field_t form_fields[64];	/* Maximum 64 fields */
	char		*uri_decoded;		/* decoded version of request_info.uri */
	struct aws_context_t *ctx;		/* Mongoose context we belong to*/
	struct socket_t	client;		/* Connected client		*/
	time_t		birth_time;	/* Time connection was accepted	*/
	bool_t		free_post_data;	/* post_data was malloc-ed	*/
	uint32_t	num_bytes_sent;	/* Total bytes sent to client	*/
};

/*
 * this function is used for logging. It prints stuff to the 
 * conn->error_log, which is stderr unless "error_log" option was set.
 */
static void
builtin_errors(struct aws_context_t *ctx, struct socket_t *client, 
		const struct aws_request_t *request_info, void *message)
{
	FILE	*fp;
	time_t	timestamp;

	fp = ctx->error_log;
	flockfile(fp);

	timestamp = time(NULL);

	if (client != NULL)
		(void) fprintf(fp,
		    "[%010lu] [error] [client %s] ",
		    (unsigned long) timestamp,
		    inet_ntoa(client->rsa.u.sin.sin_addr));

	if (request_info != NULL && request_info->request_method != NULL)
		(void) fprintf(fp, "%s %s: ",
		    request_info->request_method,
		    request_info->uri);

	(void) fprintf(fp, "%s", (char *) message);

	fputc('\n', fp);

	funlockfile(fp);
}

/*
 * Print error message to the opened error log stream.
 */
static void
cry_con(struct aws_connect_t *conn, const char *fmt, ...)
{
	char	buf[BUFSIZ];
	va_list	ap;

	va_start(ap, fmt);
	(void) vsnprintf(buf, sizeof(buf), fmt, ap);
	builtin_errors(conn->ctx, &conn->client, &conn->request_info, buf);
	va_end(ap);
}

static void
cry_ctx(struct aws_context_t *ctx, const char *fmt, ...)
{
	char	buf[BUFSIZ];
	va_list	ap;

	va_start(ap, fmt);
	(void) vsnprintf(buf, sizeof(buf), fmt, ap);
	builtin_errors(ctx, NULL, NULL, buf);
	va_end(ap);
}


static void
mg_strlcpy(register char *dst, register const char *src, size_t n)
{
	for (; *src != '\0' && n > 1; n--)
		*dst++ = *src++;
	*dst = '\0';
}

static int
lowercase(const char *s)
{
	return (tolower(* (unsigned char *) s));
}

static int
mg_strncasecmp(const char *s1, const char *s2, size_t len)
{
	int	diff = 0;

	if (len > 0)
		do {
			diff = lowercase(s1++) - lowercase(s2++);
		} while (diff == 0 && s1[-1] != '\0' && --len > 0);

	return (diff);
}

static int
mg_strcasecmp(const char *s1, const char *s2)
{
	int	diff;

	do {
		diff = lowercase(s1++) - lowercase(s2++);
	} while (diff == 0 && s1[-1] != '\0');

	return (diff);
}

static char *
mg_strndup(const char *ptr, size_t len)
{
	char	*p;

	if ((p = (char *) malloc(len + 1)) != NULL)
		mg_strlcpy(p, ptr, len + 1);

	return (p);

}

static char *
mg_strdup(const char *str)
{
	return (mg_strndup(str, strlen(str)));
}


static char *
mg_strnchr(char *src, int c, int n)
{
	for (; n>0; n--,src++) {
		if (*src == c) return src; 
		if (*src == '\0') return (NULL);
	}
	return NULL;
}

/*
 * Find the first occurrence of string "sub" in string "src", where 
 * the search is limited to the first "n" characters of "src".
 */
static char *
mg_strnstr(const char *src, const char *sub, size_t n)
{
	char c, sc;
	size_t len;

	if ((c = *sub++) != '\0') {
		len = strlen(sub);
		do {
			do {
				if (n-- < 1 || (sc = *src++) == '\0')
					return (NULL);
			} while (sc != c);
			
			if (len > n) return (NULL);
			
		} while (strncmp(src, sub, len) != 0);
		src--;
	}
	return ((char *)src);
}


/*
 * Like snprintf(), but never returns negative value, or the value
 * that is larger than a supplied buffer.
 * Thanks to Adam Zeldis to pointing snprintf()-caused vulnerability
 * in his audit report.
 */
static int
mg_vsnprintf(struct aws_connect_t *conn,
		char *buf, size_t buflen, const char *fmt, va_list ap)
{
	int	n;

	if (buflen == 0)
		return (0);

	n = vsnprintf(buf, buflen, fmt, ap);

	if (n < 0) {
		cry_con(conn, "vsnprintf error");
		n = 0;
	} else if (n >= (int) buflen) {
		cry_con(conn, "truncating vsnprintf buffer: [%.*s]",
		    n > 200 ? 200 : n, buf);
		n = (int) buflen - 1;
	}
	buf[n] = '\0';

	return (n);
}

static int
mg_snprintf(struct aws_connect_t *conn,
		char *buf, size_t buflen, const char *fmt, ...)
{
	va_list	ap;
	int	n;

	va_start(ap, fmt);
	n = mg_vsnprintf(conn, buf, buflen, fmt, ap);
	va_end(ap);

	return (n);
}

/*
 * Convert string representing a boolean value to a boolean value
 */
static bool_t
is_true(const char *str)
{
	static const char	*trues[] = {"1", "yes", "true", "ja", NULL};
	int			i;

	for (i = 0; trues[i] != NULL; i++)
		if (str != NULL && mg_strcasecmp(str, trues[i]) == 0)
			return (TRUE);

	return (FALSE);
}

/*
 * Skip the characters until one of the delimiters characters found.
 * 0-terminate resulting word. Skip the rest of the delimiters if any.
 * Advance pointer to buffer to the next word. Return found 0-terminated word.
 */
static char *
skip(char **buf, const char *delimiters)
{
	char	*p, *begin_word, *end_word, *end_delimiters;

	begin_word = *buf;
	end_word = begin_word + strcspn(begin_word, delimiters);
	end_delimiters = end_word + strspn(end_word, delimiters);

	for (p = end_word; p < end_delimiters; p++)
		*p = '\0';

	*buf = end_delimiters;

	return (begin_word);
}

/*
 * Return HTTP header value, or NULL if not found.
 */
static const char *
get_header(const struct aws_request_t *ri, const char *name)
{
	int	i;

	for (i = 0; i < ri->num_headers; i++)
		if (!mg_strcasecmp(name, ri->http_headers[i].name))
			return (ri->http_headers[i].value);

	return (NULL);
}

const char *
aws_get_header(const struct aws_connect_t *conn, const char *name)
{
	return (get_header(&conn->request_info, name));
}

/*
 * A helper function for traversing comma separated list of values.
 * It returns a list pointer shifted to the next value, of NULL if the end
 * of the list found.
 * Value is stored in val vector. If value has form "x=y", then eq_val
 * vector is initialized to point to the "y" part, and val vector length
 * is adjusted to point only to "x".
 */
static const char *
next_option(const char *list, struct vec_t *val, struct vec_t *eq_val)
{
	if (list == NULL || *list == '\0') {
		/* End of the list */
		list = NULL;
	} else {
		val->ptr = list;
		if ((list = strchr(val->ptr, ',')) != NULL) {
			/* Comma found. Store length and shift the list ptr */
			val->len = list - val->ptr;
			list++;
		} else {
			/* This value is the last one */
			list = val->ptr + strlen(val->ptr);
			val->len = list - val->ptr;
		}

		if (eq_val != NULL) {
			/*
			 * Value has form "x=y", adjust pointers and lengths
			 * so that val points to "x", and eq_val points to "y".
			 */
			eq_val->len = 0;
			eq_val->ptr = (const char *) memchr(val->ptr, '=', val->len);
			if (eq_val->ptr != NULL) {
				eq_val->ptr++;  /* Skip over '=' character */
				eq_val->len = val->ptr + val->len - eq_val->ptr;
				val->len = (eq_val->ptr - val->ptr) - 1;
			}
		}
	}

	return (list);
}

#if !(defined(NO_CGI) && defined(NO_SSI))
/*
 * Verify that given file has certain extension
 */
static bool_t
match_extension(const char *path, const char *ext_list)
{
	struct vec_t	ext_vec;
	size_t		path_len;

	path_len = strlen(path);

	while ((ext_list = next_option(ext_list, &ext_vec, NULL)) != NULL)
		if (ext_vec.len < path_len &&
		    mg_strncasecmp(path + path_len - ext_vec.len,
			    ext_vec.ptr, ext_vec.len) == 0)
			return (TRUE);

	return (FALSE);
}
#endif /* !(NO_CGI && NO_SSI) */

/*
 * Return TRUE if "uri" matches "regexp".
 * '*' in the regexp means zero or more characters.
 */
static bool_t
match_regex(const char *uri, const char *regexp)
{
	if (*regexp == '\0')
		return (*uri == '\0');

	if (*regexp == '*')
		do {
			if (match_regex(uri, regexp + 1))
				return (TRUE);
		} while (*uri++ != '\0');

	if (*uri != '\0' && *regexp == *uri)
		return (match_regex(uri + 1, regexp + 1));

	return (FALSE);
}

static const struct callback_t *
find_callback(
	struct aws_context_t *ctx, const char *uri, int status_code)
{
	const struct callback_t	*cb, *found;
	int			i;

	found = NULL;
	pthread_mutex_lock(&ctx->bind_mutex);
	for (i = 0; i < ctx->num_callbacks; i++) {
		cb = ctx->callbacks + i;
		if ((uri != NULL && cb->uri_regex != NULL &&
		    match_regex(uri, cb->uri_regex)) || (uri == NULL &&
		     (cb->status_code == 0 ||
		      cb->status_code == status_code))) {
			found = cb;
			break;
		}
	}
	pthread_mutex_unlock(&ctx->bind_mutex);

	return (found);
}

/*
 * Send error message back to the client.
 */
static void
send_error(struct aws_connect_t *conn, int status, const char *reason,
		const char *fmt, ...)
{
	const struct callback_t	*cb;
	char		buf[BUFSIZ];
	va_list		ap;
	int		len;

	conn->request_info.status_code = status;

	/* If error handler is set, call it. Otherwise, send error message */
	if ((cb = find_callback(conn->ctx, NULL, status)) != NULL) {
		cb->func(conn, &conn->request_info, cb->user_data);
	} else {
		buf[0] = '\0';
		len = 0;

		/* Errors 1xx, 204 and 304 MUST NOT send a body */
		if (status > 199 && status != 204 && status != 304) {
			len = mg_snprintf(conn, buf, sizeof(buf),
			    "Error %d: %s\n", status, reason);
			cry_con(conn, "%s", buf);

			va_start(ap, fmt);
			len += mg_vsnprintf(conn, buf + len, sizeof(buf) - len,
			    fmt, ap);
			va_end(ap);
			conn->num_bytes_sent = len;
		}

		(void) aws_printf(conn,
		    "HTTP/1.1 %d %s\r\n"
		    "Content-Type: text/plain\r\n"
		    "Content-Length: %d\r\n"
		    "Connection: close\r\n"
		    "\r\n%s", status, reason, len, buf);
	}
}

#ifdef _WIN32
static int
pthread_mutex_init(pthread_mutex_t *mutex, void *unused)
{
	unused = NULL;
	*mutex = CreateMutex(NULL, FALSE, NULL);
	return (*mutex == NULL ? -1 : 0);
}

static int
pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	return (CloseHandle(*mutex) == 0 ? -1 : 0);
}

static int
pthread_mutex_lock(pthread_mutex_t *mutex)
{
	return (WaitForSingleObject(*mutex, INFINITE) == WAIT_OBJECT_0? 0 : -1);
}

static int
pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return (ReleaseMutex(*mutex) == 0 ? -1 : 0);
}

static int
pthread_cond_init(pthread_cond_t *cv, const void *unused)
{
	unused = NULL;
	*cv = CreateEvent(NULL, FALSE, FALSE, NULL);
	return (*cv == NULL ? -1 : 0);
}

static int
pthread_cond_timedwait(pthread_cond_t *cv, pthread_mutex_t *mutex,
	const struct timespec *ts)
{
	DWORD	status;
	DWORD	msec = INFINITE;
	time_t	now;
	
	if (ts != NULL) {
		now = time(NULL);
		msec = 1000 * (now > ts->tv_sec ? 0 : ts->tv_sec - now);
	}

	(void) ReleaseMutex(*mutex);
	status = WaitForSingleObject(*cv, msec);
	(void) WaitForSingleObject(*mutex, INFINITE);
	
	return (status == WAIT_OBJECT_0 ? 0 : -1);
}

static int
pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex)
{
	return (pthread_cond_timedwait(cv, mutex, NULL));
}

static int
pthread_cond_signal(pthread_cond_t *cv)
{
	return (SetEvent(*cv) == 0 ? -1 : 0);
}

static int
pthread_cond_destroy(pthread_cond_t *cv)
{
	return (CloseHandle(*cv) == 0 ? -1 : 0);
}

/*
 * Change all slashes to backslashes. It is Windows.
 */
static void
fix_directory_separators(char *path)
{
	int	i;

	for (i = 0; path[i] != '\0'; i++) {
		if (path[i] == '/')
			path[i] = '\\';
		/* i > 0 check is to preserve UNC paths, \\server\file.txt */
		if (path[i] == '\\' && i > 0)
			while (path[i + 1] == '\\' || path[i + 1] == '/')
				(void) memmove(path + i + 1,
				    path + i + 2, strlen(path + i + 1));
	}
}

/*
 * Encode 'path' which is assumed UTF-8 string, into UNICODE string.
 * wbuf and wbuf_len is a target buffer and its length.
 */
static void
to_unicode(const char *path, wchar_t *wbuf, size_t wbuf_len)
{
	char	buf[FILENAME_MAX], *p;

	mg_strlcpy(buf, path, sizeof(buf));
	fix_directory_separators(buf);

	/* Point p to the end of the file name */
	p = buf + strlen(buf) - 1;

	/* Trim trailing backslash character */
	while (p > buf && *p == '\\' && p[-1] != ':')
		*p-- = '\0';

	/*
	 * Protect from CGI code disclosure.
	 * This is very nasty hole. Windows happily opens files with
	 * some garbage in the end of file name. So fopen("a.cgi    ", "r")
	 * actually opens "a.cgi", and does not return an error!
	 */
	if (*p == 0x20 || *p == 0x2e) {
		/* Rejecting suspicious path */
		buf[0] = '\0'; return;
	}

	(void) MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, (int) wbuf_len);
}

static int
mg_rename(const char* oldname, const char* newname)
{
	wchar_t	woldbuf[FILENAME_MAX];
	wchar_t	wnewbuf[FILENAME_MAX];

	to_unicode(oldname, woldbuf, ARRAY_SIZE(woldbuf));
	to_unicode(newname, wnewbuf, ARRAY_SIZE(wnewbuf));

	return (MoveFileW(woldbuf, wnewbuf) ? 0 : -1);
}


static FILE *
mg_fopen(const char *path, const char *mode)
{
	wchar_t	wbuf[FILENAME_MAX], wmode[20];

	to_unicode(path, wbuf, ARRAY_SIZE(wbuf));
	MultiByteToWideChar(CP_UTF8, 0, mode, -1, wmode, ARRAY_SIZE(wmode));

	return (_wfopen(wbuf, wmode));
}

static int
mg_stat(const char *path, struct stat_t *stp)
{
	int				ok = -1; /* Error */
	wchar_t				wbuf[FILENAME_MAX];
	WIN32_FILE_ATTRIBUTE_DATA	info;

	to_unicode(path, wbuf, ARRAY_SIZE(wbuf));

	if (GetFileAttributesExW(wbuf, GetFileExInfoStandard, &info) != 0
		&& info.nFileSizeHigh == 0) /* DO not support big file */
	{
		stp->size = info.nFileSizeLow;
		stp->mtime = (time_t) ((
			((ULONGLONG) info.ftLastWriteTime.dwLowDateTime | 
			((ULONGLONG) info.ftLastWriteTime.dwHighDateTime << 32))
			-(ULONGLONG) 116444736 * 1000000000)/10000000);
		stp->is_directory =
		    info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		ok = 0;  /* Success */
	}

	return (ok);
}

static int
mg_remove(const char *path)
{
	wchar_t	wbuf[FILENAME_MAX];

	to_unicode(path, wbuf, ARRAY_SIZE(wbuf));

	return (DeleteFileW(wbuf) ? 0 : -1);
}

static int
mg_mkdir(const char *path, int mode)
{
	char	buf[FILENAME_MAX];
	wchar_t	wbuf[FILENAME_MAX];

	mode = 0; /* Unused */
	mg_strlcpy(buf, path, sizeof(buf));
	fix_directory_separators(buf);

	(void) MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, sizeof(wbuf));

	return (CreateDirectoryW(wbuf, NULL) ? 0 : -1);
}

static char *
mg_getcwd(char *buf, int buf_len)
{
	wchar_t		wbuf[FILENAME_MAX], *basename;

	if (GetModuleFileNameW(NULL, wbuf, ARRAY_SIZE(wbuf))) {
		if ((basename = wcsrchr(wbuf, DIRSEP)) != NULL) {
			*basename = L'\0';
			if (WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, buf,
			    buf_len, NULL, NULL) > 0)
				return (buf);
		}
	}

	return (NULL);
}

/*
 * Implementation of POSIX opendir/closedir/readdir for Windows.
 */
static DIR *
opendir(const char *name)
{
	DIR	*dir = NULL;
	wchar_t	wpath[FILENAME_MAX];
	DWORD attrs;

	if (name == NULL) {
		SetLastError(ERROR_BAD_ARGUMENTS);
	} else if ((dir = (DIR *) malloc(sizeof(*dir))) == NULL) {
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	} else {
		to_unicode(name, wpath, ARRAY_SIZE(wpath));
		attrs = GetFileAttributesW(wpath);
		if (attrs != 0xFFFFFFFF &&
		    ((attrs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)) {
			(void) wcscat(wpath, L"\\*");
			dir->handle = FindFirstFileW(wpath, &dir->info);
			dir->result.d_name[0] = '\0';
		} else {
			free(dir);
			dir = NULL;
		}
	}

	return (dir);
}

static int
closedir(DIR *dir)
{
	int result = 0;

	if (dir != NULL) {
		if (dir->handle != INVALID_HANDLE_VALUE)
			result = FindClose(dir->handle) ? 0 : -1;

		free(dir);
	} else {
		result = -1;
		SetLastError(ERROR_BAD_ARGUMENTS);
	}

	return (result);
}

struct dirent *
readdir(DIR *dir)
{
	struct dirent *result = 0;

	if (dir) {
		if (dir->handle != INVALID_HANDLE_VALUE) {
			result = &dir->result;
			(void) WideCharToMultiByte(CP_UTF8, 0,
			    dir->info.cFileName, -1, result->d_name,
			    sizeof(result->d_name), NULL, NULL);

			if (!FindNextFileW(dir->handle, &dir->info)) {
				(void) FindClose(dir->handle);
				dir->handle = INVALID_HANDLE_VALUE;
			}

		} else {
			SetLastError(ERROR_FILE_NOT_FOUND);
		}
	} else {
		SetLastError(ERROR_BAD_ARGUMENTS);
	}

	return (result);
}

#define	set_close_on_exec(fd)	/* No FD_CLOEXEC on Windows */

static int
start_thread(struct aws_context_t *ctx, mg_thread_func_t func, void *param)
{
	HANDLE	hThread;

	ctx = NULL;	/* Unused */
	
	hThread = CreateThread(NULL, 0,
	    (LPTHREAD_START_ROUTINE) func, param, 0, NULL);

	if (hThread != NULL)
		(void) CloseHandle(hThread);

	return (hThread == NULL ? -1 : 0);
}

#if !defined(NO_CGI)
static int
kill(pid_t pid, int sig_num)
{
	(void) TerminateProcess(pid, sig_num);
	(void) CloseHandle(pid);
	return (0);
}

static pid_t
spawn_process(struct aws_connect_t *conn, const char *prog, char *envblk,
		char *envp[], int fd_stdin, int fd_stdout, const char *dir)
{
	HANDLE	me;
	char	*p, *interp, cmdline[FILENAME_MAX], line[FILENAME_MAX];
	FILE	*fp;
	STARTUPINFOA		si;
	PROCESS_INFORMATION	pi;

	envp = NULL; /* Unused */

	(void) memset(&si, 0, sizeof(si));
	(void) memset(&pi, 0, sizeof(pi));

	/* XXX redirect CGI errors to the error log file */
	si.cb		= sizeof(si);
	si.dwFlags	= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow	= SW_HIDE;

	me = GetCurrentProcess();
	(void) DuplicateHandle(me, (HANDLE) _get_osfhandle(fd_stdin), me,
	    &si.hStdInput, 0, TRUE, DUPLICATE_SAME_ACCESS);
	(void) DuplicateHandle(me, (HANDLE) _get_osfhandle(fd_stdout), me,
	    &si.hStdOutput, 0, TRUE, DUPLICATE_SAME_ACCESS);

	/* If CGI file is a script, try to read the interpreter line */
	interp = conn->ctx->options[OPT_CGI_INTERPRETER];
	if (interp == NULL) {
		line[2] = '\0';
		(void) mg_snprintf(conn, cmdline, sizeof(cmdline), "%s%c%s",
		    dir, DIRSEP, prog);
		if ((fp = fopen(cmdline, "r")) != NULL) {
			(void) fgets(line, sizeof(line), fp);
			if (memcmp(line, "#!", 2) != 0)
				line[2] = '\0';
			/* Trim whitespaces from interpreter name */
			for (p = &line[strlen(line) - 1]; p > line &&
			    isspace(*p); p--)
				*p = '\0';
			(void) fclose(fp);
		}
		interp = line + 2;
	}

	if ((p = (char *) strrchr(prog, '/')) != NULL)
		prog = p + 1;

	(void) mg_snprintf(conn, cmdline, sizeof(cmdline), "%s%s%s",
	    interp, interp[0] == '\0' ? "" : " ", prog);

	(void) mg_snprintf(conn, line, sizeof(line), "%s", dir);
	fix_directory_separators(line);

	DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: Running [%s]", __func__, cmdline));
	if (CreateProcessA(NULL, cmdline, NULL, NULL, TRUE,
	    CREATE_NEW_PROCESS_GROUP, envblk, line, &si, &pi) == 0) {
		cry_con(conn, "%s: CreateProcess(%s): %d",
		    __func__, cmdline, ERRNO);
		pi.hProcess = (pid_t) -1;
	} else {
		(void) close(fd_stdin);
		(void) close(fd_stdout);
	}

	(void) CloseHandle(si.hStdOutput);
	(void) CloseHandle(si.hStdInput);
	(void) CloseHandle(pi.hThread);

	return ((pid_t) pi.hProcess);
}

static int
pipe(int *fds)
{
	return (_pipe(fds, BUFSIZ, _O_BINARY));
}
#endif /* !NO_CGI */

static int
set_non_blocking_mode(struct aws_connect_t *conn, SOCKET sock)
{
	unsigned long	on = 1;

	conn = NULL; /* unused */
	return (ioctlsocket(sock, FIONBIO, &on));
}

#else

static int
mg_stat(const char *path, struct stat_t *stp)
{
	struct stat	st;
	int		ok;

	if (stat(path, &st) == 0) {
		ok = 0;
		stp->size = st.st_size;
		stp->mtime = st.st_mtime;
		stp->is_directory = S_ISDIR(st.st_mode);
	} else {
		ok = -1;
	}

	return (ok);
}

static void
set_close_on_exec(int fd)
{
	(void) fcntl(fd, F_SETFD, FD_CLOEXEC);
}

static int
start_thread(struct aws_context_t *ctx, mg_thread_func_t func, void *param)
{
	pthread_t	thread_id;
	pthread_attr_t	attr;
	int		retval;

	(void) pthread_attr_init(&attr);
	(void) pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if ((retval = pthread_create(&thread_id, &attr, func, param)) != 0)
		cry_ctx(ctx,  "%s: %s", __func__, strerror(retval));

	return (retval);
}

#ifndef NO_CGI
static pid_t
spawn_process(struct aws_connect_t *conn, const char *prog, char *envblk,
		char *envp[], int fd_stdin, int fd_stdout, const char *dir)
{
	pid_t		pid;
	const char	*interp;

	envblk = NULL;	/* unused */

	if ((pid = fork()) == -1) {
		/* Parent */
		send_error(conn, 500, http_500_error,
		    "fork(): %s", strerror(ERRNO));
	} else if (pid == 0) {
		/* Child */
		if (chdir(dir) != 0) {
			cry_con(conn, "%s: chdir(%s): %s",
			    __func__, dir, strerror(ERRNO));
		} else if (dup2(fd_stdin, 0) == -1) {
			cry_con(conn, "%s: dup2(stdin, %d): %s",
			    __func__, fd_stdin, strerror(ERRNO));
		} else if (dup2(fd_stdout, 1) == -1) {
			cry_con(conn, "%s: dup2(stdout, %d): %s",
			    __func__, fd_stdout, strerror(ERRNO));
		} else {
			/* If error file is specified, send errors there */
			if (conn->ctx->error_log != NULL)
				(void) dup2(fileno(conn->ctx->error_log), 2);

			(void) close(fd_stdin);
			(void) close(fd_stdout);

			/* Execute CGI program */
			interp = conn->ctx->options[OPT_CGI_INTERPRETER];
			if (interp == NULL) {
				(void) execle(prog, prog, NULL, envp);
				cry_con(conn, "%s: execle(%s): %s",
				    __func__, prog, strerror(ERRNO));
			} else {
				(void) execle(interp, interp, prog, NULL, envp);
				cry_con(conn, "%s: execle(%s %s): %s",
				    __func__, interp, prog, strerror(ERRNO));
			}
		}
		exit(EXIT_FAILURE);
	} else {
		/* Parent. Close stdio descriptors */
		(void) close(fd_stdin);
		(void) close(fd_stdout);
	}

	return (pid);
}
#endif /* !NO_CGI */

static int
set_non_blocking_mode(struct aws_connect_t *conn, SOCKET sock)
{
	int	flags, ok = -1;

	if ((flags = fcntl(sock, F_GETFL, 0)) == -1) {
		cry_con(conn, "%s: fcntl(F_GETFL): %d", __func__, ERRNO);
	} else if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) != 0) {
		cry_con(conn, "%s: fcntl(F_SETFL): %d", __func__, ERRNO);
	} else {
		ok = 0;	/* Success */
	}

	return (ok);
}
#endif /* _WIN32 */

static void
lock_option(struct aws_context_t *ctx, int opt_index)
{
	if (pthread_mutex_lock(&ctx->opt_mutex[opt_index]) != 0)
		cry_ctx(ctx,  "pthread_mutex_lock: %s", strerror(ERRNO));
}

static void
unlock_option(struct aws_context_t *ctx, int opt_index)
{
	if (pthread_mutex_unlock(&ctx->opt_mutex[opt_index]) != 0)
		cry_ctx(ctx,  "pthread_mutex_unlock: %s", strerror(ERRNO));
}

/*
 * Write data to the IO channel - opened file descriptor, or socket
 * descriptor. Return number of bytes written.
 */
static uint32_t
push(FILE *fp, SOCKET sock, const char *buf, uint32_t len)
{
	uint32_t	sent;
	int		n, k;

	sent = 0;
	while (sent < len) {

		/* How many bytes we send in this iteration */
		k = len - sent > INT_MAX ? INT_MAX : (int) (len - sent);

		if (fp != NULL) {
			n = fwrite(buf + sent, 1, k, fp);
			if (ferror(fp))
				n = -1;
		} else {
			n = send(sock, buf + sent, k, 0);
		}

		if (n < 0)
			break;

		sent += n;
	}

	return (sent);
}

/*
 * Read from IO channel - opened file descriptor, or socket descriptor.
 * Return number of bytes read.
 */
static int
pull(FILE *fp, SOCKET sock, char *buf, int len)
{
	int	nread;

	if (fp != NULL) {
		nread = fread(buf, 1, (size_t) len, fp);
		if (ferror(fp))
			nread = -1;
	} else {
		nread = recv(sock, buf, (size_t) len, 0);
	}

	return (nread);
}

int
aws_write(struct aws_connect_t *conn, const void *buf, int len)
{
	assert(len >= 0);
	return ((int) push(NULL, conn->client.sock,
				(const char *) buf, (uint32_t) len));
}

int
aws_printf(struct aws_connect_t *conn, const char *fmt, ...)
{
	char	buf[MAX_REQUEST_SIZE];
	int	len;
	va_list	ap;

	va_start(ap, fmt);
	len = mg_vsnprintf(conn, buf, sizeof(buf), fmt, ap);
	va_end(ap);

	return (aws_write(conn, buf, len));
}

/*
 * Return content length of the request, or UNKNOWN_CONTENT_LENGTH constant if
 * Content-Length header is not set.
 */
static uint32_t
get_content_length(const struct aws_connect_t *conn)
{
	const char *cl = aws_get_header(conn, "Content-Length");
	return (cl == NULL ? UNKNOWN_CONTENT_LENGTH : strtoul(cl, NULL, 10));
}

/*
 * URL-decode input buffer into destination buffer.
 * 0-terminate the destination buffer. Return the length of decoded data.
 * form-url-encoded data differs from URI encoding in a way that it
 * uses '+' as character for space, see RFC 1866 section 8.2.1
 * http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
 */
static size_t
url_decode(const char *src, size_t src_len, char *dst, size_t dst_len,
		bool_t is_form_url_encoded)
{
	size_t	i, j;
	int	a, b;
#define	HEXTOI(x)	(isdigit(x) ? x - '0' : x - 'W')

	for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++) {
		if (src[i] == '%' &&
		    isxdigit(* (unsigned char *) (src + i + 1)) &&
		    isxdigit(* (unsigned char *) (src + i + 2))) {
			a = tolower(* (unsigned char *) (src + i + 1));
			b = tolower(* (unsigned char *) (src + i + 2));
			dst[j] = ((HEXTOI(a) << 4) | HEXTOI(b)) & 0xff;
			i += 2;
		} else if (is_form_url_encoded && src[i] == '+') {
			dst[j] = ' ';
		} else {
			dst[j] = src[i];
		}
	}

	dst[j] = '\0';	/* Null-terminate the destination */

	return (j);
}

static char *
url_encode(const char *src, size_t src_len, 
	bool_t is_form_url_encoded)
{
	static const unsigned char hexchar[] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};
	register unsigned char c;
	unsigned char *dst, *to;
	const char *end = src + src_len;
	
	to = dst = (unsigned char *) malloc(src_len*3 + 1);
	assert(to!=NULL);
	
	while (src < end) {
		c = *src++;
		if (is_form_url_encoded && c == ' ') {
			*to++ = '+';
		}
		else if((c < '0' && c != '-' && c != '.') ||
				(c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') ||
				(c > 'z')) {
			*to++ = '%';
			*to++ = hexchar[c >> 4];
			*to++ = hexchar[c & 15];
		}
		else {
			*to++ = c;
		}
	}
	*to = 0;
	return (char *) dst;
}

/*
 * Search for a form variable in a given buffer.
 * Semantic is the same as for mg_get_var().
 */
static char *
get_var(const char *name, const char *buf, size_t buf_len)
{
	const char	*p, *e, *s;
	char		*val;
	size_t		var_len, len;

	var_len = strlen(name);
	e = buf + buf_len;
	val = NULL;

	/* buf is "var1=val1&var2=val2...". Find variable first */
	for (p = buf; p + var_len < e; p++)
		if ((p == buf || p[-1] == '&') && p[var_len] == '=' &&
		    !mg_strncasecmp(name, p, var_len)) {

			/* Point p to variable value */
			p += var_len + 1;

			/* Point s to the end of the value */
			s = (const char *) memchr(p, '&', e - p);
			if (s == NULL)
				s = e;

			/* Try to allocate the buffer */
			len = s - p;
			if ((val = (char *) malloc(len + 1)) != NULL)
				(void) url_decode(p, len, val, len + 1, TRUE);
			break;
		}

	return (val);
}

/*
 * Free the pointer returned by aws_find_XXX_var(). 
 */
void
aws_free(void *data)
{
	free(data);
}

/*
 * Return form data variable specified in query string.
 * It is caller's responsibility to free the returned value.
 */
char *
aws_find_get_var(const struct aws_connect_t *conn, const char *name)
{
	const struct aws_request_t *ri = &conn->request_info;
	char	*v1 = NULL;

	if (ri->query_string != NULL)
		v1 = get_var(name, ri->query_string, strlen(ri->query_string));

	return v1;
}

/*
 * Return form data variable specified in the POST data.
 * It is caller's responsibility to free the returned value.
 */
char *
aws_find_post_var(const struct aws_connect_t *conn, const char *name)
{
	const struct aws_request_t *ri = &conn->request_info;
	int		i, len;
	char	*v2 = NULL;
	const struct field_t *p;

	if (ri->post_data_len > 0) {
		if (conn->is_form_data != 0) {
			for (i = 0; i < conn->num_fields; i++)
			{
				p = &conn->form_fields[i];
				if (!mg_strncasecmp(name, p->name, p->name_len) && p->fn == NULL)
				{
					len = p->data_len;
					if ((v2 = (char *) malloc(len + 1)) != NULL) {
						memcpy(v2, p->data, len);
						v2[len] = '\0';
					}
					break;
				}
			}
		}
		else {
			v2 = get_var(name, ri->post_data, ri->post_data_len);
		}
	}

	return v2;
}

/*
 * Return posted file descriptor specified in the POST form.
 * Do not call aws_free for this function!
 */
struct aws_file_t *
aws_find_post_file(const struct aws_connect_t *conn, const char *name)
{
	const struct aws_request_t *ri = &conn->request_info;
	int		i;
	const struct field_t	*p;
	struct aws_file_t	*r;
	char		*s;

	if (ri->post_data_len > 0) {
		if (conn->is_form_data != 0) {
			for (i = 0; i < conn->num_fields; i++)
			{
				p = &conn->form_fields[i];
				if (!mg_strncasecmp(name, p->name, p->name_len) 
					&& p->fn != NULL && p->mime != NULL)
				{
					r = (struct aws_file_t *) malloc(sizeof(*r) + 
						p->name_len+1 + p->fn_len+1 + p->mime_len+1);
					s = (char *) (r + 1);
					memcpy(r->name=s, p->name, p->name_len);
					s += p->name_len; *s++ = '\0';
					memcpy(r->fn=s, p->fn, p->fn_len);
					s += p->fn_len; *s++ = '\0';
					memcpy(r->mime=s, p->mime, p->mime_len);
					s += p->mime_len; *s = '\0';
					r->data = p->data;
					r->data_len = p->data_len; return r;
				}
			}
		}
	}
	return NULL;
}

int aws_save_as(char *data, int len, const char *fn)
{
	FILE	*fp;
	int		n;
	if ((fp = mg_fopen(fn, "wb")) == NULL) 
		return (0);
	
	n = fwrite(data, 1, len, fp);
	fclose(fp);
	return (n == len ? 1 : 0);
}

/*
 * Transform URI to the file name.
 */
static void
convert_uri_to_file_name(struct aws_connect_t *conn, const char *uri,
		char *buf, size_t buf_len)
{
	struct aws_context_t	*ctx = conn->ctx;
	struct vec_t		uri_vec, path_vec;
	const char		*list;

	lock_option(ctx, OPT_ROOT);
	mg_snprintf(conn, buf, buf_len, "%s%s", ctx->options[OPT_ROOT], uri);
	unlock_option(ctx, OPT_ROOT);

	/* If requested URI has aliased prefix, use alternate root */
	lock_option(ctx, OPT_ALIASES);
	list = ctx->options[OPT_ALIASES];

	while ((list = next_option(list, &uri_vec, &path_vec)) != NULL) {
		if (memcmp(uri, uri_vec.ptr, uri_vec.len) == 0) {
			(void) mg_snprintf(conn, buf, buf_len, "%.*s%s",
			    path_vec.len, path_vec.ptr, uri + uri_vec.len);
			break;
		}
	}
	unlock_option(ctx, OPT_ALIASES);

#ifdef _WIN32
	fix_directory_separators(buf);
#endif /* _WIN32 */
}

/*
 * Setup listening socket on given address, return socket.
 * Address format: [local_ip_address:]port_number
 */
static SOCKET
mg_open_listening_port(struct aws_context_t *ctx, const char *str, struct usa_t *usa)
{
	SOCKET		sock;
	int		on = 1, a, b, c, d, port;

	/* MacOS needs that. If we do not zero it, bind() will fail. */
	(void) memset(usa, 0, sizeof(*usa));

	if (sscanf(str, "%d.%d.%d.%d:%d", &a, &b, &c, &d, &port) == 5) {
		/* IP address to bind to is specified */
		usa->u.sin.sin_addr.s_addr =
		    htonl((a << 24) | (b << 16) | (c << 8) | d);
	} else if (sscanf(str, "%d", &port) == 1) {
		/* Only port number is specified. Bind to all addresses */
		usa->u.sin.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		return (INVALID_SOCKET);
	}

	usa->len			= sizeof(usa->u.sin);
	usa->u.sin.sin_family		= AF_INET;
	usa->u.sin.sin_port		= htons((uint16_t) port);

	if ((sock = socket(PF_INET, SOCK_STREAM, 6)) != INVALID_SOCKET &&
	    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
	    (char *) &on, sizeof(on)) == 0 &&
	    bind(sock, &usa->u.sa, usa->len) == 0 &&
	    listen(sock, 128) == 0) {
		/* Success */
		set_close_on_exec(sock);
	} else {
		/* Error */
		cry_ctx(ctx,  "%s(%d): %s", __func__, port, strerror(ERRNO));
		if (sock != INVALID_SOCKET)
			(void) closesocket(sock);
		sock = INVALID_SOCKET;
	}

	return (sock);
}

/*
 * Check whether full request is buffered. Return:
 *   -1         if request is malformed
 *    0         if request is not yet fully buffered
 *   >0         actual request length, including last \r\n\r\n
 */
static int
get_request_len(const char *buf, size_t buflen)
{
	const char	*s, *e;
	int		len = 0;

	for (s = buf, e = s + buflen - 1; len <= 0 && s < e; s++)
		/* Control characters are not allowed but >=128 is. */
		if (!isprint(* (unsigned char *) s) && *s != '\r' &&
		    *s != '\n' && * (unsigned char *) s < 128)
			len = -1;
		else if (s[0] == '\n' && s[1] == '\n')
			len = (int) (s - buf) + 2;
		else if (s[0] == '\n' && &s[1] < e &&
		    s[1] == '\r' && s[2] == '\n')
			len = (int) (s - buf) + 3;

	return (len);
}

/*
 * Convert month to the month number. Return -1 on error, or month number
 */
static int
montoi(const char *s)
{
	size_t	i;

	for (i = 0; i < sizeof(month_names) / sizeof(month_names[0]); i++)
		if (!strcmp(s, month_names[i]))
			return ((int) i);

	return (-1);
}

/*
 * Parse date-time string, and return the corresponding time_t value
 */
static time_t
date_to_epoch(const char *s)
{
	time_t		current_time;
	struct tm	tm, *tmp;
	char		mon[32];
	int		sec, min, hour, mday, month, year;

	(void) memset(&tm, 0, sizeof(tm));
	sec = min = hour = mday = month = year = 0;

	if (((sscanf(s, "%d/%3s/%d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%d %3s %d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%*3s, %d %3s %d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%d-%3s-%d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6)) &&
	    (month = montoi(mon)) != -1) {
		tm.tm_mday	= mday;
		tm.tm_mon	= month;
		tm.tm_year	= year;
		tm.tm_hour	= hour;
		tm.tm_min	= min;
		tm.tm_sec	= sec;
	}

	if (tm.tm_year > 1900)
		tm.tm_year -= 1900;
	else if (tm.tm_year < 70)
		tm.tm_year += 100;

	/* Set Daylight Saving Time field */
	current_time = time(NULL);
	tmp = localtime(&current_time);
	tm.tm_isdst = tmp->tm_isdst;

	return (mktime(&tm));
}

/*
 * Protect against directory disclosure attack by removing '..',
 * excessive '/' and '\' characters
 */
static void
remove_double_dots_and_double_slashes(char *s)
{
	char	*p = s;

	while (*s != '\0') {
		*p++ = *s++;
		if (s[-1] == '/' || s[-1] == '\\') {
			/* Skip all double-dots */
			while (*s == '.' && s[1] == '.')
				s += 2;
			
			/* Skip all following slashes and backslashes */
			while (*s == '/' || *s == '\\')
				s++;
		}
	}
	*p = '\0';
}

/*
 * Built-in mime types
 */
static const struct {
	const char	*extension;
	size_t		ext_len;
	const char	*mime_type;
	size_t		mime_type_len;
} mime_types[] = {
	{".html",	5,	"text/html",			9},
	{".htm",	4,	"text/html",			9},
	{".shtm",	5,	"text/html",			9},
	{".shtml",	6,	"text/html",			9},
	{".css",	4,	"text/css",			8},
	{".js",		3,	"application/x-javascript",	24},
	{".ico",	4,	"image/x-icon",			12},
	{".gif",	4,	"image/gif",			9},
	{".jpg",	4,	"image/jpeg",			10},
	{".jpeg",	5,	"image/jpeg",			10},
	{".png",	4,	"image/png",			9},
	{".svg",	4,	"image/svg+xml",		13},
	{".torrent",	8,	"application/x-bittorrent",	24},
	{".wav",	4,	"audio/x-wav",			11},
	{".mp3",	4,	"audio/x-mp3",			11},
	{".mid",	4,	"audio/mid",			9},
	{".m3u",	4,	"audio/x-mpegurl",		15},
	{".ram",	4,	"audio/x-pn-realaudio",		20},
	{".ra",		3,	"audio/x-pn-realaudio",		20},
	{".doc",	4,	"application/msword",		19},
	{".exe",	4,	"application/octet-stream",	24},
	{".zip",	4,	"application/x-zip-compressed",	28},
	{".xls",	4,	"application/excel",		17},
	{".tgz",	4,	"application/x-tar-gz",		20},
	{".tar",	4,	"application/x-tar",		17},
	{".gz",		3,	"application/x-gunzip",		20},
	{".arj",	4,	"application/x-arj-compressed",	28},
	{".rar",	4,	"application/x-arj-compressed",	28},
	{".rtf",	4,	"application/rtf",		15},
	{".pdf",	4,	"application/pdf",		15},
	{".swf",	4,	"application/x-shockwave-flash",29},
	{".mpg",	4,	"video/mpeg",			10},
	{".mpeg",	5,	"video/mpeg",			10},
	{".asf",	4,	"video/x-ms-asf",		14},
	{".avi",	4,	"video/x-msvideo",		15},
	{".bmp",	4,	"image/bmp",			9},
	{NULL,		0,	NULL,				0}
};

/*
 * Look at the "path" extension and figure what mime type it has.
 * Store mime type in the vector.
 */
static void
get_mime_type(struct aws_context_t *ctx, const char *path, struct vec_t *vec)
{
	struct vec_t	ext_vec, mime_vec;
	const char	*list, *ext;
	size_t		i, path_len;

	path_len = strlen(path);

	/*
	 * Scan user-defined mime types first, in case user wants to
	 * override default mime types.
	 */
	lock_option(ctx, OPT_MIME_TYPES);
	list = ctx->options[OPT_MIME_TYPES];
	while ((list = next_option(list, &ext_vec, &mime_vec)) != NULL) {
		/* ext now points to the path suffix */
		ext = path + path_len - ext_vec.len;
		if (mg_strncasecmp(ext, ext_vec.ptr, ext_vec.len) == 0) {
			*vec = mime_vec;
			unlock_option(ctx, OPT_MIME_TYPES);
			return;
		}
	}
	unlock_option(ctx, OPT_MIME_TYPES);

	/* Now scan built-in mime types */
	for (i = 0; mime_types[i].extension != NULL; i++) {
		ext = path + (path_len - mime_types[i].ext_len);
		if (path_len > mime_types[i].ext_len &&
		    mg_strcasecmp(ext, mime_types[i].extension) == 0) {
			vec->ptr = mime_types[i].mime_type;
			vec->len = mime_types[i].mime_type_len;
			return;
		}
	}

	/* Nothing found. Fall back to text/plain */
	vec->ptr = "text/plain";
	vec->len = 10;
}

#ifndef HAVE_MD5
typedef struct MD5Context {
	uint32_t	buf[4];
	uint32_t	bits[2];
	unsigned char	in[64];
} MD5_CTX;

#if __BYTE_ORDER == 1234
#define byteReverse(buf, len)	/* Nothing */
#else
/*
 * Note: this code is harmless on little-endian machines.
 */
static void
byteReverse(unsigned char *buf, unsigned longs)
{
	uint32_t t;
	do {
		t = (uint32_t) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
			((unsigned) buf[1] << 8 | buf[0]);
		*(uint32_t *) buf = t;
		buf += 4;
	} while (--longs);
}
#endif /* __BYTE_ORDER */

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void
MD5Init(MD5_CTX *ctx)
{
	ctx->buf[0] = 0x67452301;
	ctx->buf[1] = 0xefcdab89;
	ctx->buf[2] = 0x98badcfe;
	ctx->buf[3] = 0x10325476;

	ctx->bits[0] = 0;
	ctx->bits[1] = 0;
}

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void
MD5Transform(uint32_t buf[4], uint32_t const in[16])
{
	register uint32_t a, b, c, d;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

	MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void
MD5Update(MD5_CTX *ctx, unsigned char const *buf, unsigned len)
{
	uint32_t t;

	/* Update bitcount */

	t = ctx->bits[0];
	if ((ctx->bits[0] = t + ((uint32_t) len << 3)) < t)
		ctx->bits[1]++;		/* Carry from low to high */
	ctx->bits[1] += len >> 29;

	t = (t >> 3) & 0x3f;	/* Bytes already in shsInfo->data */

	/* Handle any leading odd-sized chunks */

	if (t) {
		unsigned char *p = (unsigned char *) ctx->in + t;

		t = 64 - t;
		if (len < t) {
			memcpy(p, buf, len);
			return;
		}
		memcpy(p, buf, t);
		byteReverse(ctx->in, 16);
		MD5Transform(ctx->buf, (uint32_t *) ctx->in);
		buf += t;
		len -= t;
	}
	/* Process data in 64-byte chunks */

	while (len >= 64) {
		memcpy(ctx->in, buf, 64);
		byteReverse(ctx->in, 16);
		MD5Transform(ctx->buf, (uint32_t *) ctx->in);
		buf += 64;
		len -= 64;
	}

	/* Handle any remaining bytes of data. */

	memcpy(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void
MD5Final(unsigned char digest[16], MD5_CTX *ctx)
{
	unsigned count;
	unsigned char *p;

	/* Compute number of bytes mod 64 */
	count = (ctx->bits[0] >> 3) & 0x3F;

	/* Set the first char of padding to 0x80.  This is safe since there is
	   always at least one byte free */
	p = ctx->in + count;
	*p++ = 0x80;

	/* Bytes of padding needed to make 64 bytes */
	count = 64 - 1 - count;

	/* Pad out to 56 mod 64 */
	if (count < 8) {
		/* Two lots of padding:  Pad the first block to 64 bytes */
		memset(p, 0, count);
		byteReverse(ctx->in, 16);
		MD5Transform(ctx->buf, (uint32_t *) ctx->in);

		/* Now fill the next block with 56 bytes */
		memset(ctx->in, 0, 56);
	} else {
		/* Pad block to 56 bytes */
		memset(p, 0, count - 8);
	}
	byteReverse(ctx->in, 14);

	/* Append length in bits and transform */
	((uint32_t *) ctx->in)[14] = ctx->bits[0];
	((uint32_t *) ctx->in)[15] = ctx->bits[1];

	MD5Transform(ctx->buf, (uint32_t *) ctx->in);
	byteReverse((unsigned char *) ctx->buf, 4);
	memcpy(digest, ctx->buf, 16);
	memset((char *) ctx, 0, sizeof(ctx));	/* In case it's sensitive */
}
#endif /* !HAVE_MD5 */

/*
 * Stringify binary data. Output buffer must be twice as big as input,
 * because each byte takes 2 bytes in string representation
 */
static void
bin2str(char *to, const unsigned char *p, size_t len)
{
	static const char *hex = "0123456789abcdef";

	for (; len--; p++) {
		*to++ = hex[p[0] >> 4];
		*to++ = hex[p[0] & 0x0f];
	}
	*to = '\0';
}

/*
 * Return stringified MD5 hash for list of vectors.
 * buf must point to 33-bytes long buffer
 */
static void
mg_md5(char *buf, ...)
{
	unsigned char	hash[16];
	const char	*p;
	va_list		ap;
	MD5_CTX		ctx;

	MD5Init(&ctx);

	va_start(ap, buf);
	while ((p = va_arg(ap, const char *)) != NULL)
		MD5Update(&ctx, (unsigned char *) p, (int) strlen(p));
	va_end(ap);

	MD5Final(hash, &ctx);
	bin2str(buf, hash, sizeof(hash));
}

/*
 * Check the user's password, return 1 if OK
 */
static bool_t
check_password(const char *method, const char *ha1, const char *uri,
		const char *nonce, const char *nc, const char *cnonce,
		const char *qop, const char *response)
{
	char	ha2[32 + 1], expected_response[32 + 1];

	/* XXX  Due to a bug in MSIE, we do not compare the URI	 */
	/* Also, we do not check for authentication timeout */
	if (/*strcmp(dig->uri, c->ouri) != 0 || */
	    strlen(response) != 32 /*||
	    now - strtoul(dig->nonce, NULL, 10) > 3600 */)
		return (FALSE);

	mg_md5(ha2, method, ":", uri, NULL);
	mg_md5(expected_response, ha1, ":", nonce, ":", nc,
	    ":", cnonce, ":", qop, ":", ha2, NULL);

	return (!mg_strcasecmp(response, expected_response));
}

/*
 * Use the global passwords file, if specified by auth_gpass option,
 * or search for .htpasswd in the requested directory.
 */
static FILE *
open_auth_file(struct aws_connect_t *conn, const char *path)
{
	struct aws_context_t	*ctx = conn->ctx;
	char 			name[FILENAME_MAX];
	const char		*p, *e;
	struct stat_t	st;
	FILE			*fp;

	if (ctx->options[OPT_AUTH_GPASSWD] != NULL) {
		/* Use global passwords file */
		fp =  mg_fopen(ctx->options[OPT_AUTH_GPASSWD], "r");
		if (fp == NULL)
			cry_ctx(ctx,  "fopen(%s): %s",
			    ctx->options[OPT_AUTH_GPASSWD], strerror(ERRNO));
	} else if (!mg_stat(path, &st) && st.is_directory) {
		(void) mg_snprintf(conn, name, sizeof(name), "%s%c%s",
		    path, DIRSEP, PASSWORDS_FILE_NAME);
		fp = mg_fopen(name, "r");
	} else {
		/*
		 * Try to find .htpasswd in requested directory.
		 * Given the path, create the path to .htpasswd file
		 * in the same directory. Find the right-most
		 * directory separator character first. That would be the
		 * directory name. If directory separator character is not
		 * found, 'e' will point to 'p'.
		 */
		for (p = path, e = p + strlen(p) - 1; e > p; e--)
			if (IS_DIRSEP_CHAR(*e))
				break;

		/*
		 * Make up the path by concatenating directory name and
		 * .htpasswd file name.
		 */
		(void) mg_snprintf(conn, name, sizeof(name), "%.*s%c%s",
		    (int) (e - p), p, DIRSEP, PASSWORDS_FILE_NAME);
		fp = mg_fopen(name, "r");
	}

	return (fp);
}

/*
 * Parsed Authorization: header
 */
struct ah {
	char	*user, *uri, *cnonce, *response, *qop, *nc, *nonce;
};

static bool_t
parse_auth_header(struct aws_connect_t *conn, char *buf, size_t buf_size,
		struct ah *ah)
{
	char		*name, *value, *s;
	const char	*auth_header;

	if ((auth_header = aws_get_header(conn, "Authorization")) == NULL ||
	    mg_strncasecmp(auth_header, "Digest ", 7) != 0)
		return (FALSE);

	/* Make modifiable copy of the auth header */
	(void) mg_strlcpy(buf, auth_header + 7, buf_size);

	s = buf;
	(void) memset(ah, 0, sizeof(*ah));

	/* Gobble initial spaces */
	while (isspace(* (unsigned char *) s))
		s++;

	/* Parse authorization header */
	for (;;) {
		name = skip(&s, "=");
		value = skip(&s, ", ");

		if (*value == '"') {
			value++;
			value[strlen(value) - 1] = '\0';
		} else if (*value == '\0') {
			break;
		}

		if (!strcmp(name, "username")) {
			ah->user = value;
		} else if (!strcmp(name, "cnonce")) {
			ah->cnonce = value;
		} else if (!strcmp(name, "response")) {
			ah->response = value;
		} else if (!strcmp(name, "uri")) {
			ah->uri = value;
		} else if (!strcmp(name, "qop")) {
			ah->qop = value;
		} else if (!strcmp(name, "nc")) {
			ah->nc = value;
		} else if (!strcmp(name, "nonce")) {
			ah->nonce = value;
		}
	}

	/* CGI needs it as REMOTE_USER */
	if (ah->user != NULL)
		conn->request_info.remote_user = mg_strdup(ah->user);

	return (TRUE);
}

/*
 * Authorize against the opened passwords file. Return 1 if authorized.
 */
static bool_t
authorize(struct aws_connect_t *conn, FILE *fp)
{
	struct ah	ah;
	char		line[256], f_user[256], domain[256], ha1[256],
			buf[MAX_REQUEST_SIZE];

	if (!parse_auth_header(conn, buf, sizeof(buf), &ah))
		return (FALSE);

	/* Loop over passwords file */
	while (fgets(line, sizeof(line), fp) != NULL) {

		if (sscanf(line, "%[^:]:%[^:]:%s", f_user, domain, ha1) != 3)
			continue;

		if (!strcmp(ah.user, f_user) &&
		    !strcmp(domain, conn->ctx->options[OPT_AUTH_DOMAIN]))
			return (check_password(
			    conn->request_info.request_method, ha1,
			    ah.uri, ah.nonce, ah.nc, ah.cnonce,
			    ah.qop, ah.response));
	}

	return (FALSE);
}

/*
 * Return TRUE if request is authorised, FALSE otherwise.
 */
static bool_t
check_authorization(struct aws_connect_t *conn, const char *path)
{
	FILE		*fp;
	char		fname[FILENAME_MAX];
	struct vec_t	uri_vec, filename_vec;
	const char	*list;
	bool_t		authorized;

	fp = NULL;
	authorized = TRUE;

	lock_option(conn->ctx, OPT_PROTECT);
	list = conn->ctx->options[OPT_PROTECT];
	while ((list = next_option(list, &uri_vec, &filename_vec)) != NULL) {
		if (!memcmp(conn->uri_decoded, uri_vec.ptr, uri_vec.len)) {
			(void) mg_snprintf(conn, fname, sizeof(fname), "%.*s",
			    filename_vec.len, filename_vec.ptr);
			if ((fp = mg_fopen(fname, "r")) == NULL)
				cry_con(conn, "%s: cannot open %s: %s",
				    __func__, fname, strerror(errno));
			break;
		}
	}
	unlock_option(conn->ctx, OPT_PROTECT);

	if (fp == NULL)
		fp = open_auth_file(conn, path);

	if (fp != NULL) {
		authorized = authorize(conn, fp);
		(void) fclose(fp);
	}

	return (authorized);
}

static void
send_authorization_request(struct aws_connect_t *conn)
{
	conn->request_info.status_code = 401;
	(void) aws_printf(conn,
	    "HTTP/1.1 401 Unauthorized\r\n"
	    "WWW-Authenticate: Digest qop=\"auth\", "
	    "realm=\"%s\", nonce=\"%lu\"\r\n\r\n",
	    conn->ctx->options[OPT_AUTH_DOMAIN], (unsigned long) time(NULL));
}

static bool_t
is_authorized_for_put(struct aws_connect_t *conn)
{
	FILE	*fp;
	int	ret = FALSE;

	if ((fp = mg_fopen(conn->ctx->options[OPT_AUTH_PUT], "r")) != NULL) {
		set_close_on_exec(fileno(fp));
		ret = authorize(conn, fp);
		(void) fclose(fp);
	}

	return (ret);
}

int
aws_update_password(struct aws_context_t *ctx, const char *path,
		const char *user, const char *pass)
{
	int		found;
	char	line[512], u[512], d[512], ha1[33];
	char	fname[FILENAME_MAX], tmp[FILENAME_MAX];
	struct stat_t	st;
	const char	*domain;
	FILE	*fp, *fp2;

	found = 0;
	fp = fp2 = NULL;
	domain = ctx->options[OPT_AUTH_DOMAIN];

	/* Regard empty password as no password - remove user record. */
	if (pass[0] == '\0')
		pass = NULL;

	if (!mg_stat(path, &st) && st.is_directory) {
		(void) snprintf(fname, sizeof(fname), "%s%c%s",
		    path, DIRSEP, PASSWORDS_FILE_NAME);
	}
	else {
		mg_strlcpy(fname, path, sizeof(fname));
	}

	(void) snprintf(tmp, sizeof(tmp), "%s.tmp", fname);

	/* Create the file if does not exist */
	if ((fp = mg_fopen(fname, "a+")) != NULL)
		(void) fclose(fp);

	/* Open the given file and temporary file */
	if ((fp = mg_fopen(fname, "r")) == NULL) {
		cry_ctx(ctx,  "Cannot open %s: %s", fname, strerror(errno));
		return (0);
	} else if ((fp2 = mg_fopen(tmp, "w+")) == NULL) {
		cry_ctx(ctx,  "Cannot open %s: %s", tmp, strerror(errno));
		return (0);
	}

	/* Copy the stuff to temporary file */
	while (fgets(line, sizeof(line), fp) != NULL) {

		if (sscanf(line, "%[^:]:%[^:]:%*s", u, d) != 2)
			continue;

		if (!strcmp(u, user) && !strcmp(d, domain)) {
			found++;
			if (pass != NULL) {
				mg_md5(ha1, user, ":", domain, ":", pass, NULL);
				fprintf(fp2, "%s:%s:%s\n", user, domain, ha1);
			}
		} else {
			(void) fprintf(fp2, "%s", line);
		}
	}

	/* If new user, just add it */
	if (!found && pass != NULL) {
		mg_md5(ha1, user, ":", domain, ":", pass, NULL);
		(void) fprintf(fp2, "%s:%s:%s\n", user, domain, ha1);
	}

	/* Close files */
	(void) fclose(fp);
	(void) fclose(fp2);

	/* Put the temp file in place of real file */
	(void) mg_remove(fname);
	(void) mg_rename(tmp, fname);

	return (0);
}

struct de {
	struct aws_connect_t	*conn;
	char			*file_name;
	struct stat_t	st;
};

/*
 * This function is called from send_directory() and prints out
 * single directory entry.
 */
static void
print_dir_entry(struct de *de)
{
	char		size[64], mod[64];
	char		*url_name = url_encode(
		de->file_name, strlen(de->file_name), FALSE);

	if (de->st.is_directory) {
		(void) mg_snprintf(de->conn,
		    size, sizeof(size), "%s", "[DIRECTORY]");
	} else {
		/*
		 * We use (signed) cast below because MSVC 6 compiler cannot
		 * convert unsigned __int64 to double. Sigh.
		 */
		if (de->st.size < 1024)
			(void) mg_snprintf(de->conn, size, sizeof(size),
			    "%lu", (unsigned long) de->st.size);
		else if (de->st.size < 1024 * 1024)
			(void) mg_snprintf(de->conn, size, sizeof(size),
			    "%.1fk", (double) de->st.size / 1024.0);
		else if (de->st.size < 1024 * 1024 * 1024)
			(void) mg_snprintf(de->conn, size, sizeof(size),
			    "%.1fM", (double) de->st.size / 1048576);
		else
			(void) mg_snprintf(de->conn, size, sizeof(size),
			  "%.1fG", (double) de->st.size / 1073741824);
	}
	(void) strftime(mod, sizeof(mod), "%d-%b-%Y %H:%M",
		localtime(&de->st.mtime));
	de->conn->num_bytes_sent += aws_printf(de->conn,
	    "<tr><td><a href=\"%s%s\">%s%s</a></td>"
	    "<td>&nbsp;%s</td><td>&nbsp;&nbsp;%s</td></tr>\n",
	    de->conn->request_info.uri, url_name, de->file_name,
	    de->st.is_directory ? "/" : "", mod, size);
	free(url_name);
}

/*
 * This function is called from send_directory() and used for
 * sorting direcotory entries by size, or name, or modification time.
 */
static int
compare_dir_entries(const void *p1, const void *p2)
{
	const struct de	*a = (struct de *) p1, *b = (struct de *) p2;
	const char	*query_string = a->conn->request_info.query_string;
	int		cmp_result = 0;

	if (query_string == NULL)
		query_string = "na";

	if (a->st.is_directory && !b->st.is_directory) {
		return (-1);  /* Always put directories on top */
	} else if (!a->st.is_directory && b->st.is_directory) {
		return (1);   /* Always put directories on top */
	} else if (*query_string == 'n') {
		cmp_result = strcmp(a->file_name, b->file_name);
	} else if (*query_string == 's') {
		cmp_result = a->st.size == b->st.size ? 0 :
			a->st.size > b->st.size ? 1 : -1;
	} else if (*query_string == 'd') {
		cmp_result = a->st.mtime == b->st.mtime ? 0 :
			a->st.mtime > b->st.mtime ? 1 : -1;
	}

	return (query_string[1] == 'd' ? -cmp_result : cmp_result);
}

/*
 * Send directory contents.
 */
static void
send_directory(struct aws_connect_t *conn, const char *dir)
{
	struct dirent	*dp;
	DIR		*dirp;
	struct de	*entries = NULL;
	char		path[FILENAME_MAX];
	int		i, sort_direction, num_entries = 0, arr_size = 128;

	if ((dirp = opendir(dir)) == NULL) {
		send_error(conn, 500, "Cannot open directory",
		    "Error: opendir(%s): %s", path, strerror(ERRNO));
		return;
	}

	(void) aws_printf(conn, "%s",
	    "HTTP/1.1 200 OK\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/html; charset=utf-8\r\n\r\n");

	sort_direction = conn->request_info.query_string != NULL &&
	    conn->request_info.query_string[1] == 'd' ? 'a' : 'd';

	while ((dp = readdir(dirp)) != NULL) {

		/* Do not show current dir and passwords file */
		if (!strcmp(dp->d_name, ".") ||
		    !strcmp(dp->d_name, "..") ||
		    !strcmp(dp->d_name, PASSWORDS_FILE_NAME))
			continue;

		if (entries == NULL || num_entries >= arr_size) {
			arr_size *= 2;
			entries = (struct de *) realloc(entries,
			    arr_size * sizeof(entries[0]));
		}

		if (entries == NULL) {
			send_error(conn, 500, "Cannot open directory",
			    "%s", "Error: cannot allocate memory");
			return;
		}

		(void) mg_snprintf(conn, path, sizeof(path), "%s%c%s",
		    dir, DIRSEP, dp->d_name);

		if (!mg_stat(path, &entries[num_entries].st)) {
			entries[num_entries].conn = conn;
			entries[num_entries].file_name = mg_strdup(dp->d_name);
			num_entries++;
		}
	}
	(void) closedir(dirp);

	conn->num_bytes_sent += aws_printf(conn,
	    "<html><head><title>Index of %s</title>"
	    "<style>th {text-align: left;}</style></head>"
	    "<body><h1>Index of %s</h1><pre><table cellpadding=\"0\">"
	    "<tr><th><a href=\"?n%c\">Name</a></th>"
	    "<th><a href=\"?d%c\">Modified</a></th>"
	    "<th><a href=\"?s%c\">Size</a></th></tr>"
	    "<tr><td colspan=\"3\"><hr></td></tr>",
	    conn->uri_decoded, conn->uri_decoded,
	    sort_direction, sort_direction, sort_direction);

	/* Print first entry - link to a parent directory */
	conn->num_bytes_sent += aws_printf(conn,
	    "<tr><td><a href=\"%s%s\">%s</a></td>"
	    "<td>&nbsp;%s</td><td>&nbsp;&nbsp;%s</td></tr>\n",
	    conn->request_info.uri, "..", "Parent directory", "-", "-");

	/* Sort and print directory entries */
	qsort(entries, num_entries, sizeof(entries[0]), compare_dir_entries);
	for (i = 0; i < num_entries; i++) {
		print_dir_entry(&entries[i]);
		free(entries[i].file_name);
	}
	free(entries);

	conn->num_bytes_sent += aws_printf(conn, "%s", "</table></body></html>");
	conn->request_info.status_code = 200;
}

/*
 * Send len bytes from the opened file to the client.
 */
static void
send_opened_file_stream(struct aws_connect_t *conn, FILE *fp, uint32_t len)
{
	char	buf[BUFSIZ];
	int	to_read, num_read, num_written;

	while (len > 0) {
		/* Calculate how much to read from the file in the buffer */
		to_read = sizeof(buf);
		if ((uint32_t) to_read > len)
			to_read = (int) len;

		/* Read from file, exit the loop on error */
		if ((num_read = fread(buf, 1, to_read, fp)) == 0)
			break;

		/* Send read bytes to the client, exit the loop on error */
		if ((num_written = aws_write(conn, buf, num_read)) != num_read)
			break;

		/* Both read and were successful, adjust counters */
		conn->num_bytes_sent += num_written;
		len -= num_written;
	}
}

/*
 * Send regular file contents.
 */
static void
send_file(struct aws_connect_t *conn, const char *path, struct stat_t *stp)
{
	char		date[64], lm[64], etag[64], range[64];
	const char	*fmt = "%a, %d %b %Y %H:%M:%S %Z", *msg = "OK", *hdr;
	time_t		curtime = time(NULL);
	uint32_t	cl, r1, r2;
	struct vec_t	mime_vec;
	FILE		*fp;
	int		n;

	get_mime_type(conn->ctx, path, &mime_vec);
	cl = stp->size;
	conn->request_info.status_code = 200;
	range[0] = '\0';

	if ((fp = mg_fopen(path, "rb")) == NULL) {
		send_error(conn, 500, http_500_error,
		    "fopen(%s): %s", path, strerror(ERRNO));
		return;
	}
	set_close_on_exec(fileno(fp));

	/* If Range: header specified, act accordingly */
	r1 = r2 = 0;
	hdr = aws_get_header(conn, "Range");
	if (hdr != NULL && (n = sscanf(hdr,
	    "bytes=%u-%u", &r1, &r2)) > 0) {
		conn->request_info.status_code = 206;
		(void) fseeko(fp, (off_t) r1, SEEK_SET);
		cl = n == 2 ? r2 - r1 + 1: cl - r1;
		(void) mg_snprintf(conn, range, sizeof(range),
		    "Content-Range: bytes "
		    "%u-%u/%u\r\n",
		    r1, r1 + cl - 1, stp->size);
		msg = "Partial Content";
	}

	/* Prepare Etag, Date, Last-Modified headers */
	(void) strftime(date, sizeof(date), fmt, localtime(&curtime));
	(void) strftime(lm, sizeof(lm), fmt, localtime(&stp->mtime));
	(void) mg_snprintf(conn, etag, sizeof(etag), "%lx.%lx",
	    (unsigned long) stp->mtime, (unsigned long) stp->size);

	(void) aws_printf(conn,
	    "HTTP/1.1 %d %s\r\n"
	    "Date: %s\r\n"
	    "Last-Modified: %s\r\n"
	    "Etag: \"%s\"\r\n"
	    "Content-Type: %.*s\r\n"
	    "Content-Length: %lu\r\n"
	    "Connection: close\r\n"
	    "Accept-Ranges: bytes\r\n"
	    "%s\r\n",
	    conn->request_info.status_code, msg, date, lm, etag,
	    mime_vec.len, mime_vec.ptr, cl, range);

	if (strcmp(conn->request_info.request_method, "HEAD") != 0)
		send_opened_file_stream(conn, fp, cl);
	(void) fclose(fp);
}

int aws_send_file(struct aws_connect_t *conn, const char *fn)
{
	struct stat_t st;
	if (mg_stat(fn, &st) == 0 && !st.is_directory) {
		send_file(conn, fn, &st);
		return 1;
	}
	return 0;
}

/*
 * Parse HTTP headers from the given buffer, advance buffer to the point
 * where parsing stopped.
 */
static void
parse_http_headers(char **buf, struct aws_request_t *ri)
{
	int	i;

	for (i = 0; i < (int) ARRAY_SIZE(ri->http_headers); i++) {
		ri->http_headers[i].name = skip(buf, ": ");
		ri->http_headers[i].value = skip(buf, "\r\n");
		if (ri->http_headers[i].name[0] == '\0')
			break;
		ri->num_headers = i + 1;
	}
}

static bool_t
is_known_http_method(const char *method)
{
	return (!strcmp(method, "GET") ||
	    !strcmp(method, "POST") ||
	    !strcmp(method, "HEAD") ||
	    !strcmp(method, "PUT") ||
	    !strcmp(method, "DELETE"));
}

/*
 * Parse HTTP request, fill in aws_request_t structure.
 */
static bool_t
parse_http_request(char *buf, struct aws_request_t *ri, const struct usa_t *usa)
{
	char	*http_version;
	int	n, success_code = FALSE;

	ri->request_method = skip(&buf, " ");
	ri->uri = skip(&buf, " ");
	http_version = skip(&buf, "\r\n");

	if (is_known_http_method(ri->request_method) &&
	    ri->uri[0] == '/' &&
	    sscanf(http_version, "HTTP/%d.%d%n",
	    &ri->http_version_major, &ri->http_version_minor, &n) == 2 &&
	    http_version[n] == '\0') {
		parse_http_headers(&buf, ri);
		ri->remote_port = ntohs(usa->u.sin.sin_port);
		(void) memcpy(&ri->remote_ip, &usa->u.sin.sin_addr.s_addr, 4);
		ri->remote_ip = ntohl(ri->remote_ip);
		success_code = TRUE;
	}

	return (success_code);
}

/*
 * Keep reading the input (either opened file descriptor fd, or socket sock
 * descriptor) into buffer buf, until \r\n\r\n appears in the
 * buffer (which marks the end of HTTP request). Buffer buf may already
 * have some data. The length of the data is stored in nread.
 * Upon every read operation, increase nread by the number of bytes read.
 */
static int
read_request(FILE *fp, SOCKET sock, char *buf, int bufsiz, int *nread)
{
	int	n, request_len;

	request_len = 0;
	while (*nread < bufsiz && request_len == 0) {
		n = pull(fp, sock, buf + *nread, bufsiz - *nread);
		if (n <= 0) {
			break;
		} else {
			*nread += n;
			request_len = get_request_len(buf, (size_t) *nread);
		}
	}

	return (request_len);
}

/*
 * For given directory path, substitute it to valid index file.
 * Return 0 if index file has been found, -1 if not found.
 * If the file is found, it's stats is returned in stp.
 */
static bool_t
substitute_index_file(struct aws_connect_t *conn,
		char *path, size_t path_len, struct stat_t *stp)
{
	const char	*list;
	struct stat_t	st;
	struct vec_t	filename_vec;
	size_t		n;
	bool_t		found;

	n = strlen(path);

	/*
	 * The 'path' given to us points to the directory. Remove all trailing
	 * directory separator characters from the end of the path, and
	 * then append single directory separator character.
	 */
	while (n > 0 && IS_DIRSEP_CHAR(path[n - 1]))
		n--;
	path[n] = DIRSEP;

	/*
	 * Traverse index files list. For each entry, append it to the given
	 * path and see if the file exists. If it exists, break the loop
	 */
	lock_option(conn->ctx, OPT_INDEX_FILES);
	list = conn->ctx->options[OPT_INDEX_FILES];
	found = FALSE;

	while ((list = next_option(list, &filename_vec, NULL)) != NULL) {

		/* Ignore too long entries that may overflow path buffer */
		if (filename_vec.len > path_len - n)
			continue;

		/* Prepare full path to the index file  */
		(void) mg_strlcpy(path + n + 1,
		    filename_vec.ptr, filename_vec.len + 1);

		/* Does it exist ? */
		if (mg_stat(path, &st) == 0) {
			/* Yes it does, break the loop */
			*stp = st;
			found = TRUE;
			break;
		}
	}
	unlock_option(conn->ctx, OPT_INDEX_FILES);

	/* If no index file exists, restore directory path */
	if (found == FALSE)
		path[n] = '\0';

	return (found);
}

static void
remove_callback(
	struct aws_context_t *ctx,	const char *uri_regex, int status_code)
{
	struct callback_t	*cb;
	int		i;

	for (i = 0; i < ctx->num_callbacks; i++) {
		cb = ctx->callbacks + i;
		if ((uri_regex != NULL && cb->uri_regex != NULL &&
		    !strcmp(uri_regex, cb->uri_regex)) || (uri_regex == NULL &&
		     (cb->status_code == 0 ||
		      cb->status_code == status_code))) {
			(void) memmove(cb, cb + 1,
			    (char *) (ctx->callbacks + ctx->num_callbacks) -
			    (char *) (cb + 1));
			break;
		}
	}
}

static void
add_callback(struct aws_context_t *ctx, const char *uri_regex, int status_code,
		aws_callback_t func, void *user_data)
{
	struct callback_t	*cb;

	pthread_mutex_lock(&ctx->bind_mutex);
	if (func == NULL) {
		remove_callback(ctx, uri_regex, status_code);
	} else if (ctx->num_callbacks >= (int) ARRAY_SIZE(ctx->callbacks) - 1) {
		cry_ctx(ctx,  "Too many callbacks! Increase MAX_CALLBACKS.");
	} else {
		cb = &ctx->callbacks[ctx->num_callbacks];
		cb->uri_regex = uri_regex ? mg_strdup(uri_regex) : NULL;
		cb->func = func;
		cb->status_code = status_code;
		cb->user_data = user_data;
		ctx->num_callbacks++;
		DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: uri %s code %d",
		    __func__, uri_regex ? uri_regex : "NULL", status_code));
	}
	pthread_mutex_unlock(&ctx->bind_mutex);
}

void
aws_set_uri_callback(struct aws_context_t *ctx, const char *uri_regex,
		aws_callback_t func, void *user_data)
{
	assert(uri_regex != NULL);
	add_callback(ctx, uri_regex, -1, func, user_data);
}

void
aws_set_err_callback(struct aws_context_t *ctx, int error_code,
		aws_callback_t func, void *user_data)
{
	assert(error_code >= 0 && error_code < 1000);
	add_callback(ctx, NULL, error_code, func, user_data);
}

/*
 * Return True if we should reply 304 Not Modified.
 */
static bool_t
is_not_modified(const struct aws_connect_t *conn, const struct stat_t *stp)
{
	const char *ims = aws_get_header(conn, "If-Modified-Since");
	return (ims != NULL && stp->mtime < date_to_epoch(ims));
}


/*
 * An analyzer for form-data, which extracts different fields sequently.
 */
static void 
analyze_form_data(struct aws_connect_t *conn)
{
	struct aws_request_t *ri = &conn->request_info;
	const char *ct = get_header(ri, "Content-Type");
	
	if (ct != NULL && 
		!mg_strncasecmp(ct, "multipart/form-data", 19))
	{
		int	i, j, b;
		conn->is_form_data = 1;
		conn->num_fields = 0;
		
		ct = strstr(ct, "boundary=");
		if (ct != NULL) ct += 9;
		
		for (b = 0; b < ri->post_data_len; b++) {
			if (ri->post_data[b] == 13) break;
		}
		if (b == 0) return;
		
		if (ct != NULL && 
			mg_strnstr(ri->post_data, ct, b) == NULL) return;
		ct = ri->post_data;
		
		for (i = b+2; i < ri->post_data_len && conn->num_fields < 
			(int) ARRAY_SIZE(conn->form_fields); )
		{
			/* (1) find the ending of the header */
			struct field_t *p;
			char *s, *e, *t;
			
			s = ri->post_data + i;
			for (; i+2 < ri->post_data_len; i++) { 
				if (ri->post_data[i]==13 && 
					ri->post_data[i+2]==13) break;
			}
			if (i+2 >= ri->post_data_len) break;
			
			e = ri->post_data + i;
			p = &conn->form_fields[conn->num_fields];
			
			/* (2) parser the data header */
			s = mg_strnstr(s, "name=\"", e-s);
			if (s != NULL) {
				p->name = (s+=6);
				if ((s = mg_strnchr(s, '\"', e-s)) != NULL) 
					p->name_len = (s++) - p->name;
			}
			if (s == NULL) break;
			
			s = mg_strnstr(s, "filename=\"", e-s);
			if (s != NULL) {
				p->fn = (s+=10);
				if ((s = mg_strnchr(s, '\"', e-s)) != NULL) {
					while ((t=mg_strnchr(
						p->fn, '/',  s-p->fn)) != NULL) p->fn = t+1;
					while ((t=mg_strnchr(
						p->fn, '\\', s-p->fn)) != NULL) p->fn = t+1;
					p->fn_len = (s++) - p->fn;
					s = mg_strnstr(s, "Content-Type: ", e-s);
					if (s != NULL) {
						p->mime = (s+=14);
						p->mime_len = e-s;
					}
				}
				else break;
			}
			
			/* (3) get the data section */
			s = ri->post_data + (i+=4);
			for (j = 0; i < ri->post_data_len;)
			{
				if (ri->post_data[i] == ct[j])
				{
					++i; ++j;
					if (j == b) {
						e = ri->post_data + (i-b-2);
						p->data = s;
						p->data_len = e-s;
						i+=2; 
						conn->num_fields++; break;
					}
				}
				else {++i; j=0;}
			}
		}
	}
}

static bool_t
append_chunk(struct aws_request_t *ri, FILE *fp, const char *buf, int len)
{
	bool_t	ret_code = TRUE;

	if (fp == NULL) {
		/* TODO: check for NULL here */
		ri->post_data = (char *) realloc(ri->post_data,
		    ri->post_data_len + len);
		(void) memcpy(ri->post_data + ri->post_data_len, buf, len);
		ri->post_data_len += len;
	} else if (push(fp, INVALID_SOCKET,
	    buf, (uint32_t) len) != (uint32_t) len) {
		ret_code = FALSE;
	}

	return (ret_code);
}

static bool_t
handle_request_body(struct aws_connect_t *conn, FILE *fp)
{
	struct aws_request_t	*ri = &conn->request_info;
	const char	*expect, *tmp;
	uint32_t	content_len;
	char		buf[BUFSIZ];
	int		to_read, nread, already_read;
	bool_t		success_code = FALSE;

	content_len = get_content_length(conn);
	expect = aws_get_header(conn, "Expect");

	if (content_len == UNKNOWN_CONTENT_LENGTH) {
		send_error(conn, 411, "Length Required", "");
	} else if (expect != NULL && mg_strcasecmp(expect, "100-continue")) {
		send_error(conn, 417, "Expectation Failed", "");
	} else {
		if (expect != NULL)
			(void) aws_printf(conn, "HTTP/1.1 100 Continue\r\n\r\n");

		already_read = ri->post_data_len;
		assert(already_read >= 0);

		if (content_len <= (uint32_t) already_read) {
			ri->post_data_len = (int) content_len;
			/*
			 * If fp is NULL, this is embedded mode, and we do not
			 * have to do anything: POST data is already there,
			 * no need to allocate a buffer and copy it in.
			 * If fp != NULL, we need to write the data.
			 */
			success_code = fp == NULL || (push(fp, INVALID_SOCKET,
			    ri->post_data, content_len) == content_len) ?
			    TRUE : FALSE;
		} else {

			if (fp == NULL) {
				conn->free_post_data = TRUE;
				tmp = ri->post_data;
				/* +1 in case if already_read == 0 */
				ri->post_data = (char*)malloc(already_read + 1);
				(void) memcpy(ri->post_data, tmp, already_read);
			} else {
				(void) push(fp, INVALID_SOCKET, 
				    ri->post_data, (uint32_t) already_read);
			}

			content_len -= already_read;

			while (content_len > 0) {
				to_read = sizeof(buf);
				if ((uint32_t) to_read > content_len)
					to_read = (int) content_len;
				nread = pull(NULL, conn->client.sock, buf, to_read);
				if (nread <= 0)
					break;
				if (!append_chunk(ri, fp, buf, nread))
					break;
				content_len -= nread;
			}
			success_code = content_len == 0 ? TRUE : FALSE;
		}

		/* Each error code path in this function must send an error */
		if (success_code != TRUE)
			send_error(conn, 577, http_500_error,
			    "%s", "Error handling body data");
	}
	
	if (success_code == TRUE) 
		analyze_form_data(conn);
	
	return (success_code);
}

#if !defined(NO_CGI)

/*
 * This structure helps to create an environment for the spawned CGI program.
 * Environment is an array of "VARIABLE=VALUE\0" ASCIIZ strings,
 * last element must be NULL.
 * However, on Windows there is a requirement that all these VARIABLE=VALUE\0
 * strings must reside in a contiguous buffer. The end of the buffer is
 * marked by two '\0' characters.
 * We satisfy both worlds: we create an envp array (which is vars), all
 * entries are actually pointers inside buf.
 */
struct cgi_env_block {
	struct aws_connect_t *conn;
	char	buf[CGI_ENVIRONMENT_SIZE];	/* Environment buffer	*/
	int	len;				/* Space taken		*/
	char	*vars[MAX_CGI_ENVIR_VARS];	/* char **envp		*/
	int	nvars;				/* Number of variables	*/
};

/*
 * Append VARIABLE=VALUE\0 string to the buffer, and add a respective
 * pointer into the vars array.
 */
static char *
addenv(struct cgi_env_block *block, const char *fmt, ...)
{
	int	n, space;
	char	*added;
	va_list	ap;

	/* Calculate how much space is left in the buffer */
	space = sizeof(block->buf) - block->len - 2;
	assert(space >= 0);

	/* Make a pointer to the free space int the buffer */
	added = block->buf + block->len;

	/* Copy VARIABLE=VALUE\0 string into the free space */
	va_start(ap, fmt);
	n = mg_vsnprintf(block->conn, added, (size_t) space, fmt, ap);
	va_end(ap);

	/* Make sure we do not overflow buffer and the envp array */
	if (n > 0 && n < space &&
	    block->nvars < (int) ARRAY_SIZE(block->vars) - 2) {
		/* Append a pointer to the added string into the envp array */
		block->vars[block->nvars++] = block->buf + block->len;
		/* Bump up used length counter. Include \0 terminator */
		block->len += n + 1;
	}

	return (added);
}

static void
prepare_cgi_environment(struct aws_connect_t *conn, const char *prog,
		struct cgi_env_block *blk)
{
	const char	*s, *script_filename, *root;
	struct vec_t	var_vec;
	char		*p;
	int		i;

	blk->len = blk->nvars = 0;
	blk->conn = conn;

	/* SCRIPT_FILENAME */
	script_filename = prog;
	if ((s = strrchr(prog, '/')) != NULL)
		script_filename = s + 1;

	lock_option(conn->ctx, OPT_ROOT);
	root = conn->ctx->options[OPT_ROOT];
	addenv(blk, "SERVER_NAME=%s", conn->ctx->options[OPT_AUTH_DOMAIN]);
	unlock_option(conn->ctx, OPT_ROOT);

	/* Prepare the environment block */
	addenv(blk, "%s", "GATEWAY_INTERFACE=CGI/1.1");
	addenv(blk, "%s", "SERVER_PROTOCOL=HTTP/1.1");
	addenv(blk, "%s", "REDIRECT_STATUS=200");	/* PHP */
	addenv(blk, "SERVER_PORT=%d", ntohs(conn->client.lsa.u.sin.sin_port));
	addenv(blk, "SERVER_ROOT=%s", root);
	addenv(blk, "DOCUMENT_ROOT=%s", root);
	addenv(blk, "REQUEST_METHOD=%s", conn->request_info.request_method);
	addenv(blk, "REMOTE_ADDR=%s",
	    inet_ntoa(conn->client.rsa.u.sin.sin_addr));
	addenv(blk, "REMOTE_PORT=%d", conn->request_info.remote_port);
	addenv(blk, "REQUEST_URI=%s", conn->request_info.uri);
	addenv(blk, "SCRIPT_NAME=%s", prog + strlen(root));
	addenv(blk, "SCRIPT_FILENAME=%s", script_filename);	/* PHP */
	addenv(blk, "PATH_TRANSLATED=%s", prog);
	addenv(blk, "HTTPS=off");

	if ((s = aws_get_header(conn, "Content-Type")) != NULL)
		addenv(blk, "CONTENT_TYPE=%s", s);

	if (conn->request_info.query_string != NULL)
		addenv(blk, "QUERY_STRING=%s", conn->request_info.query_string);

	if ((s = aws_get_header(conn, "Content-Length")) != NULL)
		addenv(blk, "CONTENT_LENGTH=%s", s);

	if ((s = getenv("PATH")) != NULL)
		addenv(blk, "PATH=%s", s);

#if defined(_WIN32)
	if ((s = getenv("COMSPEC")) != NULL)
		addenv(blk, "COMSPEC=%s", s);
	if ((s = getenv("SYSTEMROOT")) != NULL)
		addenv(blk, "SYSTEMROOT=%s", s);
#else
	if ((s = getenv("LD_LIBRARY_PATH")) != NULL)
		addenv(blk, "LD_LIBRARY_PATH=%s", s);
#endif /* _WIN32 */

	if ((s = getenv("PERLLIB")) != NULL)
		addenv(blk, "PERLLIB=%s", s);

	if (conn->request_info.remote_user != NULL) {
		addenv(blk, "REMOTE_USER=%s", conn->request_info.remote_user);
		addenv(blk, "%s", "AUTH_TYPE=Digest");
	}

	/* Add all headers as HTTP_* variables */
	for (i = 0; i < conn->request_info.num_headers; i++) {
		p = addenv(blk, "HTTP_%s=%s",
		    conn->request_info.http_headers[i].name,
		    conn->request_info.http_headers[i].value);

		/* Convert variable name into uppercase, and change - to _ */
		for (; *p != '=' && *p != '\0'; p++) {
			if (*p == '-')
				*p = '_';
			*p = (char) toupper(* (unsigned char *) p);
		}
	}

	/* Add user-specified variables */
	lock_option(conn->ctx, OPT_CGI_ENV);
	s = conn->ctx->options[OPT_CGI_ENV];
	while ((s = next_option(s, &var_vec, NULL)) != NULL)
		addenv(blk, "%.*s", var_vec.len, var_vec.ptr);
	unlock_option(conn->ctx, OPT_CGI_ENV);

	blk->vars[blk->nvars++] = NULL;
	blk->buf[blk->len++] = '\0';

	assert(blk->nvars < (int) ARRAY_SIZE(blk->vars));
	assert(blk->len > 0);
	assert(blk->len < (int) sizeof(blk->buf));
}

static void
send_cgi(struct aws_connect_t *conn, const char *prog)
{
	int			headers_len, data_len, i, n;
	const char		*status;
	char			buf[MAX_REQUEST_SIZE], *pbuf;
	struct aws_request_t	ri;
	struct cgi_env_block	blk;
	char			dir[FILENAME_MAX], *p;
	int			fd_stdin[2], fd_stdout[2];
	FILE			*in, *out;
	pid_t			pid;

	(void) memset(&ri, 0, sizeof(ri));
	prepare_cgi_environment(conn, prog, &blk);

	/* CGI must be executed in its own directory */
	(void) mg_snprintf(conn, dir, sizeof(dir), "%s", prog);
	if ((p = strrchr(dir, DIRSEP)) != NULL)
		*p++ = '\0';

	pid = (pid_t) -1;
	fd_stdin[0] = fd_stdin[1] = fd_stdout[0] = fd_stdout[1] = -1;
	in = out = NULL;

	if (pipe(fd_stdin) != 0 || pipe(fd_stdout) != 0) {
		send_error(conn, 500, http_500_error,
		    "Cannot create CGI pipe: %s", strerror(ERRNO));
		goto done;
	} else if ((pid = spawn_process(conn, p, blk.buf, blk.vars,
	    fd_stdin[0], fd_stdout[1], dir)) == (pid_t) -1) {
		goto done;
	} else if ((in = fdopen(fd_stdin[1], "wb")) == NULL ||
	    (out = fdopen(fd_stdout[0], "rb")) == NULL) {
		send_error(conn, 500, http_500_error,
		    "fopen: %s", strerror(ERRNO));
		goto done;
	}

	setbuf(in, NULL);
	setbuf(out, NULL);

	/*
	 * spawn_process() must close those!
	 * If we don't mark them as closed, close() attempt before
	 * return from this function throws an exception on Windows.
	 * Windows does not like when closed descriptor is closed again.
	 */
	fd_stdin[0] = fd_stdout[1] = -1;

	/* Send POST data to the CGI process if needed */
	if (!strcmp(conn->request_info.request_method, "POST") &&
	    !handle_request_body(conn, in)) {
		goto done;
	}

	/*
	 * Now read CGI reply into a buffer. We need to set correct
	 * status code, thus we need to see all HTTP headers first.
	 * Do not send anything back to client, until we buffer in all
	 * HTTP headers.
	 */
	data_len = 0;
	headers_len = read_request(out, INVALID_SOCKET, 
	    buf, sizeof(buf), &data_len);
	if (headers_len <= 0) {
		send_error(conn, 500, http_500_error,
		    "CGI program sent malformed HTTP headers: [%.*s]",
		    data_len, buf);
		goto done;
	}
	pbuf = buf;
	buf[headers_len - 1] = '\0';
	parse_http_headers(&pbuf, &ri);

	/* Make up and send the status line */
	status = get_header(&ri, "Status");
	conn->request_info.status_code = status == NULL ? 200 : atoi(status);
	(void) aws_printf(conn, "HTTP/1.1 %d OK\r\n",
	    conn->request_info.status_code);

	/* Send headers */
	for (i = 0; i < ri.num_headers; i++)
		(void) aws_printf(conn, "%s: %s\r\n",
		    ri.http_headers[i].name,
		    ri.http_headers[i].value);
	(void) aws_write(conn, "\r\n", 2);

	/* Send chunk of data that may be read after the headers */
	conn->num_bytes_sent += aws_write(conn,
	    buf + headers_len, data_len - headers_len);

	/*
	 * Read the rest of CGI output and send to the client. If read from
	 * CGI returns 0, CGI has finished output. If it returns < 0,
	 * some read error occured (CGI process terminated unexpectedly?)
	 * If write to the client fails, the means client has disconnected
	 * unexpectedly.
	 * In all such cases, stop data exchange and do cleanup.
	 */
	do {
		n = pull(out, INVALID_SOCKET, buf, sizeof(buf));
		if (n > 0)
			n = aws_write(conn, buf, n);
		if (n > 0)
			conn->num_bytes_sent += n;
	} while (n > 0);

done:
	if (pid != (pid_t) -1)
		kill(pid, SIGTERM);
	if (fd_stdin[0] != -1)
		(void) close(fd_stdin[0]);
	if (fd_stdout[1] != -1)
		(void) close(fd_stdout[1]);

	if (in != NULL)
		(void) fclose(in);
	else if (fd_stdin[1] != -1)
		(void) close(fd_stdin[1]);

	if (out != NULL)
		(void) fclose(out);
	else if (fd_stdout[0] != -1)
		(void) close(fd_stdout[0]);
}
#endif /* !NO_CGI */

/*
 * For a given PUT path, create all intermediate subdirectories
 * for given path. Return 0 if the path itself is a directory,
 * or -1 on error, 1 if OK.
 */
static int
put_dir(const char *path)
{
	char		buf[FILENAME_MAX];
	const char	*s, *p;
	struct stat_t	st;
	size_t		len;

	for (s = p = path + 2; (p = strchr(s, '/')) != NULL; s = ++p) {
		len = p - path;
		assert(len < sizeof(buf));
		(void) memcpy(buf, path, len);
		buf[len] = '\0';

		/* Try to create intermediate directory */
		if (mg_stat(buf, &st) == -1 && mg_mkdir(buf, 0755) != 0)
			return (-1);

		/* Is path itself a directory ? */
		if (p[1] == '\0')
			return (0);
	}

	return (1);
}

static void
put_file(struct aws_connect_t *conn, const char *path)
{
	struct stat_t	st;
	FILE		*fp;
	int		rc;

	conn->request_info.status_code = mg_stat(path, &st) == 0 ? 200 : 201;

	if (aws_get_header(conn, "Range")) {
		send_error(conn, 501, "Not Implemented",
		    "%s", "Range support for PUT requests is not implemented");
	} else if ((rc = put_dir(path)) == 0) {
		(void) aws_printf(conn, "HTTP/1.1 %d OK\r\n\r\n",
		    conn->request_info.status_code);
	} else if (rc == -1) {
		send_error(conn, 500, http_500_error,
		    "put_dir(%s): %s", path, strerror(ERRNO));
	} else if ((fp = mg_fopen(path, "wb+")) == NULL) {
		send_error(conn, 500, http_500_error,
		    "fopen(%s): %s", path, strerror(ERRNO));
	} else {
		set_close_on_exec(fileno(fp));
		if (handle_request_body(conn, fp))
			(void) aws_printf(conn, "HTTP/1.1 %d OK\r\n\r\n",
			    conn->request_info.status_code);
		(void) fclose(fp);
	}
}

#if !defined(NO_SSI)
static void send_ssi_file(struct aws_connect_t *, const char *, FILE *, int);

static void
do_ssi_include(struct aws_connect_t *conn, const char *ssi, char *tag,
		int include_level)
{
	char	file_name[BUFSIZ], path[FILENAME_MAX], *p;
	FILE	*fp;

	/*
	 * sscanf() is safe here, since send_ssi_file() also uses buffer
	 * of size BUFSIZ to get the tag. So strlen(tag) is always < BUFSIZ.
	 */
	if (sscanf(tag, " virtual=\"%[^\"]\"", file_name) == 1) {
		/* File name is relative to the webserver root */
		lock_option(conn->ctx, OPT_ROOT);
		(void) mg_snprintf(conn, path, sizeof(path), "%s%c%s",
		    conn->ctx->options[OPT_ROOT], DIRSEP, file_name);
		unlock_option(conn->ctx, OPT_ROOT);
	} else if (sscanf(tag, " file=\"%[^\"]\"", file_name) == 1) {
		/*
		 * File name is relative to the webserver working directory
		 * or it is absolute system path
		 */
		(void) mg_snprintf(conn, path, sizeof(path), "%s", file_name);
	} else if (sscanf(tag, " \"%[^\"]\"", file_name) == 1) {
		/* File name is relative to the currect document */
		(void) mg_snprintf(conn, path, sizeof(path), "%s", ssi);
		if ((p = strrchr(path, DIRSEP)) != NULL)
			p[1] = '\0';
		(void) mg_snprintf(conn, path + strlen(path),
		    sizeof(path) - strlen(path), "%s", file_name);
	} else {
		cry_con(conn, "Bad SSI #include: [%s]", tag);
		return;
	}

	if ((fp = mg_fopen(path, "rb")) == NULL) {
		cry_con(conn, "Cannot open SSI #include: [%s]: fopen(%s): %s",
		    tag, path, strerror(ERRNO));
	} else {
		set_close_on_exec(fileno(fp));
		if (match_extension(path,
		    conn->ctx->options[OPT_SSI_EXTENSIONS])) {
			send_ssi_file(conn, path, fp, include_level + 1);
		} else {
			send_opened_file_stream(conn, fp,
			    UNKNOWN_CONTENT_LENGTH);
		}
		(void) fclose(fp);
	}
}

static void
do_ssi_exec(struct aws_connect_t *conn, char *tag)
{
	char	cmd[BUFSIZ];
	FILE	*fp;

	if (sscanf(tag, " \"%[^\"]\"", cmd) != 1) {
		cry_con(conn, "Bad SSI #exec: [%s]", tag);
	} else if ((fp = popen(cmd, "r")) == NULL) {
		cry_con(conn, "Cannot SSI #exec: [%s]: %s", cmd, strerror(ERRNO));
	} else {
		send_opened_file_stream(conn, fp, UNKNOWN_CONTENT_LENGTH);
		(void) pclose(fp);
	}
}

static void
send_ssi_file(struct aws_connect_t *conn, const char *path, FILE *fp,
		int include_level)
{
	char	buf[BUFSIZ];
	int	ch, len, in_ssi_tag;

	if (include_level > 10) {
		cry_con(conn, "SSI #include level is too deep (%s)", path);
		return;
	}

	in_ssi_tag = FALSE;
	len = 0;

	while ((ch = fgetc(fp)) != EOF) {
		if (in_ssi_tag && ch == '>') {
			in_ssi_tag = FALSE;
			buf[len++] = ch & 0xff;
			buf[len] = '\0';
			assert(len <= (int) sizeof(buf));
			if (len < 6 || memcmp(buf, "<!--#", 5) != 0) {
				/* Not an SSI tag, pass it */
				(void) aws_write(conn, buf, len);
			} else {
				if (!memcmp(buf + 5, "include", 7)) {
					do_ssi_include(conn, path, buf + 12,
					    include_level);
				} else if (!memcmp(buf + 5, "exec", 4)) {
					do_ssi_exec(conn, buf + 9);
				} else {
					cry_con(conn, "%s: unknown SSI "
					    "command: \"%s\"", path, buf);
				}
			}
			len = 0;
		} else if (in_ssi_tag) {
			if (len == 5 && memcmp(buf, "<!--#", 5) != 0) {
				/* Not an SSI tag */
				in_ssi_tag = FALSE;
			} else if (len == (int) sizeof(buf) - 2) {
				cry_con(conn, "%s: SSI tag is too large", path);
				len = 0;
			}
			buf[len++] = ch & 0xff;
		} else if (ch == '<') {
			in_ssi_tag = TRUE;
			if (len > 0)
				(void) aws_write(conn, buf, len);
			len = 0;
			buf[len++] = ch & 0xff;
		} else {
			buf[len++] = ch & 0xff;
			if (len == (int) sizeof(buf)) {
				(void) aws_write(conn, buf, len);
				len = 0;
			}
		}
	}

	/* Send the rest of buffered data */
	if (len > 0)
		(void) aws_write(conn, buf, len);

}

static void
send_ssi(struct aws_connect_t *conn, const char *path)
{
	FILE	*fp;

	if ((fp = mg_fopen(path, "rb")) == NULL) {
		send_error(conn, 500, http_500_error,
		    "fopen(%s): %s", path, strerror(ERRNO));
	} else {
		set_close_on_exec(fileno(fp));
		(void) aws_printf(conn, "%s", "HTTP/1.1 200 OK\r\n"
		    "Content-Type: text/html\r\nConnection: close\r\n\r\n");
		send_ssi_file(conn, path, fp, 0);
		(void) fclose(fp);
	}
}
#endif /* !NO_SSI */

/*
 * This is the heart of the AWS's logic.
 * This function is called when the request is read, parsed and validated,
 * and AWS must decide what action to take: serve a file, or
 * a directory, or call embedded function, etcetera.
 */
static void
analyze_request(struct aws_connect_t *conn)
{
	struct aws_request_t *ri = &conn->request_info;
	char			path[FILENAME_MAX];
	struct stat_t	st;
	const struct callback_t	*cb;
	char			*uri = mg_strdup(ri->uri);

	conn->uri_decoded = uri; /* backup decoded uri */

	if ((conn->request_info.query_string = strchr(uri, '?')) != NULL)
		* conn->request_info.query_string++ = '\0';

	(void) url_decode(uri, (int) strlen(uri), uri, strlen(uri) + 1, FALSE);
	remove_double_dots_and_double_slashes(uri);
	convert_uri_to_file_name(conn, uri, path, sizeof(path));

	if (!check_authorization(conn, path)) {
		send_authorization_request(conn);
	} else if ((cb = find_callback(conn->ctx, uri, -1)) != NULL) {
		if ((strcmp(ri->request_method, "POST") != 0 &&
		    strcmp(ri->request_method, "PUT") != 0) ||
		    handle_request_body(conn, NULL))
			cb->func(conn, &conn->request_info, cb->user_data);
	} else if (strstr(path, PASSWORDS_FILE_NAME)) {
		/* Do not allow to view passwords files */
		send_error(conn, 403, "Forbidden", "Access Forbidden");
	} else if ((!strcmp(ri->request_method, "PUT") ||
	    !strcmp(ri->request_method, "DELETE")) &&
	    (conn->ctx->options[OPT_AUTH_PUT] == NULL ||
	     !is_authorized_for_put(conn))) {
		send_authorization_request(conn);
	} else if (!strcmp(ri->request_method, "PUT")) {
		put_file(conn, path);
	} else if (!strcmp(ri->request_method, "DELETE")) {
		if (mg_remove(path) == 0)
			send_error(conn, 200, "OK", "");
		else
			send_error(conn, 500, http_500_error,
			    "remove(%s): %s", path, strerror(ERRNO));
	} else if (mg_stat(path, &st) != 0) {
		send_error(conn, 404, "Not Found", "%s", "File not found");
	} else if (st.is_directory && uri[strlen(uri) - 1] != '/') {
		(void) aws_printf(conn,
		    "HTTP/1.1 301 Moved Permanently\r\n"
		    "Location: %s/\r\n\r\n", ri->uri);
	} else if (st.is_directory &&
	    substitute_index_file(conn, path, sizeof(path), &st) == FALSE) {
		if (is_true(conn->ctx->options[OPT_DIR_LIST])) {
			send_directory(conn, path);
		} else {
			send_error(conn, 403, "Directory Listing Denied",
			    "Directory listing denied");
		}
#if !defined(NO_CGI)
	} else if (match_extension(path,
	    conn->ctx->options[OPT_CGI_EXTENSIONS])) {
		if (strcmp(ri->request_method, "POST") &&
		    strcmp(ri->request_method, "GET")) {
			send_error(conn, 501, "Not Implemented",
			    "Method %s is not implemented", ri->request_method);
		} else {
			send_cgi(conn, path);
		}
#endif /* NO_CGI */
#if !defined(NO_SSI)
	} else if (match_extension(path,
	    conn->ctx->options[OPT_SSI_EXTENSIONS])) {
		send_ssi(conn, path);
#endif /* NO_SSI */
	} else if (is_not_modified(conn, &st)) {
		send_error(conn, 304, "Not Modified", "");
	} else {
		send_file(conn, path, &st);
	}
}

static void
close_all_listening_sockets(struct aws_context_t *ctx)
{
	int	i;

	for (i = 0; i < ctx->num_listeners; i++)
		(void) closesocket(ctx->listeners[i].sock);
	ctx->num_listeners = 0;
}

static bool_t
set_ports_option(struct aws_context_t *ctx, const char *list)
{
	SOCKET		sock;
	struct vec_t	vec;
	struct socket_t	*listener;

	close_all_listening_sockets(ctx);
	assert(ctx->num_listeners == 0);

	while ((list = next_option(list, &vec, NULL)) != NULL) {

		listener = ctx->listeners + ctx->num_listeners;

		if (ctx->num_listeners >=
		    (int) (ARRAY_SIZE(ctx->listeners) - 1)) {
			cry_ctx(ctx,  "%s", "Too many listeninig sockets");
			return (FALSE);
		} else if ((sock = mg_open_listening_port(ctx,
		    vec.ptr, &listener->lsa)) == INVALID_SOCKET) {
			cry_ctx(ctx,  "cannot bind to %.*s", vec.len, vec.ptr);
			return (FALSE);
		} else {
			listener->sock = sock;
			ctx->num_listeners++;
		}
	}

	return (TRUE);
}

static void
log_header(const struct aws_connect_t *conn, const char *header, FILE *fp)
{
	const char	*header_value;

	if ((header_value = aws_get_header(conn, header)) == NULL) {
		(void) fprintf(fp, "%s", " -");
	} else {
		(void) fprintf(fp, " \"%s\"", header_value);
	}
}

static void
log_access(const struct aws_connect_t *conn)
{
	const struct aws_request_t *ri;
	char		date[64];

	if (conn->ctx->access_log == NULL)
		return;

	(void) strftime(date, sizeof(date), "%d/%b/%Y:%H:%M:%S %z",
	    localtime(&conn->birth_time));

	ri = &conn->request_info;

	flockfile(conn->ctx->access_log);

	(void) fprintf(conn->ctx->access_log,
	    "%s - %s [%s] \"%s %s HTTP/%d.%d\" %d %u",
	    inet_ntoa(conn->client.rsa.u.sin.sin_addr),
	    ri->remote_user == NULL ? "-" : ri->remote_user,
	    date,
	    ri->request_method ? ri->request_method : "-",
	    ri->uri ? ri->uri : "-",
	    ri->http_version_major, ri->http_version_minor,
	    conn->request_info.status_code, conn->num_bytes_sent);
	log_header(conn, "Referer", conn->ctx->access_log);
	log_header(conn, "User-Agent", conn->ctx->access_log);
	(void) fputc('\n', conn->ctx->access_log);
	(void) fflush(conn->ctx->access_log);

	funlockfile(conn->ctx->access_log);
}

static bool_t
isbyte(int n) {
	return (n >= 0 && n <= 255);
}

/*
 * Verify given socket address against the ACL.
 * Return -1 if ACL is malformed, 0 if address is disallowed, 1 if allowed.
 */
static int
check_acl(struct aws_context_t *ctx, const char *list, const struct usa_t *usa)
{
	int		a, b, c, d, n, mask, allowed;
	char		flag;
	uint32_t	acl_subnet, acl_mask, remote_ip;
	struct vec_t	vec;

	(void) memcpy(&remote_ip, &usa->u.sin.sin_addr, sizeof(remote_ip));

	/* If any ACL is set, deny by default */
	allowed = '-';

	while ((list = next_option(list, &vec, NULL)) != NULL) {

		mask = 32;

		if (sscanf(vec.ptr, "%c%d.%d.%d.%d%n",
		    &flag, &a, &b, &c, &d, &n) != 5) {
			cry_ctx(ctx, 
			    "%s: subnet must be [+|-]x.x.x.x[/x]", __func__);
			return (-1);
		} else if (flag != '+' && flag != '-') {
			cry_ctx(ctx,  "%s: flag must be + or -: [%s]",
			    __func__, vec.ptr);
			return (-1);
		} else if (!isbyte(a)||!isbyte(b)||!isbyte(c)||!isbyte(d)) {
			cry_ctx(ctx, 
			    "%s: bad ip address: [%s]", __func__, vec.ptr);
			return (-1);
		} else if (sscanf(vec.ptr + n, "/%d", &mask) == 0) {
			/* Do nothing, no mask specified */
		} else if (mask < 0 || mask > 32) {
			cry_ctx(ctx,  "%s: bad subnet mask: %d [%s]",
			    __func__, n, vec.ptr);
			return (-1);
		}

		acl_subnet = (a << 24) | (b << 16) | (c << 8) | d;
		acl_mask = mask ? 0xffffffffU << (32 - mask) : 0;

		if (acl_subnet == (ntohl(remote_ip) & acl_mask))
			allowed = flag;
	}

	return (allowed == '+' ? 1 : 0);
}

static void
add_to_set(SOCKET fd, fd_set *set, int *max_fd)
{
	FD_SET(fd, set);
	if (fd > (SOCKET) *max_fd)
		*max_fd = (int) fd;
}

/*
 * Deallocate AWS context, free up the resources
 */
static void
mg_fini(struct aws_context_t *ctx)
{
	int	i;

	close_all_listening_sockets(ctx);

	/* Wait until all threads finish */
	(void) pthread_mutex_lock(&ctx->thr_mutex);
	while (ctx->num_threads > 0)
		(void) pthread_cond_wait(&ctx->thr_cond, &ctx->thr_mutex);
	(void) pthread_mutex_unlock(&ctx->thr_mutex);

	/* Deallocate all registered callbacks */
	for (i = 0; i < ctx->num_callbacks; i++)
		if (ctx->callbacks[i].uri_regex != NULL)
			free(ctx->callbacks[i].uri_regex);

	/* Deallocate all options */
	for (i = 0; i < NUM_OPTIONS; i++)
		if (ctx->options[i] != NULL)
			free(ctx->options[i]);

	/* Close log files */
	if (ctx->access_log)
		(void) fclose(ctx->access_log);
	if (ctx->error_log)
		(void) fclose(ctx->error_log);

	/* Deallocate mutexes and condvars */
	for (i = 0; i < NUM_OPTIONS; i++)
		(void) pthread_mutex_destroy(&ctx->opt_mutex[i]);

	(void) pthread_mutex_destroy(&ctx->thr_mutex);
	(void) pthread_mutex_destroy(&ctx->bind_mutex);
	(void) pthread_cond_destroy(&ctx->thr_cond);
	(void) pthread_cond_destroy(&ctx->empty_cond);
	(void) pthread_cond_destroy(&ctx->full_cond);

	/* Signal aws_stop() that we're done */
	ctx->stop_flag = 2;
}

#if !defined(_WIN32)
static bool_t
set_uid_option(struct aws_context_t *ctx, const char *uid)
{
	struct passwd	*pw;
	int		retval = FALSE;

	if ((pw = getpwnam(uid)) == NULL)
		cry_ctx(ctx,  "%s: unknown user [%s]", __func__, uid);
	else if (setgid(pw->pw_gid) == -1)
		cry_ctx(ctx,  "%s: setgid(%s): %s",
		    __func__, uid, strerror(errno));
	else if (setuid(pw->pw_uid) == -1)
		cry_ctx(ctx,  "%s: setuid(%s): %s",
		    __func__, uid, strerror(errno));
	else
		retval = TRUE;

	return (retval);
}
#endif /* !_WIN32 */


static bool_t
open_log_file(struct aws_context_t *ctx, FILE **fpp, const char *path)
{
	bool_t	retval = TRUE;

	if (*fpp != NULL)
		(void) fclose(*fpp);

	if (path == NULL) {
		*fpp = NULL;
	} else if ((*fpp = mg_fopen(path, "a")) == NULL) {
		cry_ctx(ctx,  "%s(%s): %s", __func__, path, strerror(errno));
		retval = FALSE;
	} else {
		set_close_on_exec(fileno(*fpp));
	}

	return (retval);
}

static bool_t
set_alog_option(struct aws_context_t *ctx, const char *path)
{
	return (open_log_file(ctx, &ctx->access_log, path));
}

static bool_t
set_elog_option(struct aws_context_t *ctx, const char *path)
{
	return (open_log_file(ctx, &ctx->error_log, path));
}

static bool_t
set_gpass_option(struct aws_context_t *ctx, const char *path)
{
	struct stat_t	mgstat;
	ctx = NULL;
	return (mg_stat(path, &mgstat) == 0);
}

static bool_t
set_max_threads_option(struct aws_context_t *ctx, const char *str)
{
	ctx->max_threads = atoi(str);
	return (TRUE);
}

static bool_t
set_acl_option(struct aws_context_t *ctx, const char *acl)
{
	struct usa_t fake;
	return (check_acl(ctx, acl, &fake) != -1);
}

static void admin_page(struct aws_connect_t *,
		const struct aws_request_t *, void *);
static bool_t
set_admin_uri_option(struct aws_context_t *ctx, const char *uri)
{
	aws_set_uri_callback(ctx, uri, &admin_page, NULL);
	return (TRUE);
}

/*
 * Check if the comma-separated list of options has a format of key-value
 * pairs: "k1=v1,k2=v2". Return FALSE if any entry has invalid key or value.
 */
static bool_t
set_kv_list_option(struct aws_context_t *ctx, const char *str)
{
	const char	*list;
	struct vec_t	key, value;

	list = str;
	while ((list = next_option(list, &key, &value)) != NULL)
		if (key.len == 0 || value.len == 0) {
			cry_ctx(ctx,  "Invalid list specified: [%s], "
			    "expecting key1=value1,key2=value2,...", str);
			return (FALSE);
		}

	return (TRUE);
}

static const struct option_t known_options[] = {
	{"root", "\tWeb root directory", NULL, OPT_ROOT, NULL},
	{"index_files",	"Index files", "index.html,index.htm,index.cgi",
		OPT_INDEX_FILES, NULL},
	{"ports", "Listening ports", NULL,
		OPT_PORTS, &set_ports_option},
	{"dir_list", "Directory listing", "yes",
		OPT_DIR_LIST, NULL},
	{"protect", "URI to htpasswd mapping", NULL,
		OPT_PROTECT, &set_kv_list_option},
#if !defined(NO_CGI)
	{"cgi_ext", "CGI extensions", ".cgi,.pl,.php",
		OPT_CGI_EXTENSIONS, NULL},
	{"cgi_interp", "CGI interpreter to use with all CGI scripts", NULL,
		OPT_CGI_INTERPRETER, NULL},
	{"cgi_env", "Custom CGI enviroment variables", NULL,
		OPT_CGI_ENV, &set_kv_list_option},
#endif /* NO_CGI */
	{"ssi_ext", "SSI extensions", ".shtml,.shtm",
		OPT_SSI_EXTENSIONS, NULL},
	{"auth_realm", "Authentication domain name", "mydomain.com",
		OPT_AUTH_DOMAIN, NULL},
	{"auth_gpass", "Global passwords file", NULL,
		OPT_AUTH_GPASSWD, &set_gpass_option},
	{"auth_PUT", "PUT,DELETE auth file", NULL,
		OPT_AUTH_PUT, NULL},
#if !defined(_WIN32)
	{"uid", "\tRun as user", NULL, OPT_UID, &set_uid_option},
#endif /* !_WIN32 */
	{"access_log", "Access log file", NULL,
		OPT_ACCESS_LOG, &set_alog_option},
	{"error_log", "Error log file", NULL,
		OPT_ERROR_LOG, &set_elog_option},
	{"aliases", "Path=URI mappings", NULL,
		OPT_ALIASES, &set_kv_list_option},
	{"admin_uri", "Administration page URI", NULL,
		OPT_ADMIN_URI, &set_admin_uri_option},
	{"acl", "\tAllow/deny IP addresses/subnets", NULL,
		OPT_ACL, &set_acl_option},
	{"max_threads", "Maximum simultaneous threads to spawn", "100",
		OPT_MAX_THREADS, &set_max_threads_option},
	{"idle_time", "Time in seconds connection stays idle", "10",
		OPT_IDLE_TIME, NULL},
	{"mime_types", "Comma separated list of ext=mime_type pairs", NULL,
		OPT_MIME_TYPES, &set_kv_list_option},
	{NULL, NULL, NULL, 0, NULL}
};

static const struct option_t *
find_opt(const char *opt_name)
{
	int	i;

	for (i = 0; known_options[i].name != NULL; i++)
		if (!strcmp(opt_name, known_options[i].name))
			return (known_options + i);

	return (NULL);
}

int
aws_set_option(struct aws_context_t *ctx, const char *opt, const char *val)
{
	const struct option_t	*option;
	int			i, retval;

	DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: [%s]->[%s]", __func__, opt, val));
	if (opt != NULL && (option = find_opt(opt)) != NULL) {
		i = (int) (option - known_options);
		lock_option(ctx, i);

		if (option->setter != NULL)
			retval = option->setter(ctx, val);
		else
			retval = TRUE;

		/* Free old value if any */
		if (ctx->options[option->index] != NULL)
			free(ctx->options[option->index]);

		/* Set new option value */
		ctx->options[option->index] = val ? mg_strdup(val) : NULL;
		unlock_option(ctx, i);

		if (retval == FALSE)
			cry_ctx(ctx,  "%s(%s): failure", __func__, opt);
	} else {
		cry_ctx(ctx,  "%s: No such option: [%s]", __func__, opt);
		retval = -1;
	}

	return (retval);
}


const char *
aws_get_option(const struct aws_context_t *ctx, const char *option_name)
{
	const struct option_t	*option;

	if ((option = find_opt(option_name)) != NULL)
		return (ctx->options[option->index]);
	else
		return (NULL);
}

static void
admin_page(struct aws_connect_t *conn, const struct aws_request_t *ri,
			   void *user_data)
{
	const struct option_t	*option;
	char		*name, *val;
	const char  *option_value;

	user_data = NULL; /* Unused */

	(void) aws_printf(conn,
	"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n\r\n"
			"<html><body><h1>AWS v.1</h1>");

	if (!strcmp(ri->request_method, "POST")) {
		name = aws_find_post_var(conn, "o");
		val = aws_find_post_var(conn, "v");
		if (aws_set_option(conn->ctx,
		    name, val) == -1) {
			(void) aws_printf(conn,
			    "<p style=\"background: red\">Error setting "
			    "option \"%s\"</p>",
			    name ? name : "(null)");
		} else {
			(void) aws_printf(conn,
			    "<p style=\"color: green\">Saved: %s=%s</p>",
			    name, val ? val : "NULL");
		}
		if (name) aws_free(name);
		if (val) aws_free(val);
	}

	/* Print table with all options */
	(void) aws_printf(conn, "%s", "<table border=\"1\""
			"<tr><th>Option</th><th>Description</th>"
					"<th colspan=2>Value</th></tr>");

	for (option = known_options; option->name != NULL; option++) {
		option_value = aws_get_option(conn->ctx, option->name);
		if (option_value == NULL)
			option_value = "";
		(void) aws_printf(conn,
		    "<form method=post><tr><td>%s</td><td>%s</td>"
		    "<input type=hidden name=o value='%s'>"
		    "<td><input type=text name=v value='%s'></td>"
		    "<td><input type=submit value=save></td></form></tr>",
		    option->name, option->description,
		    option->name, option_value);
	}

	(void) aws_printf(conn, "%s", "</table></body></html>");
}

static void
free_connection_memories(struct aws_connect_t *conn)
{
	if (conn->request_info.remote_user != NULL) {
		free((void *) conn->request_info.remote_user);
		conn->request_info.remote_user = NULL;
	}
	if (conn->free_post_data && conn->request_info.post_data != NULL) {
		free((void *) conn->request_info.post_data);
		conn->request_info.post_data = NULL;
	}
	if (conn->uri_decoded) {
		free((void *) conn->uri_decoded);
		conn->uri_decoded = NULL;
	}
}

static void
close_socket_gracefully(struct aws_connect_t *conn, SOCKET sock)
{
	char	buf[BUFSIZ];
	int	n;

	/* Send FIN to the client */
	(void) shutdown(sock, SHUT_WR);
	set_non_blocking_mode(conn, sock);

	/*
	 * Read and discard pending data. If we do not do that and close the
	 * socket, the data in the send buffer may be discarded. This
	 * behaviour is seen on Windows, when client keeps sending data
	 * when server decide to close the connection; then when client
	 * does recv() it gets no data back.
	 */
	do {
		n = pull(NULL, sock, buf, sizeof(buf));
	} while (n > 0);

	/* Now we know that our FIN is ACK-ed, safe to close */
	(void) closesocket(sock);
}

static void
close_connection(struct aws_connect_t *conn)
{
	free_connection_memories(conn);

	if (conn->client.sock != INVALID_SOCKET)
		close_socket_gracefully(conn, conn->client.sock);
}

static void
reset_connection_attributes(struct aws_connect_t *conn)
{
	free_connection_memories(conn);
	conn->free_post_data = FALSE;
	conn->request_info.status_code = -1;
	conn->num_bytes_sent = 0;
	(void) memset(&conn->request_info, 0, sizeof(conn->request_info));
	conn->is_form_data = 0;
	conn->num_fields = 0;
	(void) memset( conn->form_fields, 0, sizeof(conn->form_fields));
}

static void
shift_to_next(struct aws_connect_t *conn, char *buf, int req_len, int *nread)
{
	uint32_t	cl;
	int		over_len, body_len;

	cl = get_content_length(conn);
	over_len = *nread - req_len;
	assert(over_len >= 0);

	if (cl == UNKNOWN_CONTENT_LENGTH) {
		body_len = 0;
	} else if (cl < (uint32_t) over_len) {
		body_len = (int) cl;
	} else {
		body_len = over_len;
	}

	*nread -= req_len + body_len;
	(void) memmove(buf, buf + req_len + body_len, *nread);
}

static void
process_new_connection(struct aws_connect_t *conn)
{
	struct aws_request_t *ri = &conn->request_info;
	char	buf[MAX_REQUEST_SIZE];
	int	request_len, nread;

	nread = 0;
	reset_connection_attributes(conn);

	/* If next request is not pipelined, read it in */
	if ((request_len = get_request_len(buf, (size_t) nread)) == 0)
		request_len = read_request(NULL, conn->client.sock,
		    buf, sizeof(buf), &nread);
	assert(nread >= request_len);

	if (request_len <= 0)
		return;	/* Remote end closed the connection */

	/* 0-terminate the request: parse_request uses sscanf */
	buf[request_len - 1] = '\0';

	if (parse_http_request(buf, ri, &conn->client.rsa)) {
		if (ri->http_version_major != 1 ||
		    (ri->http_version_major == 1 &&
		    (ri->http_version_minor < 0 ||
		    ri->http_version_minor > 1))) {
			send_error(conn, 505,
			    "HTTP version not supported",
			    "%s", "Weird HTTP version");
			log_access(conn);
		} else {
			ri->post_data = buf + request_len;
			ri->post_data_len = nread - request_len;
			conn->birth_time = time(NULL);
			analyze_request(conn);
			log_access(conn);
			shift_to_next(conn, buf, request_len, &nread);
		}
	} else {
		/* Do not put garbage in the access log */
		send_error(conn, 400, "Bad Request",
		    "Can not parse request: [%.*s]", nread, buf);
	}

}

/*
 * Worker threads take accepted socket from the queue
 */
static bool_t
get_socket(struct aws_context_t *ctx, struct socket_t *sp)
{
	struct timespec	ts;

	(void) pthread_mutex_lock(&ctx->thr_mutex);
	DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: thread %p: going idle",
	    __func__, (void *) pthread_self()));

	/* If the queue is empty, wait. We're idle at this point. */
	ctx->num_idle++;
	while (ctx->sq_head == ctx->sq_tail) {
		ts.tv_nsec = 0;
		ts.tv_sec = time(NULL) + atoi(ctx->options[OPT_IDLE_TIME]) + 1;
		if (pthread_cond_timedwait(&ctx->empty_cond,
		    &ctx->thr_mutex, &ts) != 0) {
			/* Timeout! release the mutex and return */
			(void) pthread_mutex_unlock(&ctx->thr_mutex);
			return (FALSE);
		}
	}
	assert(ctx->sq_head > ctx->sq_tail);

	/* We're going busy now: got a socket to process! */
	ctx->num_idle--;

	/* Copy socket from the queue and increment tail */
	*sp = ctx->queue[ctx->sq_tail % ARRAY_SIZE(ctx->queue)];
	ctx->sq_tail++;
	DEBUG_TRACE((DEBUG_MGS_PREFIX
	    "%s: thread %p grabbed socket %d, going busy",
	    __func__, (void *) pthread_self(), sp->sock));

	/* Wrap pointers if needed */
	while (ctx->sq_tail > (int) ARRAY_SIZE(ctx->queue)) {
		ctx->sq_tail -= ARRAY_SIZE(ctx->queue);
		ctx->sq_head -= ARRAY_SIZE(ctx->queue);
	}

	pthread_cond_signal(&ctx->full_cond);
	(void) pthread_mutex_unlock(&ctx->thr_mutex);

	return (TRUE);
}

static void
worker_thread(struct aws_context_t *ctx)
{
	struct aws_connect_t	conn;

	DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: thread %p starting",
	    __func__, (void *) pthread_self()));

	(void) memset(&conn, 0, sizeof(conn));

	while (get_socket(ctx, &conn.client) == TRUE) {
		conn.birth_time = time(NULL);
		conn.ctx = ctx;

		process_new_connection(&conn);
		close_connection(&conn);
	}

	/* Signal master that we're done with connection and exiting */
	pthread_mutex_lock(&conn.ctx->thr_mutex);
	conn.ctx->num_threads--;
	conn.ctx->num_idle--;
	pthread_cond_signal(&conn.ctx->thr_cond);
	assert(conn.ctx->num_threads >= 0);
	pthread_mutex_unlock(&conn.ctx->thr_mutex);

	DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: thread %p exiting",
	    __func__, (void *) pthread_self()));
}

/*
 * Master thread adds accepted socket to a queue
 */
static void
put_socket(struct aws_context_t *ctx, const struct socket_t *sp)
{
	(void) pthread_mutex_lock(&ctx->thr_mutex);

	/* If the queue is full, wait */
	while (ctx->sq_head - ctx->sq_tail >= (int) ARRAY_SIZE(ctx->queue))
		(void) pthread_cond_wait(&ctx->full_cond, &ctx->thr_mutex);
	assert(ctx->sq_head - ctx->sq_tail < (int) ARRAY_SIZE(ctx->queue));

	/* Copy socket to the queue and increment head */
	ctx->queue[ctx->sq_head % ARRAY_SIZE(ctx->queue)] = *sp;
	ctx->sq_head++;
	DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: queued socket %d",
	    __func__, sp->sock));

	/* If there are no idle threads, start one */
	if (ctx->num_idle == 0 && ctx->num_threads < ctx->max_threads) {
		if (start_thread(ctx,
		    (mg_thread_func_t) worker_thread, ctx) != 0)
			cry_ctx(ctx,  "Cannot start thread: %d", ERRNO);
		else
			ctx->num_threads++;
	}

	pthread_cond_signal(&ctx->empty_cond);
	(void) pthread_mutex_unlock(&ctx->thr_mutex);
}

static void
accept_new_connection(const struct socket_t *listener, struct aws_context_t *ctx)
{
	struct socket_t	accepted;

	accepted.rsa.len = sizeof(accepted.rsa.u.sin);
	accepted.lsa = listener->lsa;
	if ((accepted.sock = accept(listener->sock,
	    &accepted.rsa.u.sa, &accepted.rsa.len)) == INVALID_SOCKET)
		return;

	lock_option(ctx, OPT_ACL);
	if (ctx->options[OPT_ACL] != NULL &&
	    !check_acl(ctx, ctx->options[OPT_ACL], &accepted.rsa)) {
		cry_ctx(ctx,  "%s: %s is not allowed to connect",
		    __func__, inet_ntoa(accepted.rsa.u.sin.sin_addr));
		(void) closesocket(accepted.sock);
		unlock_option(ctx, OPT_ACL);
		return;
	}
	unlock_option(ctx, OPT_ACL);

	/* Put accepted socket structure into the queue */
	DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: accepted socket %d",
	    __func__, accepted.sock));

	put_socket(ctx, &accepted);
}

static void
master_thread(struct aws_context_t *ctx)
{
	fd_set		read_set;
	struct timeval	tv;
	int		i, max_fd;

	while (ctx->stop_flag == 0) {
		FD_ZERO(&read_set);
		max_fd = -1;

		/* Add listening sockets to the read set */
		lock_option(ctx, OPT_PORTS);
		for (i = 0; i < ctx->num_listeners; i++)
			add_to_set(ctx->listeners[i].sock, &read_set, &max_fd);
		unlock_option(ctx, OPT_PORTS);

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		if (select(max_fd + 1, &read_set, NULL, NULL, &tv) < 0) {
#ifdef _WIN32
			/*
			 * On windows, if read_set and write_set are empty,
			 * select() returns "Invalid parameter" error
			 * (at least on my Windows XP Pro). So in this case,
			 * we sleep here.
			 */
			sleep(1);
#endif /* _WIN32 */
		} else {
			lock_option(ctx, OPT_PORTS);
			for (i = 0; i < ctx->num_listeners; i++)
				if (FD_ISSET(ctx->listeners[i].sock, &read_set))
					accept_new_connection(
					    ctx->listeners + i, ctx);
			unlock_option(ctx, OPT_PORTS);
		}
	}

	/* Stop signal received: somebody called aws_stop. Quit. */
	mg_fini(ctx);
}

void
aws_stop(struct aws_context_t *ctx)
{
	ctx->stop_flag = 1;

	/* Wait until mg_fini() stops */
	while (ctx->stop_flag != 2)
		(void) sleep(1);

	assert(ctx->num_threads == 0);
	free((void *) ctx);

#if defined(_WIN32)
	(void) WSACleanup();
#endif /* _WIN32 */
}

struct aws_context_t *
aws_start(void)
{
	struct aws_context_t	*ctx;
	const struct option_t	*option;
	char			web_root[FILENAME_MAX];
	int			i;

#if defined(_WIN32)
	WSADATA data;
	WSAStartup(MAKEWORD(2,2), &data);
#endif /* _WIN32 */

	if ((ctx = (struct aws_context_t *) calloc(1, sizeof(*ctx))) == NULL) {
		cry_ctx(ctx,  "cannot allocate AWS context");
		return (NULL);
	}

	ctx->error_log = stderr;

	/* Initialize options. First pass: set default option values */
	for (option = known_options; option->name != NULL; option++)
		ctx->options[option->index] = option->default_value == NULL ?
			NULL : mg_strdup(option->default_value);

	/* Call setter functions */
	for (option = known_options; option->name != NULL; option++)
		if (option->setter != NULL &&
		    ctx->options[option->index] != NULL)
			if (option->setter(ctx,
			    ctx->options[option->index]) == FALSE) {
				mg_fini(ctx);
				return (NULL);
			}

	/* Initial document root is set to current working directory */
	if (ctx->options[OPT_ROOT] == NULL) {
		if (mg_getcwd(web_root, sizeof(web_root)) == NULL) {
			cry_ctx(ctx,  "%s: getcwd: %s",
			    __func__, strerror(errno));
			mg_strlcpy(web_root, ".", sizeof(web_root));
		}
		ctx->options[OPT_ROOT] = mg_strdup(web_root);
	}

	DEBUG_TRACE((DEBUG_MGS_PREFIX "%s: root [%s]",
	    __func__, ctx->options[OPT_ROOT]));

#if !defined(_WIN32)
	/*
	 * Ignore SIGPIPE signal, so if browser cancels the request, it
	 * won't kill the whole process.
	 */
	(void) signal(SIGPIPE, SIG_IGN);
#endif /* _WIN32 */

	/* Initialize options mutexes */
	for (i = 0; i < NUM_OPTIONS; i++)
		(void) pthread_mutex_init(&ctx->opt_mutex[i], NULL);

	(void) pthread_mutex_init(&ctx->thr_mutex, NULL);
	(void) pthread_mutex_init(&ctx->bind_mutex, NULL);
	(void) pthread_cond_init(&ctx->thr_cond, NULL);
	(void) pthread_cond_init(&ctx->empty_cond, NULL);
	(void) pthread_cond_init(&ctx->full_cond, NULL);

	/* Start master (listening) thread */
	start_thread(ctx, (mg_thread_func_t) master_thread, ctx);

	return (ctx);
}
