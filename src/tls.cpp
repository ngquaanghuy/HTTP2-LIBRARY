#include <iostream>
#include <openssl/ssl.h>
#include <nghttp2/nghttp2.h>
#include <openssl/err.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <http2/http2.h>
#include <stdexcept>
struct TlsConfig {
	std::string ip;
	int port;
	std::string version;
};
namespace TLS {
	void initialize_config (const TlsConfig &cfg) {
		SSL_load_error_strings();
		OpenSSl_add_ssl_algorithms();
		const SSL_METHOD *method = TLS_client_method();
		SSL_CTX *ctx = SSL_CTX_new(method);
		if (!ctx) {
			throw std::runtime_error("Unable to create SSL context");
		}
		// set config version
		if (cfg.version == "TLSV1_2") {
			SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
			SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION);	
		}
	}
	void initialize_normal(std::string version) {
		//if (!version) {
		//	HTTP2_ERROR("Tls handshake setup failed");
		//	return;
		//}
		addrinfo hints{}, *res;
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		connect(fd, res->ai_addr, res->ai_addrlen);
		int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (version == "TLSV1_2") {
			SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
			SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
			SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION);
			const unsigned char alpn_protos[] = {2, 'h', '2'};
			SSL_CTX_set_alpn_protos(ctx, alpn_protos, sizeof(alpn_protos)-1);
			SSL *ssl = SSL_new(ctx);
			SSL_set_fd(ssl, sock_fd);
			if (SSL_connect(ssl) <= 0) {
				ERR_print_errors_fp(stderr);
			}
		}
		if (version == "TLSV1_3") {
			SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
			SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
			SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

			const unsigned char alpn_protos[] = {2, 'h', '2'};
			SSL_CTX_set_alpn_protos(ctx, alpn_protos, sizeof(alpn_protos)-1);
			SSL *ssl = SSL_new(ctx);
			SSL_set_fd(ssl, sock_fd);
			if (SSL_connect(ssl) <= 0) {
				ERR_print_errors_fp(stderr);
			}
		}
	}
}
