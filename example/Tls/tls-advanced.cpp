#include <http2/http2.h>
TlsConfig cfg = {
	.host = "example.com",
	.port = 443,
	.timeout_ms = 3000
};

int main() {
	SSL *ssl = TLS::initialize_config(cfg);
	return 0;
}
