#include <http2/http2.h>
#include <iostream>
int main() {
	auto session = Http2::Session();
	auto callback = Http2::SetupCallback();
	std::cout << session << "\n";
	Session::clean_session(session);
	Session::clean_callback(callback);
	TLS::initialize("TLSV1_2");
	if (session == nullptr) {
		std::cout << "Remove session sucessfully\n";
	} else {
		std::cout << "Faild to remove\n";
	}
	return 0;
}
