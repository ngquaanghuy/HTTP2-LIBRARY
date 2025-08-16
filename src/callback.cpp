#include <http2/http2.h>
#include <iostream>
#include <nghttp2/nghttp2.h>
#include <cstdint>
#include <cstddef>
#include <openssl/ssl.h>
#include <openssl/err.h>
struct Connection {
	int fd;
	SSL *ssl;
};
static ssize_t send_callback(nghttp2_session *session, const uint8_t *data, size_t length, int, void *user_data) {
	Connection *conn = (Connection*)user_data;
	int ret = SSL_write(conn->ssl, data, length);
	if (ret <= 0) {
		int err = SSL_get_error(conn->ssl, ret);
		if (err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ) {
			return NGHTTP2_ERR_WOULDBLOCK;
		}
		return NGHTTP2_ERR_CALLBACK_FAILURE;
	}
	return ret;
}
static ssize_t recv_callback(nghttp2_session *session, uint8_t *buf, size_t length, int, void *user_data) {
	Connection *conn = (Connection*)user_data;
	int ret = SSL_read(conn->ssl, buf, length);
	if (ret <= 0) {
		int err = SSL_get_error(conn->ssl, ret);
		if (err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ) {
			return NGHTTP2_ERR_WOULDBLOCK;
		}
		return NGHTTP2_ERR_CALLBACK_FAILURE;
	}
	return ret;
}
static int on_header_callback(nghttp2_session *session, const nghttp2_frame *frame, const uint8_t *name, size_t namelen, const uint8_t *value, size_t valuelen, uint8_t, void*) {
	if (frame->hd.type == NGHTTP2_HEADERS && frame->headers.cat == NGHTTP2_HCAT_RESPONSE) {
		std::cout << std::string((const char*)name, namelen) << ";" << std::string((const char*)value, valuelen) << "\n";
	}
	return 0;
}
static int on_data_chunk_recv_callback(nghttp2_session *session, uint8_t flags, int32_t stream_id, const uint8_t *data, size_t len, void*) {
	std::cout.write((const char*)data, len);
	return 0;
}
#define HTTP2_ERROR(msg) std::cerr << __FILE__ << " : " << __LINE__ << " | " << __func__ << " -> error: " << msg << "\n"
namespace Callbacks {
	nghttp2_session_callbacks *init_callbacks() {
		nghttp2_session_callbacks *callbacks;
		return callbacks;
	}
	nghttp2_session_callbacks *init_cbs_auto() {
		nghttp2_session_callbacks *callbacks;
		nghttp2_session_callbacks_new(&callbacks);
		return callbacks;
	}
	int init_cbs_new(nghttp2_session_callbacks **callbacks) {
		if (!callbacks) {
			HTTP2_ERROR("The data passed in is not callbacks.");
			return 1;
		}
		return nghttp2_session_callbacks_new(callbacks);
	}
	int init_send(nghttp2_session_callbacks **callbacks) {
		if (!callbacks) {
			HTTP2_ERROR("The data passed in is not callbacks.");
			return 1;
		}
		nghttp2_session_callbacks_set_send_callback(*callbacks, send_callback);
		return 0;
	}
	int init_send_handmade(nghttp2_session_callbacks **callbacks, nghttp2_send_callback send_cb) {
		if (!callbacks || !*callbacks) {
			HTTP2_ERROR("Inappropriate callbacks");
			return 1;
		}
		nghttp2_session_callbacks_set_send_callback(*callbacks, send_cb);
		return 0;
	}
	int init_recv(nghttp2_session_callbacks **callbacks) {
		if (!callbacks) {
			HTTP2_ERROR("The data passed in is not callbacks.");
			return 1;
		}
		nghttp2_session_callbacks_set_recv_callback(*callbacks, recv_callback);
		return 0;
	}
	int init_recv_handmade(nghttp2_session_callbacks **callbacks, nghttp2_recv_callback recv_cb) {
		if (!callbacks || !*callbacks) {
			HTTP2_ERROR("Inappropriate callbacks");
			return 1;
		}
		nghttp2_session_callbacks_set_recv_callback(*callbacks, recv_cb);
		return 0;
	}
}
