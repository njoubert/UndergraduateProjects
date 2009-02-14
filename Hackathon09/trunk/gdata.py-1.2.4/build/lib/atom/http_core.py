#!/usr/bin/env python
#
#    Copyright (C) 2009 Google Inc.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.


# This module is used for version 2 of the Google Data APIs.
# TODO: add proxy handling.


__author__ = 'j.s@google.com (Jeff Scudder)'


import StringIO
import urlparse
import urllib
import httplib


class Error(Exception):
  pass


class UnknownSize(Error):
  pass


MIME_BOUNDARY = 'END_OF_PART'


class HttpRequest(object):
  """Contains all of the parameters for an HTTP 1.1 request.
 
  The HTTP headers are represented by a dictionary, and it is the
  responsibility of the user to ensure that duplicate field names are combined
  into one header value according to the rules in section 4.2 of RFC 2616.
  """
  scheme = None
  host = None
  port = None
  method = None
  uri = None
 
  def __init__(self, scheme=None, host=None, port=None, method=None, uri=None,
      headers=None):
    """Construct an HTTP request.

    Args:
      scheme: str The protocol to be used, usually this is 'http' or 'https'
      host: str The name or IP address string of the server.
      port: int The port number to connect to on the server.
      method: The HTTP method for the request, examples include 'GET', 'POST',
              etc.
      uri: str The relative path inclusing escaped query parameters.
      headers: dict of strings The HTTP headers to include in the request.
    """
    self.headers = headers or {}
    self._body_parts = []
    if scheme is not None:
      self.scheme = scheme
    if host is not None:
      self.host = host
    if port is not None:
      self.port = port
    if method is not None:
      self.method = method
    if uri is not None:
      self.uri = uri

  def add_body_part(self, data, mime_type, size=None):
    """Adds data to the HTTP request body.
   
    If more than one part is added, this is assumed to be a mime-multipart
    request. This method is designed to create MIME 1.0 requests as specified
    in RFC 1341.

    Args:
      data: str or a file-like object containing a part of the request body.
      mime_type: str The MIME type describing the data
      size: int Required if the data is a file like object. If the data is a
            string, the size is calculated so this parameter is ignored.
    """
    if isinstance(data, str):
      size = len(data)
    if size is None:
      # TODO: support chunked transfer if some of the body is of unknown size.
      raise UnknownSize('Each part of the body must have a known size.')
    if 'Content-Length' in self.headers:
      content_length = int(self.headers['Content-Length'])
    else:
      content_length = 0
    # If this is the first part added to the body, then this is not a multipart
    # request.
    if len(self._body_parts) == 0:
      self.headers['Content-Type'] = mime_type
      content_length = size
      self._body_parts.append(data)
    elif len(self._body_parts) == 1:
      # This is the first member in a mime-multipart request, so change the
      # _body_parts list to indicate a multipart payload.
      self._body_parts.insert(0, 'Media multipart posting')
      boundary_string = '\r\n--%s\r\n' % (MIME_BOUNDARY,)
      content_length += len(boundary_string) + size
      self._body_parts.insert(1, boundary_string)
      content_length += len('Media multipart posting')
      # Put the content type of the first part of the body into the multipart
      # payload.
      original_type_string = 'Content-Type: %s\r\n\r\n' % (
          self.headers['Content-Type'],)
      self._body_parts.insert(2, original_type_string)
      content_length += len(original_type_string)
      boundary_string = '\r\n--%s\r\n' % (MIME_BOUNDARY,)
      self._body_parts.append(boundary_string)
      content_length += len(boundary_string)
      # Change the headers to indicate this is now a mime multipart request.
      self.headers['Content-Type'] = 'multipart/related; boundary="%s"' % (
          MIME_BOUNDARY,)
      self.headers['MIME-version'] = '1.0'
      # Include the mime type of this part.
      type_string = 'Content-Type: %s\r\n\r\n' % (mime_type)
      self._body_parts.append(type_string)
      content_length += len(type_string)
      self._body_parts.append(data)
      ending_boundary_string = '\r\n--%s--' % (MIME_BOUNDARY,)
      self._body_parts.append(ending_boundary_string)
      content_length += len(ending_boundary_string)
    else:
      # This is a mime multipart request.
      boundary_string = '\r\n--%s\r\n' % (MIME_BOUNDARY,)
      self._body_parts.insert(-1, boundary_string)
      content_length += len(boundary_string) + size
      # Include the mime type of this part.
      type_string = 'Content-Type: %s\r\n\r\n' % (mime_type)
      self._body_parts.insert(-1, type_string)
      content_length += len(type_string)
      self._body_parts.insert(-1, data)
    self.headers['Content-Length'] = str(content_length)
  # I could add an "append_to_body_part" method as well.

  AddBodyPart = add_body_part

  def add_form_inputs(self, form_data,
                      mime_type='application/x-www-form-urlencoded'):
    body = urllib.urlencode(form_data)
    self.add_body_part(body, mime_type)

  AddFormInputs = add_form_inputs

  def _copy(self):
    new_request = HttpRequest(scheme=self.scheme, host=self.host,
        port=self.port, method=self.method, uri=self.uri,
        headers=self.headers.copy())
    new_request._body_parts = self._body_parts[:]
    return new_request


def _apply_defaults(http_request):
  if http_request.scheme is None:
    if http_request.port == 443:
      http_request.scheme = 'https'
    else:
      http_request.scheme = 'http'


class Uri(object):
  """A URI as used in HTTP 1.1"""
  scheme = None
  host = None
  port = None
  path = None
 
  def __init__(self, scheme=None, host=None, port=None, path=None, query=None):
    """Constructor for a URI.

    Args:
      scheme: str This is usually 'http' or 'https'.
      ... TODO
      query: dict of strings The URL query parameters. The keys and values are
             both escaped so this dict should contain the unescaped values.

    """
    self.query = query or {}
    if scheme is not None:
      self.scheme = scheme
    if host is not None:
      self.host = host
    if port is not None:
      self.port = port
    if path:
      self.path = path
     
  def _get_query_string(self):
    param_pairs = []
    for key, value in self.query.iteritems():
      param_pairs.append('='.join((urllib.quote_plus(key),
          urllib.quote_plus(str(value)))))
    return '&'.join(param_pairs)

  def _get_relative_path(self):
    """Returns the path with the query parameters escaped and appended."""
    param_string = self._get_query_string()
    if self.path is None:
      path = '/'
    else:
      path = self.path
    if param_string:
      return '?'.join([path, param_string])
    else:
      return path
     
  def _to_string(self):
    if self.scheme is None and self.port == 443:
      scheme = 'https'
    elif self.scheme is None:
      scheme = 'http'
    else:
      scheme = self.scheme
    if self.path is None:
      path = '/'
    else:
      path = self.path
    if self.port is None:
      return '%s://%s%s' % (scheme, self.host, self._get_relative_path())
    else:
      return '%s://%s:%s%s' % (scheme, self.host, str(self.port),
                               self._get_relative_path())
     
  def modify_request(self, http_request=None):
    """Sets HTTP request components based on the URI."""
    if http_request is None:
      http_request = HttpRequest()
    # Determine the correct scheme.
    if self.scheme:
      http_request.scheme = self.scheme
    if self.port:
      http_request.port = self.port
    if self.host:
      http_request.host = self.host
    # Set the relative uri path
    if self.path:
      http_request.uri = self._get_relative_path()
    elif not self.path and self.query:
      http_request.uri = '/%s' % self._get_relative_path()
    elif not self.path and not self.query and not http_request.uri:
      http_request.uri = '/'
    return http_request

  ModifyRequest = modify_request

 
def parse_uri(uri_string):
  """Creates a Uri object which corresponds to the URI string.
 
  This method can accept partial URIs, but it will leave missing
  members of the Uri unset.
  """
  parts = urlparse.urlparse(uri_string)
  uri = Uri()
  if parts[0]:
    uri.scheme = parts[0]
  if parts[1]:
    host_parts = parts[1].split(':')
    if host_parts[0]:
      uri.host = host_parts[0]
    if len(host_parts) > 1:
      uri.port = int(host_parts[1])
  if parts[2]:
    uri.path = parts[2]
  if parts[4]:
    param_pairs = parts[4].split('&')
    for pair in param_pairs:
      pair_parts = pair.split('=')
      if len(pair_parts) > 1:
        uri.query[urllib.unquote_plus(pair_parts[0])] = (
            urllib.unquote_plus(pair_parts[1]))
      elif len(pair_parts) == 1:
        uri.query[urllib.unquote_plus(pair_parts[0])] = None
  return uri


ParseUri = parse_uri


class HttpResponse(object):
  status = None
  reason = None
  _body = None
 
  def __init__(self, status=None, reason=None, headers=None, body=None):
    self._headers = headers or {}
    if status is not None:
      self.status = status
    if reason is not None:
      self.reason = reason
    if body is not None:
      if hasattr(body, 'read'):
        self._body = body
      else:
        self._body = StringIO.StringIO(body)
         
  def getheader(self, name, default=None):
    if name in self._headers:
      return self._headers[name]
    else:
      return default
   
  def read(self, amt=None):
    if not amt:
      return self._body.read()
    else:
      return self._body.read(amt)


class HttpClient(object):
  """Performs HTTP requests using httplib."""
  debug = None
 
  def request(self, http_request):
    return self._http_request(http_request.host, http_request.method,
        http_request.uri, http_request.scheme, http_request.port,
        http_request.headers, http_request._body_parts)

  Request = request
 
  def _http_request(self, host, method, uri, scheme=None,  port=None,
      headers=None, body_parts=None):
    """Makes an HTTP request using httplib.
   
    Args:
      uri: str
    """
    if scheme == 'https':
      if not port:
        connection = httplib.HTTPSConnection(host)
      else:
        connection = httplib.HTTPSConnection(host, int(port))
    else:
      if not port:
        connection = httplib.HTTPConnection(host)
      else:
        connection = httplib.HTTPConnection(host, int(port))
   
    if self.debug:
      connection.debuglevel = 1

    connection.putrequest(method, uri)

    # Overcome a bug in Python 2.4 and 2.5
    # httplib.HTTPConnection.putrequest adding
    # HTTP request header 'Host: www.google.com:443' instead of
    # 'Host: www.google.com', and thus resulting the error message
    # 'Token invalid - AuthSub token has wrong scope' in the HTTP response.
    if (scheme == 'https' and int(port or 443) == 443 and
        hasattr(connection, '_buffer') and
        isinstance(connection._buffer, list)):
      header_line = 'Host: %s:443' % host
      replacement_header_line = 'Host: %s' % host
      try:
        connection._buffer[connection._buffer.index(header_line)] = (
            replacement_header_line)
      except ValueError:  # header_line missing from connection._buffer
        pass

    # Send the HTTP headers.
    for header_name, value in headers.iteritems():
      connection.putheader(header_name, value)
    connection.endheaders()

    # If there is data, send it in the request.
    if body_parts:
      for part in body_parts:
        _send_data_part(part, connection)
   
    # Return the HTTP Response from the server.
    return connection.getresponse()


def _send_data_part(data, connection):
  if isinstance(data, (str, unicode)):
    # I might want to just allow str, not unicode.
    connection.send(data)
    return
  # Check to see if data is a file-like object that has a read method.
  elif hasattr(data, 'read'):
    # Read the file and send it a chunk at a time.
    while 1:
      binarydata = data.read(100000)
      if binarydata == '': break
      connection.send(binarydata)
    return
  else:
    # The data object was not a file.
    # Try to convert to a string and send the data.
    connection.send(str(data))
    return

