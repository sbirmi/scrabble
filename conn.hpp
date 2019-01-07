#ifndef __CONN_HPP
#define __CONN_HPP

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

typedef websocketpp::connection_hdl Handle;
typedef std::pair<const Handle&, std::string> HandleResponse;
typedef std::vector<HandleResponse> HandleResponseList;

#endif
