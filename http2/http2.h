#pragma once

#include <vector>
#include <string>
#include <nghttp2/nghttp2.h>
#include <openssl/ssl.h>

struct TlsConfig {
	std::string host;
	int port;
	std::string version = "TLSV1_3";
	int timeout_ms;
};
namespace Http2 {
	nghttp2_session *Session();
	nghttp2_session_callbacks *SetupCallback();
}

namespace Session {
	void clean_session(nghttp2_session *session);
	void clean_session(...) = delete;
	void clean_callback(nghttp2_session_callbacks *callbacks);
	void clean_callback(...) = delete;
}

namespace TLS {
	void initialize_normal(std::string version);
	SSL* initialize_config(const TlsConfig &cfg);
	
}
