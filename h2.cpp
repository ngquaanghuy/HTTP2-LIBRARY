#include <http2/http2.h>
#include <iostream>
int main() {
	auto session = Http2::Session();
	auto callback = Http2::SetupCallback();
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
