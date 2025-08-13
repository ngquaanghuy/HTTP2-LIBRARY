#pragma once

#include <vector>
#include <string>
#include <nghttp2/nghttp2.h>
namespace Http2 {
	nghttp2_session *Session();
	nghttp2_session_callbacks *SetupCallback();
}
