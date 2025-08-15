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
#include <fcntl.h>
#include <chrono>
#include <poll.h>
#include <http2/http2.h>
#define HTTP2_ERROR(msg) std::cerr << __FILE__ << " : " << __LINE__ << " | " << __func__ << " -> error: " << msg << "\n";
namespace TLS {
	SSL* initialize_config (const TlsConfig &cfg) {
		SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();
		SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
		if (!ctx) {
			throw std::runtime_error("Unable to create SSL context");
		}
		// set config version
		if (cfg.version == "TLSV1_2") {
			SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
			SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION);	
		} else if (cfg.version == "TLSV1_3") {
			SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
			SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);
		} else {
			throw  std::invalid_argument("TLS version " + cfg.version + " is not currently supported");
		}

		// Resolve hostname
		struct addrinfo hints{}, *res;
//		hints.ai_family = AF_UNSPEC;
//		hints.ai_socktype = SOCK_STREAM;
		std::string port_str = std::to_string(cfg.port);
		if (getaddrinfo(cfg.host.c_str(), port_str.c_str(), &hints, &res) != 0) {
			throw std::runtime_error("GetAddrInfo failed");
		}

		// Socket

		int sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sock_fd <= 0) {
			freeaddrinfo(res);
			throw std::runtime_error("Socket initialization failed!");
		}

		if (connect(sock_fd, res->ai_addr, res->ai_addrlen) != 0) {
			close(sock_fd);
			freeaddrinfo(res);
			throw std::runtime_error("Connection failed!");
		}
		freeaddrinfo(res);

		//SSL 
		SSL *ssl = SSL_new(ctx);
		SSL_set_fd(ssl, sock_fd);

		// SNI
		if (!SSL_set_tlsext_host_name(ssl, cfg.host.c_str())) {
			close(sock_fd);
			SSL_free(ssl);
			SSL_CTX_free(ctx);
			throw std::runtime_error("Failed to set SNI!");
		}
		// timeout
		fcntl(sock_fd, F_SETFL, O_NONBLOCK);
		auto start = std::chrono::steady_clock::now();
		connect(sock_fd, res->ai_addr, res->ai_addrlen);
		struct pollfd pfd{sock_fd, 0, 0};
		while (true) {
			int ret = SSL_connect(ssl);
			if (ret == 1) break;
			int err = SSL_get_error(ssl, ret);
			if (err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ) {
				pfd.events = (err == SSL_ERROR_WANT_READ) ? POLLIN : POLLOUT;
				int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
				if (elapsed >= cfg.timeout_ms) {
					close(sock_fd);
					SSL_free(ssl);
					SSL_CTX_free(ctx);
					throw std::runtime_error("TLS Handshake timeout");
				}
				poll(&pfd, 1, cfg.timeout_ms - elapsed);
			} else {
				ERR_print_errors_fp(stderr);
				close(sock_fd);
				SSL_free(ssl);
				SSL_CTX_free(ctx);
				throw std::runtime_error("TLS HANDSHAKE Failure");
			}
		}
		fcntl(sock_fd, F_SETFL, fcntl(sock_fd, F_GETFL)& ~O_NONBLOCK);
		return ssl;
		// TLS HANDSHAKE
		/*
		if (SSL_connect(ssl) <= 0) {
			ERR_print_errors_fp(stderr);
			close(sock_fd);
			SSL_free(ssl);
			SSL_CTX_free(ctx);
			throw std::runtime_error("TLS Handshake Failure");
		}
		*/
	}
	void initialize_normal(std::string version = "abcd") {
		if (version == "abcd") {
			SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
			SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
			SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);
		}
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
