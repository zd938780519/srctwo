/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From ppb_websocket.idl modified Thu May 31 15:47:38 2012. */

#ifndef PPAPI_C_PPB_WEBSOCKET_H_
#define PPAPI_C_PPB_WEBSOCKET_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_var.h"

#define PPB_WEBSOCKET_INTERFACE_1_0 "PPB_WebSocket;1.0"
#define PPB_WEBSOCKET_INTERFACE PPB_WEBSOCKET_INTERFACE_1_0

/**
 * @file
 * This file defines the <code>PPB_WebSocket</code> interface providing
 * bi-directional, full-duplex, communications over a single TCP socket.
 */


/**
 * @addtogroup Enums
 * @{
 */
/**
 * This enumeration contains the types representing the WebSocket ready state
 * and these states are based on the JavaScript WebSocket API specification.
 * GetReadyState() returns one of these states.
 */
typedef enum {
  /**
   * Ready state is queried on an invalid resource.
   */
  PP_WEBSOCKETREADYSTATE_INVALID = -1,
  /**
   * Ready state that the connection has not yet been established.
   */
  PP_WEBSOCKETREADYSTATE_CONNECTING = 0,
  /**
   * Ready state that the WebSocket connection is established and communication
   * is possible.
   */
  PP_WEBSOCKETREADYSTATE_OPEN = 1,
  /**
   * Ready state that the connection is going through the closing handshake.
   */
  PP_WEBSOCKETREADYSTATE_CLOSING = 2,
  /**
   * Ready state that the connection has been closed or could not be opened.
   */
  PP_WEBSOCKETREADYSTATE_CLOSED = 3
} PP_WebSocketReadyState;
PP_COMPILE_ASSERT_SIZE_IN_BYTES(PP_WebSocketReadyState, 4);

/**
 * This enumeration contains status codes. These codes are used in Close() and
 * GetCloseCode(). Refer to RFC 6455, The WebSocket Protocol, for further
 * information.
 * <code>PP_WEBSOCKETSTATUSCODE_NORMAL_CLOSURE</code> and codes in the range
 * <code>PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MIN</code> to
 * <code>PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MAX</code>, and
 * <code>PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MIN</code> to
 * <code>PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MAX</code> are valid for Close().
 */
typedef enum {
  /**
   * Indicates to request closing connection without status code and reason.
   *
   * (Note that the code 1005 is forbidden to send in actual close frames by
   * the RFC. PP_WebSocket reuses this code internally and the code will never
   * appear in the actual close frames.)
   */
  PP_WEBSOCKETSTATUSCODE_NOT_SPECIFIED = 1005,
  /**
   * Status codes in the range 0-999 are not used.
   */
  /**
   * Indicates a normal closure.
   */
  PP_WEBSOCKETSTATUSCODE_NORMAL_CLOSURE = 1000,
  /**
   * Indicates that an endpoint is "going away", such as a server going down.
   */
  PP_WEBSOCKETSTATUSCODE_GOING_AWAY = 1001,
  /**
   * Indicates that an endpoint is terminating the connection due to a protocol
   * error.
   */
  PP_WEBSOCKETSTATUSCODE_PROTOCOL_ERROR = 1002,
  /**
   * Indicates that an endpoint is terminating the connection because it has
   * received a type of data it cannot accept.
   */
  PP_WEBSOCKETSTATUSCODE_UNSUPPORTED_DATA = 1003,
  /**
   * Status code 1004 is reserved.
   */
  /**
   * Pseudo code to indicate that receiving close frame doesn't contain any
   * status code.
   */
  PP_WEBSOCKETSTATUSCODE_NO_STATUS_RECEIVED = 1005,
  /**
   * Pseudo code to indicate that connection was closed abnormally, e.g.,
   * without closing handshake.
   */
  PP_WEBSOCKETSTATUSCODE_ABNORMAL_CLOSURE = 1006,
  /**
   * Indicates that an endpoint is terminating the connection because it has
   * received data within a message that was not consistent with the type of
   * the message (e.g., non-UTF-8 data within a text message).
   */
  PP_WEBSOCKETSTATUSCODE_INVALID_FRAME_PAYLOAD_DATA = 1007,
  /**
   * Indicates that an endpoint is terminating the connection because it has
   * received a message that violates its policy.
   */
  PP_WEBSOCKETSTATUSCODE_POLICY_VIOLATION = 1008,
  /**
   * Indicates that an endpoint is terminating the connection because it has
   * received a message that is too big for it to process.
   */
  PP_WEBSOCKETSTATUSCODE_MESSAGE_TOO_BIG = 1009,
  /**
   * Indicates that an endpoint (client) is terminating the connection because
   * it has expected the server to negotiate one or more extension, but the
   * server didn't return them in the response message of the WebSocket
   * handshake.
   */
  PP_WEBSOCKETSTATUSCODE_MANDATORY_EXTENSION = 1010,
  /**
   * Indicates that a server is terminating the connection because it
   * encountered an unexpected condition.
   */
  PP_WEBSOCKETSTATUSCODE_INTERNAL_SERVER_ERROR = 1011,
  /**
   * Status codes in the range 1012-1014 are reserved.
   */
  /**
   * Pseudo code to indicate that the connection was closed due to a failure to
   * perform a TLS handshake.
   */
  PP_WEBSOCKETSTATUSCODE_TLS_HANDSHAKE = 1015,
  /**
   * Status codes in the range 1016-2999 are reserved.
   */
  /**
   * Status codes in the range 3000-3999 are reserved for use by libraries,
   * frameworks, and applications. These codes are registered directly with
   * IANA.
   */
  PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MIN = 3000,
  PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MAX = 3999,
  /**
   * Status codes in the range 4000-4999 are reserved for private use.
   * Application can use these codes for application specific purposes freely.
   */
  PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MIN = 4000,
  PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MAX = 4999
} PP_WebSocketCloseCode;
PP_COMPILE_ASSERT_SIZE_IN_BYTES(PP_WebSocketCloseCode, 4);
/**
 * @}
 */

/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * The <code>PPB_WebSocket</code> interface provides bi-directional,
 * full-duplex, communications over a single TCP socket.
 */
struct PPB_WebSocket_1_0 {
  /**
   * Create() creates a WebSocket instance.
   *
   * @param[in] instance A <code>PP_Instance</code> identifying the instance
   * with the WebSocket.
   *
   * @return A <code>PP_Resource</code> corresponding to a WebSocket if
   * successful.
   */
  PP_Resource (*Create)(PP_Instance instance);
  /**
   * IsWebSocket() determines if the provided <code>resource</code> is a
   * WebSocket instance.
   *
   * @param[in] resource A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns <code>PP_TRUE</code> if <code>resource</code> is a
   * <code>PPB_WebSocket</code>, <code>PP_FALSE</code> if the
   * <code>resource</code> is invalid or some type other than
   * <code>PPB_WebSocket</code>.
   */
  PP_Bool (*IsWebSocket)(PP_Resource resource);
  /**
   * Connect() connects to the specified WebSocket server. You can call this
   * function once for a <code>web_socket</code>.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @param[in] url A <code>PP_Var</code> representing a WebSocket server URL.
   * The <code>PP_VarType</code> must be <code>PP_VARTYPE_STRING</code>.
   *
   * @param[in] protocols A pointer to an array of <code>PP_Var</code>
   * specifying sub-protocols. Each <code>PP_Var</code> represents one
   * sub-protocol and its <code>PP_VarType</code> must be
   * <code>PP_VARTYPE_STRING</code>. This argument can be null only if
   * <code>protocol_count</code> is 0.
   *
   * @param[in] protocol_count The number of sub-protocols in
   * <code>protocols</code>.
   *
   * @param[in] callback A <code>PP_CompletionCallback</code> called
   * when a connection is established or an error occurs in establishing
   * connection.
   *
   * @return An int32_t containing an error code from <code>pp_errors.h</code>.
   * Returns <code>PP_ERROR_BADARGUMENT</code> if the specified
   * <code>url</code>, or <code>protocols</code> contain an invalid string as
   * defined in the WebSocket API specification.
   * <code>PP_ERROR_BADARGUMENT</code> corresponds to a SyntaxError in the
   * WebSocket API specification.
   * Returns <code>PP_ERROR_NOACCESS</code> if the protocol specified in the
   * <code>url</code> is not a secure protocol, but the origin of the caller
   * has a secure scheme. Also returns <code>PP_ERROR_NOACCESS</code> if the
   * port specified in the <code>url</code> is a port that the user agent
   * is configured to block access to because it is a well-known port like
   * SMTP. <code>PP_ERROR_NOACCESS</code> corresponds to a SecurityError of the
   * specification.
   * Returns <code>PP_ERROR_INPROGRESS</code> if this is not the first call to
   * Connect().
   */
  int32_t (*Connect)(PP_Resource web_socket,
                     struct PP_Var url,
                     const struct PP_Var protocols[],
                     uint32_t protocol_count,
                     struct PP_CompletionCallback callback);
  /**
   * Close() closes the specified WebSocket connection by specifying
   * <code>code</code> and <code>reason</code>.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @param[in] code The WebSocket close code. This is ignored if it is
   * <code>PP_WEBSOCKETSTATUSCODE_NOT_SPECIFIED</code>.
   * <code>PP_WEBSOCKETSTATUSCODE_NORMAL_CLOSURE</code> must be used for the
   * usual case. To indicate some specific error cases, codes in the range
   * <code>PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MIN</code> to
   * <code>PP_WEBSOCKETSTATUSCODE_USER_REGISTERED_MAX</code>, and in the range
   * <code>PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MIN</code> to
   * <code>PP_WEBSOCKETSTATUSCODE_USER_PRIVATE_MAX</code> are available.
   *
   * @param[in] reason A <code>PP_Var</code> representing the WebSocket
   * close reason. This is ignored if it is <code>PP_VARTYPE_UNDEFINED</code>.
   * Otherwise, its <code>PP_VarType</code> must be
   * <code>PP_VARTYPE_STRING</code>.
   *
   * @param[in] callback A <code>PP_CompletionCallback</code> called
   * when the connection is closed or an error occurs in closing the
   * connection.
   *
   * @return An int32_t containing an error code from <code>pp_errors.h</code>.
   * Returns <code>PP_ERROR_BADARGUMENT</code> if <code>reason</code> contains
   * an invalid character as a UTF-8 string, or is longer than 123 bytes.
   * <code>PP_ERROR_BADARGUMENT</code> corresponds to a JavaScript SyntaxError
   * in the WebSocket API specification.
   * Returns <code>PP_ERROR_NOACCESS</code> if the code is not an integer
   * equal to 1000 or in the range 3000 to 4999. <code>PP_ERROR_NOACCESS</code>
   * corresponds to an InvalidAccessError in the WebSocket API specification.
   * Returns <code>PP_ERROR_INPROGRESS</code> if a previous call to Close() is
   * not finished.
   */
  int32_t (*Close)(PP_Resource web_socket,
                   uint16_t code,
                   struct PP_Var reason,
                   struct PP_CompletionCallback callback);
  /**
   * ReceiveMessage() receives a message from the WebSocket server.
   * This interface only returns a single message. That is, this interface must
   * be called at least N times to receive N messages, no matter the size of
   * each message.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @param[out] message The received message is copied to provided
   * <code>message</code>. The <code>message</code> must remain valid until
   * ReceiveMessage() completes. Its received <code>PP_VarType</code> will be
   * <code>PP_VARTYPE_STRING</code> or <code>PP_VARTYPE_ARRAY_BUFFER</code>.
   *
   * @param[in] callback A <code>PP_CompletionCallback</code> called
   * when ReceiveMessage() completes. This callback is ignored if
   * ReceiveMessage() completes synchronously and returns <code>PP_OK</code>.
   *
   * @return An int32_t containing an error code from <code>pp_errors.h</code>.
   * If an error is detected or connection is closed, ReceiveMessage() returns
   * <code>PP_ERROR_FAILED</code> after all buffered messages are received.
   * Until buffered message become empty, ReceiveMessage() continues to return
   * <code>PP_OK</code> as if connection is still established without errors.
   */
  int32_t (*ReceiveMessage)(PP_Resource web_socket,
                            struct PP_Var* message,
                            struct PP_CompletionCallback callback);
  /**
   * SendMessage() sends a message to the WebSocket server.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @param[in] message A message to send. The message is copied to an internal
   * buffer, so the caller can free <code>message</code> safely after returning
   * from the function. Its sent <code>PP_VarType</code> must be
   * <code>PP_VARTYPE_STRING</code> or <code>PP_VARTYPE_ARRAY_BUFFER</code>.
   *
   * @return An int32_t containing an error code from <code>pp_errors.h</code>.
   * Returns <code>PP_ERROR_FAILED</code> if the ReadyState is
   * <code>PP_WEBSOCKETREADYSTATE_CONNECTING</code>.
   * <code>PP_ERROR_FAILED</code> corresponds to a JavaScript
   * InvalidStateError in the WebSocket API specification.
   * Returns <code>PP_ERROR_BADARGUMENT</code> if the provided
   * <code>message</code> contains an invalid character as a UTF-8 string.
   * <code>PP_ERROR_BADARGUMENT</code> corresponds to a JavaScript
   * SyntaxError in the WebSocket API specification.
   * Otherwise, returns <code>PP_OK</code>, which doesn't necessarily mean
   * that the server received the message.
   */
  int32_t (*SendMessage)(PP_Resource web_socket, struct PP_Var message);
  /**
   * GetBufferedAmount() returns the number of bytes of text and binary
   * messages that have been queued for the WebSocket connection to send, but
   * have not been transmitted to the network yet.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns the number of bytes.
   */
  uint64_t (*GetBufferedAmount)(PP_Resource web_socket);
  /**
   * GetCloseCode() returns the connection close code for the WebSocket
   * connection.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns 0 if called before the close code is set.
   */
  uint16_t (*GetCloseCode)(PP_Resource web_socket);
  /**
   * GetCloseReason() returns the connection close reason for the WebSocket
   * connection.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns a <code>PP_VARTYPE_STRING</code> var. If called before the
   * close reason is set, the return value contains an empty string. Returns a
   * <code>PP_VARTYPE_UNDEFINED</code> if called on an invalid resource.
   */
  struct PP_Var (*GetCloseReason)(PP_Resource web_socket);
  /**
   * GetCloseWasClean() returns if the connection was closed cleanly for the
   * specified WebSocket connection.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns <code>PP_FALSE</code> if called before the connection is
   * closed, called on an invalid resource, or closed for abnormal reasons.
   * Otherwise, returns <code>PP_TRUE</code> if the connection was closed
   * cleanly.
   */
  PP_Bool (*GetCloseWasClean)(PP_Resource web_socket);
  /**
   * GetExtensions() returns the extensions selected by the server for the
   * specified WebSocket connection.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns a <code>PP_VARTYPE_STRING</code> var. If called before the
   * connection is established, the var's data is an empty string. Returns a
   * <code>PP_VARTYPE_UNDEFINED</code> if called on an invalid resource.
   */
  struct PP_Var (*GetExtensions)(PP_Resource web_socket);
  /**
   * GetProtocol() returns the sub-protocol chosen by the server for the
   * specified WebSocket connection.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns a <code>PP_VARTYPE_STRING</code> var. If called before the
   * connection is established, the var contains the empty string. Returns a
   * <code>PP_VARTYPE_UNDEFINED</code> if called on an invalid resource.
   */
  struct PP_Var (*GetProtocol)(PP_Resource web_socket);
  /**
   * GetReadyState() returns the ready state of the specified WebSocket
   * connection.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns <code>PP_WEBSOCKETREADYSTATE_INVALID</code> if called
   * before Connect() is called, or if this function is called on an
   * invalid resource.
   */
  PP_WebSocketReadyState (*GetReadyState)(PP_Resource web_socket);
  /**
   * GetURL() returns the URL associated with specified WebSocket connection.
   *
   * @param[in] web_socket A <code>PP_Resource</code> corresponding to a
   * WebSocket.
   *
   * @return Returns a <code>PP_VARTYPE_STRING</code> var. If called before the
   * connection is established, the var contains the empty string. Returns a
   * <code>PP_VARTYPE_UNDEFINED</code> if this function is called on an
   * invalid resource.
   */
  struct PP_Var (*GetURL)(PP_Resource web_socket);
};

typedef struct PPB_WebSocket_1_0 PPB_WebSocket;
/**
 * @}
 */

#endif  /* PPAPI_C_PPB_WEBSOCKET_H_ */

