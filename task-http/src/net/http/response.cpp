#include "net/http/response.hpp"
#include "net/http/message.hpp"
#include "net/http/status.hpp"
#include <sstream>


HttpResponse::HttpResponse(Status aStatus, std::string aComment, std::string aVersion):
  version(aVersion), status(aStatus), comment(aComment)
{
  updateTitle();
}


HttpResponse::HttpResponse(std::string_view raw) {
  is_raw = true;
  setBody(raw);
}


void HttpResponse::updateTitle(void) {
  std::stringstream builder;
  builder << getVersion() << ' ' << getStatus() << ' ' << getComment();
  HttpMessage::setTitle(builder.str());
}


std::string HttpResponse::getVersion(void) const {
  return version;
}


void HttpResponse::setVersion(std::string aVersion) {
  version = aVersion;
  updateTitle();
}


std::string HttpResponse::getComment(void) const {
  return comment;
}


void HttpResponse::setComment(std::string aComment) {
  comment = aComment;
  updateTitle();
}


Status HttpResponse::getStatus(void) const {
  return status;
}


void HttpResponse::setStatus(Status aStatus) {
  status = aStatus;
  updateTitle();
}


std::string HttpResponse::toString(void) const {
  if (is_raw) {
    return getBody().data();
  } else {
    return HttpMessage::toString();
  }
}