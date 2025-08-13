#include <nghttp2/nghttp2.h>
#include <iostream>
#include <openssl/ssl.h>

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
namespace Http2 {
	nghttp2_session *Session() {
		nghttp2_session *session = nullptr;
		return session;
	}
	nghttp2_sesison_callbacks *SetupCallback() {
		nghttp2_session_callbacks *callbacks;
		nghttp2_session_callbacks_new(&callbacks);
		nghttp2_session_callbacks_set_send_callback(callbacks, send_callback);
		nghttp2_session_callbacks_set_recv_callback(callbacks, recv_callback);
		nghttp2_session_callbacks_set_on_header_callback(callbacks, on_header_callback);
		nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, on_data_chunk_recv_callback);
		return callbacks;
	}
}
