#include <nghttp2/nghttp2.h>
#include <iostream>
#include <openssl/ssl.h>
#include <cstdint>
#include <cstddef>
#define HTTP2_FLAG_FRAME NGHTTP2_FLAG_NONE
struct Connection {
	int fd;
	SSL *ssl;
};
#define HTTP2_ERROR(msg) std::cerr << __FILE__ << " : " << __LINE__ << " | " << __func__ << " -> error: " << msg << "\n";
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
		nghttp2_session_callbacks *callbacks;
		nghttp2_session_callbacks_new(&callbacks);
		int ret = nghttp2_session_client_new(&session, callbacks, nullptr);
		nghttp2_session_callbacks_del(callbacks);
		if (ret != 0) {
			HTTP2_ERROR("Session initialization failed!");
			return nullptr;
		}
		return session;
	}
	void setFrame_easy(nghttp2_session *session) {
		if (!session) {
			HTTP2_ERROR("Session does not exist");
			return;
		}
		nghttp2_submit_settings(session, NGHTTP2_FLAG_NONE, nullptr, 0);
	}
	void setFrame_hard(nghttp2_session *session, uint8_t flags, const nghttp2_settings_entry *iv, size_t niv) {
		if (!session) {
			HTTP2_ERROR("Session not passed in");
			return;
		}
		flags = HTTP2_FLAG_FRAME;
//		if (!flags) {
//			HTTP2_ERROR("Flag of the current version does not exist");
//			return;
//		}
		if ((flags != NGHTTP2_FLAG_NONE) && (flags != HTTP2_FLAG_FRAME)) {
			HTTP2_ERROR("Only HTTP2_FLAG_FRAME - NGHTTP2_FLAG_NONE should be applied");
			return;
		}
		flags = HTTP2_FLAG_FRAME;
		nghttp2_submit_settings(session, flags, iv, niv);
	}
	nghttp2_session_callbacks *SetupCallback() {
		nghttp2_session_callbacks *callbacks;
		nghttp2_session_callbacks_new(&callbacks);
		nghttp2_session_callbacks_set_send_callback(callbacks, send_callback);
		nghttp2_session_callbacks_set_recv_callback(callbacks, recv_callback);
		nghttp2_session_callbacks_set_on_header_callback(callbacks, on_header_callback);
		nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, on_data_chunk_recv_callback);
		return callbacks;
	}
}
