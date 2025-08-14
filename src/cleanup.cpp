#include <iostream>
#include <nghttp2/nghttp2.h>
#include <http2/http2.h>
#include <string>
#include <type_traits>

#define HTTP2_ERROR(msg) std::cerr << __FILE__ << " : " << __LINE__ << " | " << __func__ << " -> error: " << msg << "\n";
namespace Session {
	void clean_session(nghttp2_session *session) {
		if (!session) {
			HTTP2_ERROR("Force session variable data to be passed to Session::clean_session() function");
			return;
		}		
		nghttp2_session_del(session);
	}
	void clean_callback(nghttp2_session_callbacks *callbacks) {
		if (!callbacks) {
			HTTP2_ERROR("It is mandatory to pass a callbacks variable to this function");
			return;
		}
		nghttp2_session_callbacks_del(callbacks);
	}
}
