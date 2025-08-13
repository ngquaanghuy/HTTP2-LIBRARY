#pragma once

#include <vector>
#include <string>

namespace Http2 {
	nghttp2_session *Session();
	nghttp2_session_callbacks *SetupCallback();
}
