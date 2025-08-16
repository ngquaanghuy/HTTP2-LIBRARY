#include <http2/http2.h>
#include <iostream>
#include <cstdint>
#include <cstddef>
struct Connection {
		int fd;
        SSL *ssl;
};
static ssize_t send_callback(nghttp2_session *session, const uint8_t *data, size_t length, int, void *user_data) {
        Connection *conn = (Connection*)user_data;
        int ret = SSL_write(conn->ssl, data, length);
        if (ret <= 0) {
                int err = SSL_get_error(conn->ssl, ret);                   if (err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ) {
                        return NGHTTP2_ERR_WOULDBLOCK;
                }
                return NGHTTP2_ERR_CALLBACK_FAILURE;
        }                                                          return ret;
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
int main() {
	auto session = Http2::Session();
	auto callback = Http2::SetupCallback();
	auto callback_new = Callbacks::init_callbacks();
	auto callbacks_auto = Callbacks::init_cbs_auto();
	Callbacks::init_cbs_new(&callback_new);
	Callbacks::init_send(&callback_new);

	// Handmade
	auto call = Callbacks::init_callbacks();
	Callbacks::init_cbs_new(&call);
	Callbacks::init_send_handmade(&call, send_callback);
	std::cout << session << "\n";
	//Session::clean_session(session);
	Session::clean_callback(callback);
	TLS::initialize_normal("TLSV1_1");
	if (session == nullptr) {
		std::cout << "Remove session sucessfully\n";
	} else {
		std::cout << "Faild to remove\n";
	}
	//Http2::setFrame_easy(session);
	Http2::setFrame_hard(session, HTTP2_FLAG_FRAME, nullptr, 0);
	return 0;
}
