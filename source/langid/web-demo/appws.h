#ifndef __APPWS_H_2009_12__
#define	__APPWS_H_2009_12__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

struct aws_context_t;	/* Handle for the web service itself	*/
struct aws_connect_t;	/* Handle for the individual connection	*/


/*
 * This structure contains full information about the HTTP request.
 * It is passed to the user-specified callback function as a parameter.
 */
struct aws_request_t {
	char	*request_method;	/* "GET", "POST", etc	*/
	char	*uri;			/* Normalized URI	*/
	char	*query_string;		/* \0 - terminated	*/
	char	*post_data;		/* POST data buffer	*/
	char	*remote_user;		/* Authenticated user	*/
	long	remote_ip;		/* Client's IP address	*/
	int	remote_port;		/* Client's port	*/
	int	post_data_len;		/* POST buffer length	*/
	int	http_version_major;
	int	http_version_minor;
	int	status_code;		/* HTTP status code	*/
	int	num_headers;		/* Number of headers	*/
	struct header_str {
		char	*name;		/* HTTP header name	*/
		char	*value;		/* HTTP header value	*/
	} http_headers[64];		/* Maximum 64 headers	*/
};


/*
 * Start the web server.
 * This must be the first function called by the application.
 * It creates a serving thread, and returns a context structure that
 * can be used to alter the configuration, and stop the server.
 */
struct aws_context_t *aws_start(void);


/*
 * Stop the web server.
 * Must be called last, when an application wants to stop the web server and
 * release all associated resources. This function blocks until all AWS
 * threads are stopped. Context pointer becomes invalid.
 */
void aws_stop(struct aws_context_t *);


/*
 * User-defined callback function prototype for URI handling, error handling,
 * or logging server messages.
 */
typedef void (*aws_callback_t)(struct aws_connect_t *,
		const struct aws_request_t *info, void *user_data);

/*
 * Register URI handler.
 * It is possible to handle many URIs if using * in the uri_regex, which
 * matches zero or more characters. user_data pointer will be passed to the
 * handler as a third parameter. If func is NULL, then the previously installed
 * handler for this uri_regex is removed.
 */
void aws_set_uri_callback(struct aws_context_t *ctx, const char *uri_regex,
		aws_callback_t func, void *user_data);


/*
 * Register HTTP error handler.
 * An application may use that function if it wants to customize the error
 * page that user gets on the browser (for example, 404 File Not Found message).
 * It is possible to specify a error handler for all errors by passing 0 as
 * error_code. That '0' error handler must be set last, if more specific error
 * handlers are also used. The actual error code value can be taken from
 * the request info structure that is passed to the callback.
 */
void aws_set_err_callback(struct aws_context_t *ctx, int error_code,
		aws_callback_t func, void *user_data);


/*
 * Send data to the browser.
 * Return number of bytes sent. If the number of bytes sent is less then
 * requested or equals to -1, network error occured, usually meaning the
 * remote side has closed the connection.
 */
int aws_write(struct aws_connect_t *, const void *buf, int len);


/*
 * Send data to the browser using printf() semantics.
 * Works exactly like aws_write(), but allows to do message formatting.
 * Note that aws_printf() uses internal buffer of size MAX_REQUEST_SIZE
 * (8 Kb by default) as temporary message storage for formatting. Do not
 * print data that is bigger than that, otherwise it will be truncated.
 * Return number of bytes sent.
 */
int aws_printf(struct aws_connect_t *, const char *fmt, ...);


/*
 * An inherited function for aws_write and aws_printf, send a local file
 * directly to clients with "header" information as a reply.
 */
int aws_send_file(struct aws_connect_t *, const char *fn);


/*
 * Get the value of particular HTTP header.
 * This is a helper function. It traverses request_info->http_headers array,
 * and if the header is present in the array, returns its value. If it is
 * not present, NULL is returned.
 */
const char *aws_get_header(const struct aws_connect_t *, const char *hdr_name);


/*
 * Get a value of particular form variable specified in query string 
 * (whatever comes after '?' in the URL) or a POST buffer. 
 * or, Return posted file descriptor specified in the POST form.
 * Return value:
 *	NULL      if the variable is not found
 *	non-NULL  if found. 
 * 
 * NOTE: the returned value is dynamically allocated, it is an application's 
 *		 responsibility to aws_free() the variable when no longer needed. 
 */
char *aws_find_get_var(const struct aws_connect_t *, const char *var_name);

char *aws_find_post_var(const struct aws_connect_t *, const char *var_name);

struct aws_file_t {
	char	*name;		/* field name */
	char	*fn;		/* file name in data header */
	char	*mime;		/* mime type for the data */
	char	*data;		/* address of file content */
	int		data_len;
};

struct aws_file_t*
	aws_find_post_file(const struct aws_connect_t *, const char *var_name);

/*
 * Free up memory returned by aws_find_XXX_XXX().
 */
void aws_free(void *var);


/*
 * Save a buffer into a local file, fn specify its physical path
 */
int aws_save_as(char *data, int len, const char *fn);


/*
 * Return current value of a particular option.
 */
const char *aws_get_option(const struct aws_context_t *, const char *option_name);


/*
 * Set a value for a particular option.
 * AWS makes an internal copy of the option value string, which must be
 * valid nul-terminated ASCII or UTF-8 string. It is safe to change any option
 * at any time. The order of setting various options is also irrelevant with
 * one exception: if "ports" option contains SSL listening ports, a "ssl_cert"
 * option must be set BEFORE the "ports" option.
 * Return value:
 *	-1 if option is unknown
 *	0  if aws_set_option() failed
 *	1  if aws_set_option() succeeded 
 */
int aws_set_option(struct aws_context_t *, const char *opt_name, const char *value);


/*
 * Add, edit or delete the entry in the passwords file.
 * This function allows an application to manipulate .htpasswd files on the
 * fly by adding, deleting and changing user records. This is one of the two
 * ways of implementing authentication on the server side. For another,
 * cookie-based way please refer to the examples/authentication.c in the
 * source tree.
 * If password is not NULL, entry is added (or modified if already exists).
 * If password is NULL, entry is deleted. Return:
 *	1 on success
 *	0 on error 
 * 
 * NOTE: path can be a directory_name or be a file_name.
 * 
 */
int aws_update_password(struct aws_context_t *ctx, const char *path,
		const char *user_name, const char *password);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* __APPWS_H_2009_12__ */
