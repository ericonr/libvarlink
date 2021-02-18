// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error codes retured by the library functions as negative values.
 */
enum {
        VARLINK_ERROR_PANIC = 1,
        VARLINK_ERROR_INVALID_INTERFACE,
        VARLINK_ERROR_INVALID_ADDRESS,
        VARLINK_ERROR_INVALID_IDENTIFIER,
        VARLINK_ERROR_INVALID_TYPE,
        VARLINK_ERROR_INTERFACE_NOT_FOUND,
        VARLINK_ERROR_METHOD_NOT_FOUND,
        VARLINK_ERROR_CANNOT_CONNECT,
        VARLINK_ERROR_CANNOT_LISTEN,
        VARLINK_ERROR_CANNOT_ACCEPT,
        VARLINK_ERROR_SENDING_MESSAGE,
        VARLINK_ERROR_RECEIVING_MESSAGE,
        VARLINK_ERROR_INVALID_INDEX,
        VARLINK_ERROR_UNKNOWN_FIELD,
        VARLINK_ERROR_READ_ONLY,
        VARLINK_ERROR_INVALID_JSON,
        VARLINK_ERROR_INVALID_MESSAGE,
        VARLINK_ERROR_INVALID_CALL,
        VARLINK_ERROR_ACCESS_DENIED,
        VARLINK_ERROR_CONNECTION_CLOSED,
        VARLINK_ERROR_MAX
};

/*
 * Keywords/flags of a method call.
 */
enum {
        VARLINK_CALL_MORE = 1,
        VARLINK_CALL_ONEWAY = 2
};

/*
 * Keywords/flags of a method reply.
 */
enum {
        VARLINK_REPLY_CONTINUES = 1
};

/*
 * Objects and arrays represent basic data types corresponding with JSON
 * objects and arrays.
 */
typedef struct VarlinkObject VarlinkObject;
typedef struct VarlinkArray VarlinkArray;

/*
 * A varlink service exports a set of interfaces and listens on a varlink
 * address for incoming calls.
 */
typedef struct VarlinkService VarlinkService;

/*
 * An incoming method call.
 */
typedef struct VarlinkCall VarlinkCall;

/*
 * A connection from a client to a service.
 */
typedef struct VarlinkConnection VarlinkConnection;

/*
 * Called when a client calls a method of a service.
 */
typedef long (*VarlinkMethodCallback)(VarlinkService *service,
                                      VarlinkCall *call,
                                      VarlinkObject *parameters,
                                      uint64_t flags,
                                      void *userdata);

/*
 * Called when the server closes a connection.
 */
typedef void (*VarlinkConnectionClosedFunc)(VarlinkConnection *connection,
                                            void *userdata);

/*
 * Called when a client closes the connection. Call might be NULL,
 * if there is no pending call anymore but the callback still active.
 */
typedef void (*VarlinkCallConnectionClosed)(VarlinkCall *call,
                                            void *userdata);

/*
 * Called when a client receives a reply to its call.
 */
typedef long (*VarlinkReplyFunc)(VarlinkConnection *connection,
                                 const char *error,
                                 VarlinkObject *parameters,
                                 uint64_t flags,
                                 void *userdata);

/*
 * Translate the error code into the error ID string
 */
const char *varlink_error_string(long error);

/*
 * Create a new empty object.
 */
long varlink_object_new(VarlinkObject **objectp);

/*
 * Createa new object by reading its data from a JSON string.
 */
long varlink_object_new_from_json(VarlinkObject **objectp, const char *json);

/*
 * Decrement the reference count of an array. Dropping the last
 * reference frees all ressources.
 *
 * Returns NULL;
 */
VarlinkObject *varlink_object_unref(VarlinkObject *object);

/*
 * varlink_object_unref() to be used with the cleanup attribute.
 */
void varlink_object_unrefp(VarlinkObject **objectp);

/*
 * Increment the reference count of an array.
 *
 * Returns the same VarlinkArray.
 */
VarlinkObject *varlink_object_ref(VarlinkObject *object);

/*
 * Write the data of an object formatted as JSON to a newly allocated
 * string.
 *
 * Returns the length of the allocated string or a negative VARLINK_ERROR.
 */
long varlink_object_to_json(VarlinkObject *object, char **stringp);

/*
 * Retrieve an array of strings with the filed names of the object.
 */
long varlink_object_get_field_names(VarlinkObject *object, const char ***namesp);

/*
 * Get values from an object.
 */
long varlink_object_set_null(VarlinkObject *object, const char *field);
long varlink_object_get_bool(VarlinkObject *object, const char *field, bool *bp);
long varlink_object_get_int(VarlinkObject *object, const char *field, int64_t *ip);
long varlink_object_get_float(VarlinkObject *object, const char *field, double *fp);
long varlink_object_get_string(VarlinkObject *object, const char *field, const char **stringp);
long varlink_object_get_array(VarlinkObject *object, const char *field, VarlinkArray **arrayp);
long varlink_object_get_object(VarlinkObject *object, const char *field, VarlinkObject **nestedp);

/*
 * Set values of an object.
 */
long varlink_object_set_bool(VarlinkObject *object, const char *field, bool b);
long varlink_object_set_int(VarlinkObject *object, const char *field, int64_t i);
long varlink_object_set_float(VarlinkObject *object, const char *field, double f);
long varlink_object_set_string(VarlinkObject *object, const char *field, const char *string);
long varlink_object_set_array(VarlinkObject *object, const char *field, VarlinkArray *array);
long varlink_object_set_object(VarlinkObject *object, const char *field, VarlinkObject *nested);

/*
 * Create a new array.
 */
long varlink_array_new(VarlinkArray **arrayp);

/*
 * Increment the reference count of an array.
 *
 * Returns the same VarlinkArray.
 */
VarlinkArray *varlink_array_ref(VarlinkArray *array);

/*
 * Decrement the reference count of an array. Dropping the last
 * reference frees all ressources.
 *
 * Returns NULL;
 */
VarlinkArray *varlink_array_unref(VarlinkArray *array);

/*
 * varlink_array_unref() to be used with the cleanup attribute.
 */
void varlink_array_unrefp(VarlinkArray **arrayp);

/*
 * Returns the number of elements of an array.
 */
unsigned long varlink_array_get_n_elements(VarlinkArray *array);

/*
 * Extract a value of the array element at index.
 *
 * Returns 0 or a negative VARLINK_ERROR.
 */
long varlink_array_get_bool(VarlinkArray *array, unsigned long index, bool *bp);
long varlink_array_get_int(VarlinkArray *array, unsigned long index, int64_t *ip);
long varlink_array_get_float(VarlinkArray *array, unsigned long index, double *fp);
long varlink_array_get_string(VarlinkArray *array, unsigned long index, const char **stringp);
long varlink_array_get_array(VarlinkArray *array, unsigned long index, VarlinkArray **elementp);
long varlink_array_get_object(VarlinkArray *array, unsigned long index, VarlinkObject **objectp);

/*
 * Appends a value to the end of an array.
 *
 * Return 0 or a negative VARLINK_ERROR.
 */
long varlink_array_append_null(VarlinkArray *array);
long varlink_array_append_bool(VarlinkArray *array, bool b);
long varlink_array_append_int(VarlinkArray *array, int64_t i);
long varlink_array_append_float(VarlinkArray *array, double f);
long varlink_array_append_string(VarlinkArray *array, const char *string);
long varlink_array_append_array(VarlinkArray *array, VarlinkArray *element);
long varlink_array_append_object(VarlinkArray *array, VarlinkObject *object);

/*
 * Create a new varlink service with the given name and version and
 * listen for requests on the given address.
 *
 * If listen_fd is not -1, it must be an fd that was created for the
 * same address with varlink_listen().
 *
 * Returns 0 or a a negative varlink error.
 */
long varlink_service_new(VarlinkService **servicep,
                         const char *vendor,
                         const char *product,
                         const char *version,
                         const char *url,
                         const char *address,
                         int listen_fd);

/*
 * Create a new varlink service that handles all incoming requests with
 * the supplied callback, i.e., varlink_service_add_interface() does not
 * work on this object.
 */
long varlink_service_new_raw(VarlinkService **servicep,
                             const char *address,
                             int listen_fd,
                             VarlinkMethodCallback callback,
                             void *userdata);

/*
 * Destroys a VarlinkService, close all its connections and free all its
 * ressources.
 *
 * Returns NULL
 */
VarlinkService *varlink_service_free(VarlinkService *service);

/*
 * varlink_service_free() to be used with the cleanup attribute.
 */
void varlink_service_freep(VarlinkService **servicep);

/*
 * Add an interface to the service and register callbacks for its
 * methods.
 *
 * Callbacks have to be given as three arguments each: the method name
 * (without the interface prefix), a VarlinkMethodCallback, and a
 * userdata.
 */
__attribute__((sentinel))
long varlink_service_add_interface(VarlinkService *service,
                                   const char *interface_description,
                                   ...);

/*
 * Get the file descriptor to integrate with poll() into a mainloop; it becomes
 * readable whenever there is a connection which gets ready to receive or send
 * data.
 *
 * Returns the file descriptor or a negative VARLINK_ERROR.
 */
int varlink_service_get_fd(VarlinkService *service);

/*
 * Create a listen file descriptor for a varlink address and return it.
 * If the address is for a UNIX domain socket in the file system, it's
 * path will be returned in pathp and should be unlinked after closing
 * the socket.
 */
int varlink_listen(const char *address, char **pathp);

/*
 * Process pending events; It needs to be called whenever the file descriptor
 * becomes readable. Messages are sent and received, method calls are dispatched
 * according to their registered callbacks.
 *
 * Returns 0 or a negative VARLINK_ERROR.
 */
long varlink_service_process_events(VarlinkService *service);

VarlinkCall *varlink_call_ref(VarlinkCall *call);
VarlinkCall *varlink_call_unref(VarlinkCall *call);
void varlink_call_unrefp(VarlinkCall **callp);

/*
 * Get the fully-qualified method name for the current call.
 */
const char *varlink_call_get_method(VarlinkCall *call);

/*
 * Sets a function which is called when the client closes the connection.
 *
 * Only one such function can be set.
 */
long varlink_call_set_connection_closed_callback(VarlinkCall *call,
                                                 VarlinkCallConnectionClosed callback,
                                                 void *userdata);

/*
 * Retrieve the userdata pointer set with varlink_call_set_connection_closed_callback().
 */
void *varlink_call_get_connection_userdata(VarlinkCall *call);

/*
 * Get the file descriptor of the connection of the current call.
 *
 * UNIX domain socket connections carry UID, GID, PID credentials of the peer and
 * can be retrieved with SO_PEERCRED.
 *
 * Returns the file descriptor or a negative VARLINK_ERROR.
 */
int varlink_call_get_connection_fd(VarlinkCall *call);

/*
 * Reply to a method call. After this function, the call is finished.
 */
long varlink_call_reply(VarlinkCall *call,
                        VarlinkObject *parameters,
                        uint64_t flags);
/*
 * Reply to a method call with the specified error, and optional
 * parameters describing the error. Errors and their parameters need
 * to be specified in the service defintion.
 */
long varlink_call_reply_error(VarlinkCall *call,
                              const char *error,
                              VarlinkObject *parameters);

/*
 * Reply to a method call with a org.varlink.service.InvalidParameter error.
 * The error contains the field name with the invalid parameter.
 *
 */
long varlink_call_reply_invalid_parameter(VarlinkCall *call, const char *parameter);

/*
 * Create a new client connection.
 */
long varlink_connection_new(VarlinkConnection **connectionp, const char *address);

/*
 * Close a client connection and free all its ressources.
 *
 * Returns NULL
 */
VarlinkConnection *varlink_connection_free(VarlinkConnection *connection);

/*
 * varlink_connection_free() to be used with the cleanup attribute.
 */
void varlink_connection_freep(VarlinkConnection **connectionp);

/*
 * Register function to be called when the server closes the connection.
 */
void varlink_connection_set_closed_callback(VarlinkConnection *connection,
                                            VarlinkConnectionClosedFunc callback,
                                            void *userdata);
/*
 * Retrieve the userdata pointer set with varlink_connection_set_closed_callback().
 */
void *varlink_connection_get_userdata(VarlinkConnection *connection);

/*
 * Get the file descriptor to integrate with poll() into a mainloop; it becomes
 * readable whenever there is a connection which gets ready to receive or send
 * data.
 *
 * Returns the file descriptor or a negative VARLINK_ERROR.
 */
int varlink_connection_get_fd(VarlinkConnection *connection);

/*
 * Process pending events; It needs to be called whenever the file descriptor
 * becomes readable. Messages are sent and received, method calls are dispatched
 * according to their registered callbacks.
 *
 * Returns 0 or a negative VARLINK_ERROR.
 */
long varlink_connection_process_events(VarlinkConnection *connection, uint32_t events);

uint32_t varlink_connection_get_events(VarlinkConnection *connection);

/*
 * Call the specified method with the given argument. The reply will execute
 * the given callback.
 *
 * Returns 0 or a negative VARLINK_ERROR.
 */
long varlink_connection_call(VarlinkConnection *connection,
                             const char *qualified_method,
                             VarlinkObject *parameters,
                             uint64_t flags,
                             VarlinkReplyFunc callback,
                             void *userdata);

/*
 * Closes @connection.
 */
long varlink_connection_close(VarlinkConnection *connection);

bool varlink_connection_is_closed(VarlinkConnection *connection);

#ifdef __cplusplus
}
#endif
