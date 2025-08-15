#include <http2/http2.h>

int main() {
	TLS::initialize_normal("TLSV1_3"); // Default is TLSV1.3
	TLS::initialize_normal("TLSV1_2");
	// TLSV1.1 not supported
}
