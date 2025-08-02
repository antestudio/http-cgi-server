#pragma once
#ifndef _CGIHANDLER_HPP_
#define _CGIHANDLER_HPP_

#include <net/http/response.hpp>
#include <net/socket.hpp>
#include <net/http/request.hpp>

HttpResponse handle_cgi_request(const HttpRequest&, const Socket&);

#endif//_CGIHANDLER_HPP_