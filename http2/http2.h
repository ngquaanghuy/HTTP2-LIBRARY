#pragma once

#include <vector>
#include <string>
#include <nghttp2/nghttp2.h>
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
	void initialize(std::string version);
}
