#pragma once

#include <vector>
#include <string>
#include <nghttp2/nghttp2.h>
#include <openssl/ssl.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <cstddef>
#define HTTP2_FLAG_FRAME NGHTTP2_FLAG_NONE
struct TlsConfig {
	std::string host;
	int port;
	std::string version = "TLSV1_3";
	int timeout_ms;
};
namespace Http2 {
	void setFrame_easy(nghttp2_session *session);
	void setFrame_easy(...) = delete;
	void setFrame_hard(nghttp2_session *session, uint8_t flags, const nghttp2_settings_entry *iv, size_t niv);
	nghttp2_session *Session();
	nghttp2_session_callbacks *SetupCallback();
}

namespace Session {
	void clean_session(nghttp2_session *session);
	void clean_session(...) = delete;
	void clean_callback(nghttp2_session_callbacks *callbacks);
	void clean_callback(...) = delete;
}
namespace Callbacks {
	nghttp2_session_callbacks *init_callbacks();
	nghttp2_session_callbacks *init_cbs_auto();
	int init_cbs_new(nghttp2_session_callbacks **callbacks);
	int init_send(nghttp2_session_callbacks **callbacks);
	int init_send_handmade(nghttp2_session_callbacks **callbacks, nghttp2_send_callback send_cb);
	int init_recv(nghttp2_session_callbacks **callbacks);
	int init_recv_handmade(nghttp2_session_callbacks **callbacks, nghttp2_recv_callback recv_cb);
}
namespace TLS {
	void initialize_normal(std::string version);
	SSL* initialize_config(const TlsConfig &cfg);
	
}
