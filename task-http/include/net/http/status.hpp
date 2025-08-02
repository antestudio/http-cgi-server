#pragma once
#ifndef _NET_HTTP_STATUS_HPP_
#define _NET_HTTP_STATUS_HPP_

enum Status {
  /* 2xx status codes */
  OK                  = 200,
  /* 4xx status codes */
  BAD_REQUEST         = 400,
  FORBIDDEN           = 403,
  NOT_FOUND           = 404,
  /* 5xx status codes */
  INTERNAL_ERROR      = 500,
  NOT_IMPLEMENTED     = 501,
  SERVICE_UNAVAILABLE = 503,
};

#endif//_NET_HTTP_STATUS_HPP_