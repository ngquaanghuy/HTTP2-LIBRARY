#include <http2/http2.h>
#include <iostream>
int main() {
	auto session = Http2::Session();
	auto callback = Http2::SetupCallback();
	std::cout << session << "\n";
	Session::clean_session(session);
	Session::clean_callback(callback);
	std::cout << TLS::initialize("TLSV1_1") << "\n";
	if (session == nullptr) {
		std::cout << "Remove session sucessfully\n";
	} else {
		std::cout << "Faild to remove\n";
	}
	return 0;
}
