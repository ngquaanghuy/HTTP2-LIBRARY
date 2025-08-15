g++ session.cpp -o session $HOME/HTTP2-LIBRARY/src/* -I $HOME/HTTP2-LIBRARY/ -lssl -lnghttp2 -lcrypto

g++ callbacks.cpp -o callbacks $HOME/HTTP2-LIBRARY/src/* -I $HOME/HTTP2-LIBRARY/ -lssl -lnghttp2 -lcrypto 
