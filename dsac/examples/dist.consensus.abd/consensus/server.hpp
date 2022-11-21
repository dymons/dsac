#pragma once

#include <dsac/concurrency/executors/static_thread_pool.hpp>
#include <examples/dist.consensus.abd/consensus/factory.hpp>

#define CPPHTTPLIB_VERSION "0.11.2"

#ifndef CPPHTTPLIB_KEEPALIVE_TIMEOUT_SECOND
#define CPPHTTPLIB_KEEPALIVE_TIMEOUT_SECOND 5
#endif

#ifndef CPPHTTPLIB_KEEPALIVE_MAX_COUNT
#define CPPHTTPLIB_KEEPALIVE_MAX_COUNT 5
#endif

#ifndef CPPHTTPLIB_CONNECTION_TIMEOUT_SECOND
#define CPPHTTPLIB_CONNECTION_TIMEOUT_SECOND 300
#endif

#ifndef CPPHTTPLIB_CONNECTION_TIMEOUT_USECOND
#define CPPHTTPLIB_CONNECTION_TIMEOUT_USECOND 0
#endif

#ifndef CPPHTTPLIB_READ_TIMEOUT_SECOND
#define CPPHTTPLIB_READ_TIMEOUT_SECOND 5
#endif

#ifndef CPPHTTPLIB_READ_TIMEOUT_USECOND
#define CPPHTTPLIB_READ_TIMEOUT_USECOND 0
#endif

#ifndef CPPHTTPLIB_WRITE_TIMEOUT_SECOND
#define CPPHTTPLIB_WRITE_TIMEOUT_SECOND 5
#endif

#ifndef CPPHTTPLIB_WRITE_TIMEOUT_USECOND
#define CPPHTTPLIB_WRITE_TIMEOUT_USECOND 0
#endif

#ifndef CPPHTTPLIB_IDLE_INTERVAL_SECOND
#define CPPHTTPLIB_IDLE_INTERVAL_SECOND 0
#endif

#ifndef CPPHTTPLIB_IDLE_INTERVAL_USECOND
#define CPPHTTPLIB_IDLE_INTERVAL_USECOND 0
#endif

#ifndef CPPHTTPLIB_REQUEST_URI_MAX_LENGTH
#define CPPHTTPLIB_REQUEST_URI_MAX_LENGTH 8192
#endif

#ifndef CPPHTTPLIB_HEADER_MAX_LENGTH
#define CPPHTTPLIB_HEADER_MAX_LENGTH 8192
#endif

#ifndef CPPHTTPLIB_REDIRECT_MAX_COUNT
#define CPPHTTPLIB_REDIRECT_MAX_COUNT 20
#endif

#ifndef CPPHTTPLIB_MULTIPART_FORM_DATA_FILE_MAX_COUNT
#define CPPHTTPLIB_MULTIPART_FORM_DATA_FILE_MAX_COUNT 1024
#endif

#ifndef CPPHTTPLIB_PAYLOAD_MAX_LENGTH
#define CPPHTTPLIB_PAYLOAD_MAX_LENGTH ((std::numeric_limits<size_t>::max)())
#endif

#ifndef CPPHTTPLIB_FORM_URL_ENCODED_PAYLOAD_MAX_LENGTH
#define CPPHTTPLIB_FORM_URL_ENCODED_PAYLOAD_MAX_LENGTH 8192
#endif

#ifndef CPPHTTPLIB_TCP_NODELAY
#define CPPHTTPLIB_TCP_NODELAY false
#endif

#ifndef CPPHTTPLIB_RECV_BUFSIZ
#define CPPHTTPLIB_RECV_BUFSIZ size_t(4096u)
#endif

#ifndef CPPHTTPLIB_COMPRESSION_BUFSIZ
#define CPPHTTPLIB_COMPRESSION_BUFSIZ size_t(16384u)
#endif

#ifndef CPPHTTPLIB_THREAD_POOL_COUNT
#define CPPHTTPLIB_THREAD_POOL_COUNT \
  ((std::max)(8u, std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() - 1 : 0))
#endif

#ifndef CPPHTTPLIB_RECV_FLAGS
#define CPPHTTPLIB_RECV_FLAGS 0
#endif

#ifndef CPPHTTPLIB_SEND_FLAGS
#define CPPHTTPLIB_SEND_FLAGS 0
#endif

#ifndef CPPHTTPLIB_LISTEN_BACKLOG
#define CPPHTTPLIB_LISTEN_BACKLOG 5
#endif

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <pthread.h>
#include <resolv.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <csignal>

using socket_t = int;
#define INVALID_SOCKET (-1)

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cctype>
#include <climits>
#include <condition_variable>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <thread>

namespace httplib {

namespace detail {

template <class T, class... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type make_unique(Args &&...args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <class T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type make_unique(std::size_t n) {
  typedef typename std::remove_extent<T>::type RT;
  return std::unique_ptr<T>(new RT[n]);
}

struct ci {
  bool operator()(const std::string &s1, const std::string &s2) const {
    return std::lexicographical_compare(
        s1.begin(), s1.end(), s2.begin(), s2.end(), [](unsigned char c1, unsigned char c2) {
          return ::tolower(c1) < ::tolower(c2);
        });
  }
};

}  // namespace detail

using Headers = std::multimap<std::string, std::string, detail::ci>;

using Params = std::multimap<std::string, std::string>;
using Match  = std::smatch;

using Progress = std::function<bool(uint64_t current, uint64_t total)>;

struct Response;
using ResponseHandler = std::function<bool(const Response &response)>;

struct MultipartFormData {
  std::string name;
  std::string content;
  std::string filename;
  std::string content_type;
};
using MultipartFormDataItems = std::vector<MultipartFormData>;
using MultipartFormDataMap   = std::multimap<std::string, MultipartFormData>;

class DataSink {
public:
  DataSink()
    : os(&sb_)
    , sb_(*this) {
  }

  DataSink(const DataSink &)            = delete;
  DataSink &operator=(const DataSink &) = delete;
  DataSink(DataSink &&)                 = delete;
  DataSink &operator=(DataSink &&)      = delete;

  std::function<bool(const char *data, size_t data_len)> write;
  std::function<void()>                                  done;
  std::function<bool()>                                  is_writable;
  std::ostream                                           os;

private:
  class data_sink_streambuf : public std::streambuf {
  public:
    explicit data_sink_streambuf(DataSink &sink)
      : sink_(sink) {
    }

  protected:
    std::streamsize xsputn(const char *s, std::streamsize n) {
      sink_.write(s, static_cast<size_t>(n));
      return n;
    }

  private:
    DataSink &sink_;
  };

  data_sink_streambuf sb_;
};

using ContentProvider = std::function<bool(size_t offset, size_t length, DataSink &sink)>;

using ContentProviderWithoutLength = std::function<bool(size_t offset, DataSink &sink)>;

using ContentProviderResourceReleaser = std::function<void(bool success)>;

using ContentReceiverWithProgress =
    std::function<bool(const char *data, size_t data_length, uint64_t offset, uint64_t total_length)>;

using ContentReceiver = std::function<bool(const char *data, size_t data_length)>;

using MultipartContentHeader = std::function<bool(const MultipartFormData &file)>;

class ContentReader {
public:
  using Reader          = std::function<bool(ContentReceiver receiver)>;
  using MultipartReader = std::function<bool(MultipartContentHeader header, ContentReceiver receiver)>;

  ContentReader(Reader reader, MultipartReader multipart_reader)
    : reader_(std::move(reader))
    , multipart_reader_(std::move(multipart_reader)) {
  }

  bool operator()(MultipartContentHeader header, ContentReceiver receiver) const {
    return multipart_reader_(std::move(header), std::move(receiver));
  }

  bool operator()(ContentReceiver receiver) const {
    return reader_(std::move(receiver));
  }

  Reader          reader_;
  MultipartReader multipart_reader_;
};

using Range  = std::pair<ssize_t, ssize_t>;
using Ranges = std::vector<Range>;

struct Request {
  std::string method;
  std::string path;
  Headers     headers;
  std::string body;

  std::string remote_addr;
  int         remote_port = -1;

  // for server
  std::string          version;
  std::string          target;
  Params               params;
  MultipartFormDataMap files;
  Ranges               ranges;
  Match                matches;

  // for client
  ResponseHandler             response_handler;
  ContentReceiverWithProgress content_receiver;
  Progress                    progress;

  bool        has_header(const std::string &key) const;
  std::string get_header_value(const std::string &key, size_t id = 0) const;
  template <typename T>
  T      get_header_value(const std::string &key, size_t id = 0) const;
  size_t get_header_value_count(const std::string &key) const;
  void   set_header(const std::string &key, const std::string &val);

  bool        has_param(const std::string &key) const;
  std::string get_param_value(const std::string &key, size_t id = 0) const;
  size_t      get_param_value_count(const std::string &key) const;

  bool is_multipart_form_data() const;

  bool              has_file(const std::string &key) const;
  MultipartFormData get_file_value(const std::string &key) const;

  // private members...
  size_t          redirect_count_ = CPPHTTPLIB_REDIRECT_MAX_COUNT;
  size_t          content_length_ = 0;
  ContentProvider content_provider_;
  bool            is_chunked_content_provider_ = false;
  size_t          authorization_count_         = 0;
};

struct Response {
  std::string version;
  int         status = -1;
  std::string reason;
  Headers     headers;
  std::string body;
  std::string location;  // Redirect location

  bool        has_header(const std::string &key) const;
  std::string get_header_value(const std::string &key, size_t id = 0) const;
  template <typename T>
  T      get_header_value(const std::string &key, size_t id = 0) const;
  size_t get_header_value_count(const std::string &key) const;
  void   set_header(const std::string &key, const std::string &val);

  void set_redirect(const std::string &url, int status = 302);
  void set_content(const char *s, size_t n, const std::string &content_type);
  void set_content(const std::string &s, const std::string &content_type);

  void set_content_provider(
      size_t                          length,
      const std::string              &content_type,
      ContentProvider                 provider,
      ContentProviderResourceReleaser resource_releaser = nullptr);

  void set_content_provider(
      const std::string              &content_type,
      ContentProviderWithoutLength    provider,
      ContentProviderResourceReleaser resource_releaser = nullptr);

  void set_chunked_content_provider(
      const std::string              &content_type,
      ContentProviderWithoutLength    provider,
      ContentProviderResourceReleaser resource_releaser = nullptr);

  Response()                            = default;
  Response(const Response &)            = default;
  Response &operator=(const Response &) = default;
  Response(Response &&)                 = default;
  Response &operator=(Response &&)      = default;
  ~Response() {
    if (content_provider_resource_releaser_) {
      content_provider_resource_releaser_(content_provider_success_);
    }
  }

  // private members...
  size_t                          content_length_ = 0;
  ContentProvider                 content_provider_;
  ContentProviderResourceReleaser content_provider_resource_releaser_;
  bool                            is_chunked_content_provider_ = false;
  bool                            content_provider_success_    = false;
};

class Stream {
public:
  virtual ~Stream() = default;

  virtual bool is_readable() const = 0;
  virtual bool is_writable() const = 0;

  virtual ssize_t  read(char *ptr, size_t size)                             = 0;
  virtual ssize_t  write(const char *ptr, size_t size)                      = 0;
  virtual void     get_remote_ip_and_port(std::string &ip, int &port) const = 0;
  virtual socket_t socket() const                                           = 0;

  template <typename... Args>
  ssize_t write_format(const char *fmt, const Args &...args);
  ssize_t write(const char *ptr);
  ssize_t write(const std::string &s);
};

using SocketOptions = std::function<void(socket_t sock)>;

void default_socket_options(socket_t sock);

class Server {
public:
  using Handler = std::function<void(const Request &, Response &)>;

  using ExceptionHandler = std::function<void(const Request &, Response &, std::exception_ptr ep)>;

  enum class HandlerResponse {
    Handled,
    Unhandled,
  };
  using HandlerWithResponse = std::function<HandlerResponse(const Request &, Response &)>;

  using Expect100ContinueHandler = std::function<int(const Request &, Response &)>;

  Server();

  virtual ~Server();

  virtual bool is_valid() const;

  bool    set_base_dir(const std::string &dir, const std::string &mount_point = std::string());
  bool    set_mount_point(const std::string &mount_point, const std::string &dir, Headers headers = Headers());
  bool    remove_mount_point(const std::string &mount_point);
  Server &set_file_extension_and_mimetype_mapping(const std::string &ext, const std::string &mime);
  Server &set_file_request_handler(Handler handler);

  Server &set_error_handler(HandlerWithResponse handler);
  Server &set_error_handler(Handler handler);
  Server &set_exception_handler(ExceptionHandler handler);
  Server &set_pre_routing_handler(HandlerWithResponse handler);
  Server &set_post_routing_handler(Handler handler);

  Server &set_expect_100_continue_handler(Expect100ContinueHandler handler);

  Server &set_address_family(int family);
  Server &set_tcp_nodelay(bool on);
  Server &set_socket_options(SocketOptions socket_options);

  Server &set_default_headers(Headers headers);

  Server &set_keep_alive_max_count(size_t count);
  Server &set_keep_alive_timeout(time_t sec);

  Server &set_read_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  Server &set_read_timeout(const std::chrono::duration<Rep, Period> &duration);

  Server &set_write_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  Server &set_write_timeout(const std::chrono::duration<Rep, Period> &duration);

  Server &set_idle_interval(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  Server &set_idle_interval(const std::chrono::duration<Rep, Period> &duration);

  Server &set_payload_max_length(size_t length);

  bool bind_to_port(const std::string &host, int port, int socket_flags = 0);
  int  bind_to_any_port(const std::string &host, int socket_flags = 0);
  bool listen_after_bind();

  bool listen(const std::string &host, int port, int socket_flags = 0);

  bool is_running() const;
  void stop();

  dsac::executor_base_ref new_task_queue = dsac::make_static_thread_pool(4U);

protected:
  bool process_request(
      Stream                               &strm,
      bool                                  close_connection,
      bool                                 &connection_closed,
      const std::function<void(Request &)> &setup_request);

  std::atomic<socket_t> svr_sock_;
  size_t                keep_alive_max_count_   = CPPHTTPLIB_KEEPALIVE_MAX_COUNT;
  time_t                keep_alive_timeout_sec_ = CPPHTTPLIB_KEEPALIVE_TIMEOUT_SECOND;
  time_t                read_timeout_sec_       = CPPHTTPLIB_READ_TIMEOUT_SECOND;
  time_t                read_timeout_usec_      = CPPHTTPLIB_READ_TIMEOUT_USECOND;
  time_t                write_timeout_sec_      = CPPHTTPLIB_WRITE_TIMEOUT_SECOND;
  time_t                write_timeout_usec_     = CPPHTTPLIB_WRITE_TIMEOUT_USECOND;
  time_t                idle_interval_sec_      = CPPHTTPLIB_IDLE_INTERVAL_SECOND;
  time_t                idle_interval_usec_     = CPPHTTPLIB_IDLE_INTERVAL_USECOND;
  size_t                payload_max_length_     = CPPHTTPLIB_PAYLOAD_MAX_LENGTH;

private:
  using Handlers = std::vector<std::pair<std::regex, Handler>>;

  socket_t create_server_socket(
      const std::string &host, int port, int socket_flags, SocketOptions socket_options) const;
  int  bind_internal(const std::string &host, int port, int socket_flags);
  bool listen_internal();

  bool routing(Request &req, Response &res, Stream &strm);
  bool handle_file_request(const Request &req, Response &res, bool head = false);
  bool dispatch_request(Request &req, Response &res);

  bool parse_request_line(const char *s, Request &req);
  bool write_response(Stream &strm, bool close_connection, const Request &req, Response &res);
  bool write_response_with_content(Stream &strm, bool close_connection, const Request &req, Response &res);
  bool write_response_core(
      Stream &strm, bool close_connection, const Request &req, Response &res, bool need_apply_ranges);
  bool write_content_with_provider(
      Stream &strm, const Request &req, Response &res, const std::string &boundary, const std::string &content_type);
  bool read_content(Stream &strm, Request &req, Response &res);
  bool read_content_with_content_receiver(
      Stream                &strm,
      Request               &req,
      Response              &res,
      ContentReceiver        receiver,
      MultipartContentHeader multipart_header,
      ContentReceiver        multipart_receiver);
  bool read_content_core(
      Stream                &strm,
      Request               &req,
      Response              &res,
      ContentReceiver        receiver,
      MultipartContentHeader mulitpart_header,
      ContentReceiver        multipart_receiver);

  virtual bool process_and_close_socket(socket_t sock);

  struct MountPointEntry {
    std::string mount_point;
    std::string base_dir;
    Headers     headers;
  };
  std::vector<MountPointEntry> base_dirs_;

  std::atomic<bool>                  is_running_;
  std::map<std::string, std::string> file_extension_and_mimetype_map_;
  Handler                            file_request_handler_;
  Handlers                           get_handlers_;
  Handlers                           put_handlers_;
  Handlers                           patch_handlers_;
  Handlers                           delete_handlers_;
  Handlers                           options_handlers_;
  HandlerWithResponse                error_handler_;
  ExceptionHandler                   exception_handler_;
  HandlerWithResponse                pre_routing_handler_;
  Handler                            post_routing_handler_;
  Expect100ContinueHandler           expect_100_continue_handler_;

  int           address_family_ = AF_UNSPEC;
  bool          tcp_nodelay_    = CPPHTTPLIB_TCP_NODELAY;
  SocketOptions socket_options_ = default_socket_options;

  Headers default_headers_;
};

enum class Error {
  Success = 0,
  Unknown,
  Connection,
  BindIPAddress,
  Read,
  Write,
  ExceedRedirectCount,
  Canceled,
  UnsupportedMultipartBoundaryChars,
  Compression,
  ConnectionTimeout,
};

std::string to_string(const Error error);

std::ostream &operator<<(std::ostream &os, const Error &obj);

class Result {
public:
  Result(std::unique_ptr<Response> &&res, Error err, Headers &&request_headers = Headers{})
    : res_(std::move(res))
    , err_(err)
    , request_headers_(std::move(request_headers)) {
  }
  // Response
  operator bool() const {
    return res_ != nullptr;
  }
  bool operator==(std::nullptr_t) const {
    return res_ == nullptr;
  }
  bool operator!=(std::nullptr_t) const {
    return res_ != nullptr;
  }
  const Response &value() const {
    return *res_;
  }
  Response &value() {
    return *res_;
  }
  const Response &operator*() const {
    return *res_;
  }
  Response &operator*() {
    return *res_;
  }
  const Response *operator->() const {
    return res_.get();
  }
  Response *operator->() {
    return res_.get();
  }

  // Error
  Error error() const {
    return err_;
  }

  // Request Headers
  bool        has_request_header(const std::string &key) const;
  std::string get_request_header_value(const std::string &key, size_t id = 0) const;
  template <typename T>
  T      get_request_header_value(const std::string &key, size_t id = 0) const;
  size_t get_request_header_value_count(const std::string &key) const;

private:
  std::unique_ptr<Response> res_;
  Error                     err_;
  Headers                   request_headers_;
};

class ClientImpl {
public:
  explicit ClientImpl(const std::string &host);

  explicit ClientImpl(const std::string &host, int port);

  explicit ClientImpl(
      const std::string &host, int port, const std::string &client_cert_path, const std::string &client_key_path);

  virtual ~ClientImpl();

  virtual bool is_valid() const;

  bool   send(Request &req, Response &res, Error &error);
  Result send(const Request &req);

  size_t is_socket_open() const;

  socket_t socket() const;

  void stop();

  void set_hostname_addr_map(std::map<std::string, std::string> addr_map);

  void set_default_headers(Headers headers);

  void set_address_family(int family);
  void set_tcp_nodelay(bool on);
  void set_socket_options(SocketOptions socket_options);

  void set_connection_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  void set_connection_timeout(const std::chrono::duration<Rep, Period> &duration);

  void set_read_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  void set_read_timeout(const std::chrono::duration<Rep, Period> &duration);

  void set_write_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  void set_write_timeout(const std::chrono::duration<Rep, Period> &duration);

  void set_basic_auth(const std::string &username, const std::string &password);
  void set_bearer_token_auth(const std::string &token);

  void set_keep_alive(bool on);
  void set_follow_location(bool on);

  void set_url_encode(bool on);

  void set_compress(bool on);

  void set_decompress(bool on);

  void set_interface(const std::string &intf);

  void set_proxy(const std::string &host, int port);
  void set_proxy_basic_auth(const std::string &username, const std::string &password);
  void set_proxy_bearer_token_auth(const std::string &token);

protected:
  struct Socket {
    socket_t sock = INVALID_SOCKET;

    bool is_open() const {
      return sock != INVALID_SOCKET;
    }
  };

  Result send_(Request &&req);

  virtual bool create_and_connect_socket(Socket &socket, Error &error);

  void shutdown_socket(Socket &socket);
  void close_socket(Socket &socket);

  bool process_request(Stream &strm, Request &req, Response &res, bool close_connection, Error &error);

  bool write_content_with_provider(Stream &strm, const Request &req, Error &error);

  void copy_settings(const ClientImpl &rhs);

  // Socket endoint information
  const std::string host_;
  const int         port_;
  const std::string host_and_port_;

  // Current open socket
  Socket               socket_;
  mutable std::mutex   socket_mutex_;
  std::recursive_mutex request_mutex_;

  // These are all protected under socket_mutex
  size_t          socket_requests_in_flight_                    = 0;
  std::thread::id socket_requests_are_from_thread_              = std::thread::id();
  bool            socket_should_be_closed_when_request_is_done_ = false;

  // Hostname-IP map
  std::map<std::string, std::string> addr_map_;

  // Default headers
  Headers default_headers_;

  // Settings
  std::string client_cert_path_;
  std::string client_key_path_;

  time_t connection_timeout_sec_  = CPPHTTPLIB_CONNECTION_TIMEOUT_SECOND;
  time_t connection_timeout_usec_ = CPPHTTPLIB_CONNECTION_TIMEOUT_USECOND;
  time_t read_timeout_sec_        = CPPHTTPLIB_READ_TIMEOUT_SECOND;
  time_t read_timeout_usec_       = CPPHTTPLIB_READ_TIMEOUT_USECOND;
  time_t write_timeout_sec_       = CPPHTTPLIB_WRITE_TIMEOUT_SECOND;
  time_t write_timeout_usec_      = CPPHTTPLIB_WRITE_TIMEOUT_USECOND;

  std::string basic_auth_username_;
  std::string basic_auth_password_;
  std::string bearer_token_auth_token_;

  bool keep_alive_      = false;
  bool follow_location_ = false;

  bool url_encode_ = true;

  int           address_family_ = AF_UNSPEC;
  bool          tcp_nodelay_    = CPPHTTPLIB_TCP_NODELAY;
  SocketOptions socket_options_ = nullptr;

  bool compress_   = false;
  bool decompress_ = true;

  std::string interface_;

  std::string proxy_host_;
  int         proxy_port_ = -1;

  std::string proxy_basic_auth_username_;
  std::string proxy_basic_auth_password_;
  std::string proxy_bearer_token_auth_token_;

private:
  socket_t create_client_socket(Error &error) const;
  bool     read_response_line(Stream &strm, const Request &req, Response &res);
  bool     write_request(Stream &strm, Request &req, bool close_connection, Error &error);
  bool     redirect(Request &req, Response &res, Error &error);
  bool     handle_request(Stream &strm, Request &req, Response &res, bool close_connection, Error &error);
  std::unique_ptr<Response> send_with_content_provider(
      Request                     &req,
      const char                  *body,
      size_t                       content_length,
      ContentProvider              content_provider,
      ContentProviderWithoutLength content_provider_without_length,
      const std::string           &content_type,
      Error                       &error);
  Result send_with_content_provider(
      const std::string           &method,
      const std::string           &path,
      const Headers               &headers,
      const char                  *body,
      size_t                       content_length,
      ContentProvider              content_provider,
      ContentProviderWithoutLength content_provider_without_length,
      const std::string           &content_type);

  std::string adjust_host_string(const std::string &host) const;

  virtual bool process_socket(const Socket &socket, std::function<bool(Stream &strm)> callback);
};

class Client {
public:
  // Universal interface
  explicit Client(const std::string &scheme_host_port);

  explicit Client(
      const std::string &scheme_host_port, const std::string &client_cert_path, const std::string &client_key_path);

  // HTTP only interface
  explicit Client(const std::string &host, int port);

  explicit Client(
      const std::string &host, int port, const std::string &client_cert_path, const std::string &client_key_path);

  Client(Client &&) = default;

  ~Client();

  bool is_valid() const;

  bool   send(Request &req, Response &res, Error &error);
  Result send(const Request &req);

  size_t is_socket_open() const;

  socket_t socket() const;

  void stop();

  void set_hostname_addr_map(std::map<std::string, std::string> addr_map);

  void set_default_headers(Headers headers);

  void set_address_family(int family);
  void set_tcp_nodelay(bool on);
  void set_socket_options(SocketOptions socket_options);

  void set_connection_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  void set_connection_timeout(const std::chrono::duration<Rep, Period> &duration);

  void set_read_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  void set_read_timeout(const std::chrono::duration<Rep, Period> &duration);

  void set_write_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period>
  void set_write_timeout(const std::chrono::duration<Rep, Period> &duration);

  void set_basic_auth(const std::string &username, const std::string &password);
  void set_bearer_token_auth(const std::string &token);

  void set_keep_alive(bool on);
  void set_follow_location(bool on);

  void set_url_encode(bool on);

  void set_compress(bool on);

  void set_decompress(bool on);

  void set_interface(const std::string &intf);

  void set_proxy(const std::string &host, int port);
  void set_proxy_basic_auth(const std::string &username, const std::string &password);
  void set_proxy_bearer_token_auth(const std::string &token);

private:
  std::unique_ptr<ClientImpl> cli_;
};

/*
 * Implementation of template methods.
 */

namespace detail {

template <typename T, typename U>
inline void duration_to_sec_and_usec(const T &duration, U callback) {
  auto sec  = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  auto usec = std::chrono::duration_cast<std::chrono::microseconds>(duration - std::chrono::seconds(sec)).count();
  callback(sec, usec);
}

template <typename T>
inline T get_header_value(
    const Headers & /*headers*/, const std::string & /*key*/, size_t /*id*/ = 0, uint64_t /*def*/ = 0) {
}

template <>
inline uint64_t get_header_value<uint64_t>(const Headers &headers, const std::string &key, size_t id, uint64_t def) {
  auto rng = headers.equal_range(key);
  auto it  = rng.first;
  std::advance(it, static_cast<ssize_t>(id));
  if (it != rng.second) {
    return std::strtoull(it->second.data(), nullptr, 10);
  }
  return def;
}

}  // namespace detail

template <typename T>
inline T Request::get_header_value(const std::string &key, size_t id) const {
  return detail::get_header_value<T>(headers, key, id, 0);
}

template <typename T>
inline T Response::get_header_value(const std::string &key, size_t id) const {
  return detail::get_header_value<T>(headers, key, id, 0);
}

template <typename... Args>
inline ssize_t Stream::write_format(const char *fmt, const Args &...args) {
  const auto               bufsiz = 2048;
  std::array<char, bufsiz> buf{};

  auto sn = snprintf(buf.data(), buf.size() - 1, fmt, args...);
  if (sn <= 0) {
    return sn;
  }

  auto n = static_cast<size_t>(sn);

  if (n >= buf.size() - 1) {
    std::vector<char> glowable_buf(buf.size());

    while (n >= glowable_buf.size() - 1) {
      glowable_buf.resize(glowable_buf.size() * 2);
      n = static_cast<size_t>(snprintf(&glowable_buf[0], glowable_buf.size() - 1, fmt, args...));
    }
    return write(&glowable_buf[0], n);
  } else {
    return write(buf.data(), n);
  }
}

inline void default_socket_options(socket_t sock) {
  int yes = 1;
#ifdef SO_REUSEPORT
  setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<void *>(&yes), sizeof(yes));
#else
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<void *>(&yes), sizeof(yes));
#endif
}

template <class Rep, class Period>
inline Server &Server::set_read_timeout(const std::chrono::duration<Rep, Period> &duration) {
  detail::duration_to_sec_and_usec(duration, [&](time_t sec, time_t usec) { set_read_timeout(sec, usec); });
  return *this;
}

template <class Rep, class Period>
inline Server &Server::set_write_timeout(const std::chrono::duration<Rep, Period> &duration) {
  detail::duration_to_sec_and_usec(duration, [&](time_t sec, time_t usec) { set_write_timeout(sec, usec); });
  return *this;
}

template <class Rep, class Period>
inline Server &Server::set_idle_interval(const std::chrono::duration<Rep, Period> &duration) {
  detail::duration_to_sec_and_usec(duration, [&](time_t sec, time_t usec) { set_idle_interval(sec, usec); });
  return *this;
}

inline std::string to_string(const Error error) {
  switch (error) {
    case Error::Success:
      return "Success";
    case Error::Connection:
      return "Connection";
    case Error::BindIPAddress:
      return "BindIPAddress";
    case Error::Read:
      return "Read";
    case Error::Write:
      return "Write";
    case Error::ExceedRedirectCount:
      return "ExceedRedirectCount";
    case Error::Canceled:
      return "Canceled";
    case Error::UnsupportedMultipartBoundaryChars:
      return "UnsupportedMultipartBoundaryChars";
    case Error::Compression:
      return "Compression";
    case Error::ConnectionTimeout:
      return "ConnectionTimeout";
    case Error::Unknown:
      return "Unknown";
    default:
      break;
  }

  return "Invalid";
}

inline std::ostream &operator<<(std::ostream &os, const Error &obj) {
  os << to_string(obj);
  os << " (" << static_cast<std::underlying_type<Error>::type>(obj) << ')';
  return os;
}

template <typename T>
inline T Result::get_request_header_value(const std::string &key, size_t id) const {
  return detail::get_header_value<T>(request_headers_, key, id, 0);
}

template <class Rep, class Period>
inline void ClientImpl::set_connection_timeout(const std::chrono::duration<Rep, Period> &duration) {
  detail::duration_to_sec_and_usec(duration, [&](time_t sec, time_t usec) { set_connection_timeout(sec, usec); });
}

template <class Rep, class Period>
inline void ClientImpl::set_read_timeout(const std::chrono::duration<Rep, Period> &duration) {
  detail::duration_to_sec_and_usec(duration, [&](time_t sec, time_t usec) { set_read_timeout(sec, usec); });
}

template <class Rep, class Period>
inline void ClientImpl::set_write_timeout(const std::chrono::duration<Rep, Period> &duration) {
  detail::duration_to_sec_and_usec(duration, [&](time_t sec, time_t usec) { set_write_timeout(sec, usec); });
}

template <class Rep, class Period>
inline void Client::set_connection_timeout(const std::chrono::duration<Rep, Period> &duration) {
  cli_->set_connection_timeout(duration);
}

template <class Rep, class Period>
inline void Client::set_read_timeout(const std::chrono::duration<Rep, Period> &duration) {
  cli_->set_read_timeout(duration);
}

template <class Rep, class Period>
inline void Client::set_write_timeout(const std::chrono::duration<Rep, Period> &duration) {
  cli_->set_write_timeout(duration);
}

/*
 * Forward declarations and types that will be part of the .h file if split into
 * .h + .cc.
 */

std::string hosted_at(const std::string &hostname);

void hosted_at(const std::string &hostname, std::vector<std::string> &addrs);

std::string append_query_params(const std::string &path, const Params &params);

std::pair<std::string, std::string> make_range_header(Ranges ranges);

std::pair<std::string, std::string> make_basic_authentication_header(
    const std::string &username, const std::string &password, bool is_proxy = false);

namespace detail {

std::string encode_query_param(const std::string &value);

std::string decode_url(const std::string &s, bool convert_plus_to_space);

void read_file(const std::string &path, std::string &out);

std::string trim_copy(const std::string &s);

void split(const char *b, const char *e, char d, std::function<void(const char *, const char *)> fn);

bool process_client_socket(
    socket_t                      sock,
    time_t                        read_timeout_sec,
    time_t                        read_timeout_usec,
    time_t                        write_timeout_sec,
    time_t                        write_timeout_usec,
    std::function<bool(Stream &)> callback);

socket_t create_client_socket(
    const std::string &host,
    const std::string &ip,
    int                port,
    int                address_family,
    bool               tcp_nodelay,
    SocketOptions      socket_options,
    time_t             connection_timeout_sec,
    time_t             connection_timeout_usec,
    time_t             read_timeout_sec,
    time_t             read_timeout_usec,
    time_t             write_timeout_sec,
    time_t             write_timeout_usec,
    const std::string &intf,
    Error             &error);

const char *get_header_value(const Headers &headers, const std::string &key, size_t id = 0, const char *def = nullptr);

std::string params_to_query_str(const Params &params);

void parse_query_text(const std::string &s, Params &params);

bool parse_range_header(const std::string &s, Ranges &ranges);

int close_socket(socket_t sock);

ssize_t send_socket(socket_t sock, const void *ptr, size_t size, int flags);

ssize_t read_socket(socket_t sock, void *ptr, size_t size, int flags);

enum class EncodingType { None = 0, Gzip, Brotli };

EncodingType encoding_type(const Request &req, const Response &res);

class BufferStream : public Stream {
public:
  BufferStream()           = default;
  ~BufferStream() override = default;

  bool     is_readable() const override;
  bool     is_writable() const override;
  ssize_t  read(char *ptr, size_t size) override;
  ssize_t  write(const char *ptr, size_t size) override;
  void     get_remote_ip_and_port(std::string &ip, int &port) const override;
  socket_t socket() const override;

  const std::string &get_buffer() const;

private:
  std::string buffer;
  size_t      position = 0;
};

class compressor {
public:
  virtual ~compressor() = default;

  typedef std::function<bool(const char *data, size_t data_len)> Callback;
  virtual bool compress(const char *data, size_t data_length, bool last, Callback callback) = 0;
};

class decompressor {
public:
  virtual ~decompressor() = default;

  virtual bool is_valid() const = 0;

  typedef std::function<bool(const char *data, size_t data_len)> Callback;
  virtual bool decompress(const char *data, size_t data_length, Callback callback) = 0;
};

class nocompressor : public compressor {
public:
  virtual ~nocompressor() = default;

  bool compress(const char *data, size_t data_length, bool /*last*/, Callback callback) override;
};

class stream_line_reader {
public:
  stream_line_reader(Stream &strm, char *fixed_buffer, size_t fixed_buffer_size);
  const char *ptr() const;
  size_t      size() const;
  bool        end_with_crlf() const;
  bool        getline();

private:
  void append(char c);

  Stream      &strm_;
  char        *fixed_buffer_;
  const size_t fixed_buffer_size_;
  size_t       fixed_buffer_used_size_ = 0;
  std::string  glowable_buffer_;
};

}  // namespace detail

// ----------------------------------------------------------------------------

/*
 * Implementation that will be part of the .cc file if split into .h + .cc.
 */

namespace detail {

inline bool is_hex(char c, int &v) {
  if (0x20 <= c && isdigit(c)) {
    v = c - '0';
    return true;
  } else if ('A' <= c && c <= 'F') {
    v = c - 'A' + 10;
    return true;
  } else if ('a' <= c && c <= 'f') {
    v = c - 'a' + 10;
    return true;
  }
  return false;
}

inline bool from_hex_to_i(const std::string &s, size_t i, size_t cnt, int &val) {
  if (i >= s.size()) {
    return false;
  }

  val = 0;
  for (; cnt; i++, cnt--) {
    if (!s[i]) {
      return false;
    }
    int v = 0;
    if (is_hex(s[i], v)) {
      val = val * 16 + v;
    } else {
      return false;
    }
  }
  return true;
}

inline std::string from_i_to_hex(size_t n) {
  const char *charset = "0123456789abcdef";
  std::string ret;
  do {
    ret = charset[n & 15] + ret;
    n >>= 4;
  } while (n > 0);
  return ret;
}

inline size_t to_utf8(int code, char *buff) {
  if (code < 0x0080) {
    buff[0] = (code & 0x7F);
    return 1;
  } else if (code < 0x0800) {
    buff[0] = static_cast<char>(0xC0 | ((code >> 6) & 0x1F));
    buff[1] = static_cast<char>(0x80 | (code & 0x3F));
    return 2;
  } else if (code < 0xD800) {
    buff[0] = static_cast<char>(0xE0 | ((code >> 12) & 0xF));
    buff[1] = static_cast<char>(0x80 | ((code >> 6) & 0x3F));
    buff[2] = static_cast<char>(0x80 | (code & 0x3F));
    return 3;
  } else if (code < 0xE000) {  // D800 - DFFF is invalid...
    return 0;
  } else if (code < 0x10000) {
    buff[0] = static_cast<char>(0xE0 | ((code >> 12) & 0xF));
    buff[1] = static_cast<char>(0x80 | ((code >> 6) & 0x3F));
    buff[2] = static_cast<char>(0x80 | (code & 0x3F));
    return 3;
  } else if (code < 0x110000) {
    buff[0] = static_cast<char>(0xF0 | ((code >> 18) & 0x7));
    buff[1] = static_cast<char>(0x80 | ((code >> 12) & 0x3F));
    buff[2] = static_cast<char>(0x80 | ((code >> 6) & 0x3F));
    buff[3] = static_cast<char>(0x80 | (code & 0x3F));
    return 4;
  }

  // NOTREACHED
  return 0;
}

// NOTE: This code came up with the following stackoverflow post:
// https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
inline std::string base64_encode(const std::string &in) {
  static const auto lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  std::string out;
  out.reserve(in.size());

  int val  = 0;
  int valb = -6;

  for (auto c : in) {
    val = (val << 8) + static_cast<uint8_t>(c);
    valb += 8;
    while (valb >= 0) {
      out.push_back(lookup[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }

  if (valb > -6) {
    out.push_back(lookup[((val << 8) >> (valb + 8)) & 0x3F]);
  }

  while (out.size() % 4) {
    out.push_back('=');
  }

  return out;
}

inline bool is_file(const std::string &path) {
  struct stat st;
  return stat(path.c_str(), &st) >= 0 && S_ISREG(st.st_mode);
}

inline bool is_dir(const std::string &path) {
  struct stat st;
  return stat(path.c_str(), &st) >= 0 && S_ISDIR(st.st_mode);
}

inline bool is_valid_path(const std::string &path) {
  size_t level = 0;
  size_t i     = 0;

  // Skip slash
  while (i < path.size() && path[i] == '/') {
    i++;
  }

  while (i < path.size()) {
    // Read component
    auto beg = i;
    while (i < path.size() && path[i] != '/') {
      i++;
    }

    auto len = i - beg;
    assert(len > 0);

    if (!path.compare(beg, len, ".")) {
      ;
    } else if (!path.compare(beg, len, "..")) {
      if (level == 0) {
        return false;
      }
      level--;
    } else {
      level++;
    }

    // Skip slash
    while (i < path.size() && path[i] == '/') {
      i++;
    }
  }

  return true;
}

inline std::string encode_query_param(const std::string &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (auto c : value) {
    if (std::isalnum(static_cast<uint8_t>(c)) || c == '-' || c == '_' || c == '.' || c == '!' || c == '~' || c == '*' ||
        c == '\'' || c == '(' || c == ')') {
      escaped << c;
    } else {
      escaped << std::uppercase;
      escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
      escaped << std::nouppercase;
    }
  }

  return escaped.str();
}

inline std::string encode_url(const std::string &s) {
  std::string result;
  result.reserve(s.size());

  for (size_t i = 0; s[i]; i++) {
    switch (s[i]) {
      case ' ':
        result += "%20";
        break;
      case '+':
        result += "%2B";
        break;
      case '\r':
        result += "%0D";
        break;
      case '\n':
        result += "%0A";
        break;
      case '\'':
        result += "%27";
        break;
      case ',':
        result += "%2C";
        break;
      // case ':': result += "%3A"; break; // ok? probably...
      case ';':
        result += "%3B";
        break;
      default:
        auto c = static_cast<uint8_t>(s[i]);
        if (c >= 0x80) {
          result += '%';
          char hex[4];
          auto len = snprintf(hex, sizeof(hex) - 1, "%02X", c);
          assert(len == 2);
          result.append(hex, static_cast<size_t>(len));
        } else {
          result += s[i];
        }
        break;
    }
  }

  return result;
}

inline std::string decode_url(const std::string &s, bool convert_plus_to_space) {
  std::string result;

  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == '%' && i + 1 < s.size()) {
      if (s[i + 1] == 'u') {
        int val = 0;
        if (from_hex_to_i(s, i + 2, 4, val)) {
          // 4 digits Unicode codes
          char   buff[4];
          size_t len = to_utf8(val, buff);
          if (len > 0) {
            result.append(buff, len);
          }
          i += 5;  // 'u0000'
        } else {
          result += s[i];
        }
      } else {
        int val = 0;
        if (from_hex_to_i(s, i + 1, 2, val)) {
          // 2 digits hex codes
          result += static_cast<char>(val);
          i += 2;  // '00'
        } else {
          result += s[i];
        }
      }
    } else if (convert_plus_to_space && s[i] == '+') {
      result += ' ';
    } else {
      result += s[i];
    }
  }

  return result;
}

inline void read_file(const std::string &path, std::string &out) {
  std::ifstream fs(path, std::ios_base::binary);
  fs.seekg(0, std::ios_base::end);
  auto size = fs.tellg();
  fs.seekg(0);
  out.resize(static_cast<size_t>(size));
  fs.read(&out[0], static_cast<std::streamsize>(size));
}

inline std::string file_extension(const std::string &path) {
  std::smatch m;
  static auto re = std::regex("\\.([a-zA-Z0-9]+)$");
  if (std::regex_search(path, m, re)) {
    return m[1].str();
  }
  return std::string();
}

inline bool is_space_or_tab(char c) {
  return c == ' ' || c == '\t';
}

inline std::pair<size_t, size_t> trim(const char *b, const char *e, size_t left, size_t right) {
  while (b + left < e && is_space_or_tab(b[left])) {
    left++;
  }
  while (right > 0 && is_space_or_tab(b[right - 1])) {
    right--;
  }
  return std::make_pair(left, right);
}

inline std::string trim_copy(const std::string &s) {
  auto r = trim(s.data(), s.data() + s.size(), 0, s.size());
  return s.substr(r.first, r.second - r.first);
}

inline void split(const char *b, const char *e, char d, std::function<void(const char *, const char *)> fn) {
  size_t i   = 0;
  size_t beg = 0;

  while (e ? (b + i < e) : (b[i] != '\0')) {
    if (b[i] == d) {
      auto r = trim(b, e, beg, i);
      if (r.first < r.second) {
        fn(&b[r.first], &b[r.second]);
      }
      beg = i + 1;
    }
    i++;
  }

  if (i) {
    auto r = trim(b, e, beg, i);
    if (r.first < r.second) {
      fn(&b[r.first], &b[r.second]);
    }
  }
}

inline stream_line_reader::stream_line_reader(Stream &strm, char *fixed_buffer, size_t fixed_buffer_size)
  : strm_(strm)
  , fixed_buffer_(fixed_buffer)
  , fixed_buffer_size_(fixed_buffer_size) {
}

inline const char *stream_line_reader::ptr() const {
  if (glowable_buffer_.empty()) {
    return fixed_buffer_;
  } else {
    return glowable_buffer_.data();
  }
}

inline size_t stream_line_reader::size() const {
  if (glowable_buffer_.empty()) {
    return fixed_buffer_used_size_;
  } else {
    return glowable_buffer_.size();
  }
}

inline bool stream_line_reader::end_with_crlf() const {
  auto end = ptr() + size();
  return size() >= 2 && end[-2] == '\r' && end[-1] == '\n';
}

inline bool stream_line_reader::getline() {
  fixed_buffer_used_size_ = 0;
  glowable_buffer_.clear();

  for (size_t i = 0;; i++) {
    char byte;
    auto n = strm_.read(&byte, 1);

    if (n < 0) {
      return false;
    } else if (n == 0) {
      if (i == 0) {
        return false;
      } else {
        break;
      }
    }

    append(byte);

    if (byte == '\n') {
      break;
    }
  }

  return true;
}

inline void stream_line_reader::append(char c) {
  if (fixed_buffer_used_size_ < fixed_buffer_size_ - 1) {
    fixed_buffer_[fixed_buffer_used_size_++] = c;
    fixed_buffer_[fixed_buffer_used_size_]   = '\0';
  } else {
    if (glowable_buffer_.empty()) {
      assert(fixed_buffer_[fixed_buffer_used_size_] == '\0');
      glowable_buffer_.assign(fixed_buffer_, fixed_buffer_used_size_);
    }
    glowable_buffer_ += c;
  }
}

inline int close_socket(socket_t sock) {
  return close(sock);
}

template <typename T>
inline ssize_t handle_EINTR(T fn) {
  ssize_t res = false;
  while (true) {
    res = fn();
    if (res < 0 && errno == EINTR) {
      continue;
    }
    break;
  }
  return res;
}

inline ssize_t read_socket(socket_t sock, void *ptr, size_t size, int flags) {
  return handle_EINTR([&]() { return recv(sock, ptr, size, flags); });
}

inline ssize_t send_socket(socket_t sock, const void *ptr, size_t size, int flags) {
  return handle_EINTR([&]() { return send(sock, ptr, size, flags); });
}

inline ssize_t select_read(socket_t sock, time_t sec, time_t usec) {
  struct pollfd pfd_read;
  pfd_read.fd     = sock;
  pfd_read.events = POLLIN;

  auto timeout = static_cast<int>(sec * 1000 + usec / 1000);

  return handle_EINTR([&]() { return poll(&pfd_read, 1, timeout); });
}

inline ssize_t select_write(socket_t sock, time_t sec, time_t usec) {
  struct pollfd pfd_read;
  pfd_read.fd     = sock;
  pfd_read.events = POLLOUT;

  auto timeout = static_cast<int>(sec * 1000 + usec / 1000);

  return handle_EINTR([&]() { return poll(&pfd_read, 1, timeout); });
}

inline Error wait_until_socket_is_ready(socket_t sock, time_t sec, time_t usec) {
  struct pollfd pfd_read;
  pfd_read.fd     = sock;
  pfd_read.events = POLLIN | POLLOUT;

  auto timeout = static_cast<int>(sec * 1000 + usec / 1000);

  auto poll_res = handle_EINTR([&]() { return poll(&pfd_read, 1, timeout); });

  if (poll_res == 0) {
    return Error::ConnectionTimeout;
  }

  if (poll_res > 0 && pfd_read.revents & (POLLIN | POLLOUT)) {
    int       error      = 0;
    socklen_t len        = sizeof(error);
    auto      res        = getsockopt(sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&error), &len);
    auto      successful = res >= 0 && !error;
    return successful ? Error::Success : Error::Connection;
  }

  return Error::Connection;
}

inline bool is_socket_alive(socket_t sock) {
  const auto val = detail::select_read(sock, 0, 0);
  if (val == 0) {
    return true;
  } else if (val < 0 && errno == EBADF) {
    return false;
  }
  char buf[1];
  return detail::read_socket(sock, &buf[0], sizeof(buf), MSG_PEEK) > 0;
}

class SocketStream : public Stream {
public:
  SocketStream(
      socket_t sock,
      time_t   read_timeout_sec,
      time_t   read_timeout_usec,
      time_t   write_timeout_sec,
      time_t   write_timeout_usec);
  ~SocketStream() override;

  bool     is_readable() const override;
  bool     is_writable() const override;
  ssize_t  read(char *ptr, size_t size) override;
  ssize_t  write(const char *ptr, size_t size) override;
  void     get_remote_ip_and_port(std::string &ip, int &port) const override;
  socket_t socket() const override;

private:
  socket_t sock_;
  time_t   read_timeout_sec_;
  time_t   read_timeout_usec_;
  time_t   write_timeout_sec_;
  time_t   write_timeout_usec_;

  std::vector<char> read_buff_;
  size_t            read_buff_off_          = 0;
  size_t            read_buff_content_size_ = 0;

  static const size_t read_buff_size_ = 1024 * 4;
};

inline bool keep_alive(socket_t sock, time_t keep_alive_timeout_sec) {
  using namespace std::chrono;
  auto start = steady_clock::now();
  while (true) {
    auto val = select_read(sock, 0, 10000);
    if (val < 0) {
      return false;
    } else if (val == 0) {
      auto current  = steady_clock::now();
      auto duration = duration_cast<milliseconds>(current - start);
      auto timeout  = keep_alive_timeout_sec * 1000;
      if (duration.count() > timeout) {
        return false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } else {
      return true;
    }
  }
}

template <typename T>
inline bool process_server_socket_core(
    const std::atomic<socket_t> &svr_sock,
    socket_t                     sock,
    size_t                       keep_alive_max_count,
    time_t                       keep_alive_timeout_sec,
    T                            callback) {
  assert(keep_alive_max_count > 0);
  auto ret   = false;
  auto count = keep_alive_max_count;
  while (svr_sock != INVALID_SOCKET && count > 0 && keep_alive(sock, keep_alive_timeout_sec)) {
    auto close_connection  = count == 1;
    auto connection_closed = false;
    ret                    = callback(close_connection, connection_closed);
    if (!ret || connection_closed) {
      break;
    }
    count--;
  }
  return ret;
}

template <typename T>
inline bool process_server_socket(
    const std::atomic<socket_t> &svr_sock,
    socket_t                     sock,
    size_t                       keep_alive_max_count,
    time_t                       keep_alive_timeout_sec,
    time_t                       read_timeout_sec,
    time_t                       read_timeout_usec,
    time_t                       write_timeout_sec,
    time_t                       write_timeout_usec,
    T                            callback) {
  return process_server_socket_core(
      svr_sock,
      sock,
      keep_alive_max_count,
      keep_alive_timeout_sec,
      [&](bool close_connection, bool &connection_closed) {
        SocketStream strm(sock, read_timeout_sec, read_timeout_usec, write_timeout_sec, write_timeout_usec);
        return callback(strm, close_connection, connection_closed);
      });
}

inline bool process_client_socket(
    socket_t                      sock,
    time_t                        read_timeout_sec,
    time_t                        read_timeout_usec,
    time_t                        write_timeout_sec,
    time_t                        write_timeout_usec,
    std::function<bool(Stream &)> callback) {
  SocketStream strm(sock, read_timeout_sec, read_timeout_usec, write_timeout_sec, write_timeout_usec);
  return callback(strm);
}

inline int shutdown_socket(socket_t sock) {
  return shutdown(sock, SHUT_RDWR);
}

template <typename BindOrConnect>
socket_t create_socket(
    const std::string &host,
    const std::string &ip,
    int                port,
    int                address_family,
    int                socket_flags,
    bool               tcp_nodelay,
    SocketOptions      socket_options,
    BindOrConnect      bind_or_connect) {
  const char      *node = nullptr;
  struct addrinfo  hints;
  struct addrinfo *result;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  if (!ip.empty()) {
    node = ip.c_str();
    // Ask getaddrinfo to convert IP in c-string to address
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags  = AI_NUMERICHOST;
  } else {
    if (!host.empty()) {
      node = host.c_str();
    }
    hints.ai_family = address_family;
    hints.ai_flags  = socket_flags;
  }

  if (hints.ai_family == AF_UNIX) {
    const auto addrlen = host.length();
    if (addrlen > sizeof(sockaddr_un::sun_path)) return INVALID_SOCKET;

    auto sock = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    if (sock != INVALID_SOCKET) {
      sockaddr_un addr;
      addr.sun_family = AF_UNIX;
      std::copy(host.begin(), host.end(), addr.sun_path);

      hints.ai_addr    = reinterpret_cast<sockaddr *>(&addr);
      hints.ai_addrlen = static_cast<socklen_t>(sizeof(addr) - sizeof(addr.sun_path) + addrlen);

      fcntl(sock, F_SETFD, FD_CLOEXEC);
      if (socket_options) {
        socket_options(sock);
      }

      if (!bind_or_connect(sock, hints)) {
        close_socket(sock);
        sock = INVALID_SOCKET;
      }
    }
    return sock;
  }

  auto service = std::to_string(port);

  if (getaddrinfo(node, service.c_str(), &hints, &result)) {
#if defined __linux__ && !defined __ANDROID__
    res_init();
#endif
    return INVALID_SOCKET;
  }

  for (auto rp = result; rp; rp = rp->ai_next) {
    auto sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if (sock == INVALID_SOCKET) {
      continue;
    }

    if (fcntl(sock, F_SETFD, FD_CLOEXEC) == -1) {
      continue;
    }

    if (tcp_nodelay) {
      int yes = 1;
      setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&yes), sizeof(yes));
    }

    if (socket_options) {
      socket_options(sock);
    }

    if (rp->ai_family == AF_INET6) {
      int no = 0;
      setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<char *>(&no), sizeof(no));
    }

    // bind or connect
    if (bind_or_connect(sock, *rp)) {
      freeaddrinfo(result);
      return sock;
    }

    close_socket(sock);
  }

  freeaddrinfo(result);
  return INVALID_SOCKET;
}

inline void set_nonblocking(socket_t sock, bool nonblocking) {
  auto flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, nonblocking ? (flags | O_NONBLOCK) : (flags & (~O_NONBLOCK)));
}

inline bool is_connection_error() {
  return errno != EINPROGRESS;
}

inline bool bind_ip_address(socket_t sock, const std::string &host) {
  struct addrinfo  hints;
  struct addrinfo *result;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  if (getaddrinfo(host.c_str(), "0", &hints, &result)) {
    return false;
  }

  auto ret = false;
  for (auto rp = result; rp; rp = rp->ai_next) {
    const auto &ai = *rp;
    if (!::bind(sock, ai.ai_addr, static_cast<socklen_t>(ai.ai_addrlen))) {
      ret = true;
      break;
    }
  }

  freeaddrinfo(result);
  return ret;
}

#if !defined _WIN32 && !defined ANDROID && !defined _AIX
#define USE_IF2IP
#endif

#ifdef USE_IF2IP
inline std::string if2ip(int address_family, const std::string &ifn) {
  struct ifaddrs *ifap;
  getifaddrs(&ifap);
  std::string addr_candidate;
  for (auto ifa = ifap; ifa; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr && ifn == ifa->ifa_name &&
        (AF_UNSPEC == address_family || ifa->ifa_addr->sa_family == address_family)) {
      if (ifa->ifa_addr->sa_family == AF_INET) {
        auto sa = reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);
        char buf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &sa->sin_addr, buf, INET_ADDRSTRLEN)) {
          freeifaddrs(ifap);
          return std::string(buf, INET_ADDRSTRLEN);
        }
      } else if (ifa->ifa_addr->sa_family == AF_INET6) {
        auto sa = reinterpret_cast<struct sockaddr_in6 *>(ifa->ifa_addr);
        if (!IN6_IS_ADDR_LINKLOCAL(&sa->sin6_addr)) {
          char buf[INET6_ADDRSTRLEN] = {};
          if (inet_ntop(AF_INET6, &sa->sin6_addr, buf, INET6_ADDRSTRLEN)) {
            // equivalent to mac's IN6_IS_ADDR_UNIQUE_LOCAL
            auto s6_addr_head = sa->sin6_addr.s6_addr[0];
            if (s6_addr_head == 0xfc || s6_addr_head == 0xfd) {
              addr_candidate = std::string(buf, INET6_ADDRSTRLEN);
            } else {
              freeifaddrs(ifap);
              return std::string(buf, INET6_ADDRSTRLEN);
            }
          }
        }
      }
    }
  }
  freeifaddrs(ifap);
  return addr_candidate;
}
#endif

inline socket_t create_client_socket(
    const std::string &host,
    const std::string &ip,
    int                port,
    int                address_family,
    bool               tcp_nodelay,
    SocketOptions      socket_options,
    time_t             connection_timeout_sec,
    time_t             connection_timeout_usec,
    time_t             read_timeout_sec,
    time_t             read_timeout_usec,
    time_t             write_timeout_sec,
    time_t             write_timeout_usec,
    const std::string &intf,
    Error             &error) {
  auto sock = create_socket(
      host,
      ip,
      port,
      address_family,
      0,
      tcp_nodelay,
      std::move(socket_options),
      [&](socket_t sock2, struct addrinfo &ai) -> bool {
        if (!intf.empty()) {
#ifdef USE_IF2IP
          auto ip_from_if = if2ip(address_family, intf);
          if (ip_from_if.empty()) {
            ip_from_if = intf;
          }
          if (!bind_ip_address(sock2, ip_from_if.c_str())) {
            error = Error::BindIPAddress;
            return false;
          }
#endif
        }

        set_nonblocking(sock2, true);

        auto ret = ::connect(sock2, ai.ai_addr, static_cast<socklen_t>(ai.ai_addrlen));

        if (ret < 0) {
          if (is_connection_error()) {
            error = Error::Connection;
            return false;
          }
          error = wait_until_socket_is_ready(sock2, connection_timeout_sec, connection_timeout_usec);
          if (error != Error::Success) {
            return false;
          }
        }

        set_nonblocking(sock2, false);

        {
          timeval tv;
          tv.tv_sec  = static_cast<long>(read_timeout_sec);
          tv.tv_usec = static_cast<decltype(tv.tv_usec)>(read_timeout_usec);
          setsockopt(sock2, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
        }
        {
          timeval tv;
          tv.tv_sec  = static_cast<long>(write_timeout_sec);
          tv.tv_usec = static_cast<decltype(tv.tv_usec)>(write_timeout_usec);
          setsockopt(sock2, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));
        }

        error = Error::Success;
        return true;
      });

  if (sock != INVALID_SOCKET) {
    error = Error::Success;
  } else {
    if (error == Error::Success) {
      error = Error::Connection;
    }
  }

  return sock;
}

inline bool get_remote_ip_and_port(
    const struct sockaddr_storage &addr, socklen_t addr_len, std::string &ip, int &port) {
  if (addr.ss_family == AF_INET) {
    port = ntohs(reinterpret_cast<const struct sockaddr_in *>(&addr)->sin_port);
  } else if (addr.ss_family == AF_INET6) {
    port = ntohs(reinterpret_cast<const struct sockaddr_in6 *>(&addr)->sin6_port);
  } else {
    return false;
  }

  std::array<char, NI_MAXHOST> ipstr{};
  if (getnameinfo(
          reinterpret_cast<const struct sockaddr *>(&addr),
          addr_len,
          ipstr.data(),
          static_cast<socklen_t>(ipstr.size()),
          nullptr,
          0,
          NI_NUMERICHOST)) {
    return false;
  }

  ip = ipstr.data();
  return true;
}

inline void get_remote_ip_and_port(socket_t sock, std::string &ip, int &port) {
  struct sockaddr_storage addr;
  socklen_t               addr_len = sizeof(addr);

  if (!getpeername(sock, reinterpret_cast<struct sockaddr *>(&addr), &addr_len)) {
    if (addr.ss_family == AF_UNIX) {
#if defined(__linux__)
      struct ucred ucred;
      socklen_t    len = sizeof(ucred);
      if (getsockopt(sock, SOL_SOCKET, SO_PEERCRED, &ucred, &len) == 0) {
        port = ucred.pid;
      }
#elif defined(SOL_LOCAL) && defined(SO_PEERPID)  // __APPLE__
      pid_t     pid;
      socklen_t len = sizeof(pid);
      if (getsockopt(sock, SOL_LOCAL, SO_PEERPID, &pid, &len) == 0) {
        port = pid;
      }
#endif
      return;
    }
    get_remote_ip_and_port(addr, addr_len, ip, port);
  }
}

inline constexpr unsigned int str2tag_core(const char *s, size_t l, unsigned int h) {
  return (l == 0) ? h : str2tag_core(s + 1, l - 1, (h * 33) ^ static_cast<unsigned char>(*s));
}

inline unsigned int str2tag(const std::string &s) {
  return str2tag_core(s.data(), s.size(), 0);
}

namespace udl {

inline constexpr unsigned int operator"" _t(const char *s, size_t l) {
  return str2tag_core(s, l, 0);
}

}  // namespace udl

inline const char *find_content_type(const std::string &path, const std::map<std::string, std::string> &user_data) {
  auto ext = file_extension(path);

  auto it = user_data.find(ext);
  if (it != user_data.end()) {
    return it->second.c_str();
  }

  using udl::operator""_t;

  switch (str2tag(ext)) {
    default:
      return nullptr;
    case "css"_t:
      return "text/css";
    case "csv"_t:
      return "text/csv";
    case "htm"_t:
    case "html"_t:
      return "text/html";
    case "js"_t:
    case "mjs"_t:
      return "text/javascript";
    case "txt"_t:
      return "text/plain";
    case "vtt"_t:
      return "text/vtt";

    case "apng"_t:
      return "image/apng";
    case "avif"_t:
      return "image/avif";
    case "bmp"_t:
      return "image/bmp";
    case "gif"_t:
      return "image/gif";
    case "png"_t:
      return "image/png";
    case "svg"_t:
      return "image/svg+xml";
    case "webp"_t:
      return "image/webp";
    case "ico"_t:
      return "image/x-icon";
    case "tif"_t:
      return "image/tiff";
    case "tiff"_t:
      return "image/tiff";
    case "jpg"_t:
    case "jpeg"_t:
      return "image/jpeg";

    case "mp4"_t:
      return "video/mp4";
    case "mpeg"_t:
      return "video/mpeg";
    case "webm"_t:
      return "video/webm";

    case "mp3"_t:
      return "audio/mp3";
    case "mpga"_t:
      return "audio/mpeg";
    case "weba"_t:
      return "audio/webm";
    case "wav"_t:
      return "audio/wave";

    case "otf"_t:
      return "font/otf";
    case "ttf"_t:
      return "font/ttf";
    case "woff"_t:
      return "font/woff";
    case "woff2"_t:
      return "font/woff2";

    case "7z"_t:
      return "application/x-7z-compressed";
    case "atom"_t:
      return "application/atom+xml";
    case "pdf"_t:
      return "application/pdf";
    case "json"_t:
      return "application/json";
    case "rss"_t:
      return "application/rss+xml";
    case "tar"_t:
      return "application/x-tar";
    case "xht"_t:
    case "xhtml"_t:
      return "application/xhtml+xml";
    case "xslt"_t:
      return "application/xslt+xml";
    case "xml"_t:
      return "application/xml";
    case "gz"_t:
      return "application/gzip";
    case "zip"_t:
      return "application/zip";
    case "wasm"_t:
      return "application/wasm";
  }
}

inline const char *status_message(int status) {
  switch (status) {
    case 100:
      return "Continue";
    case 101:
      return "Switching Protocol";
    case 102:
      return "Processing";
    case 103:
      return "Early Hints";
    case 200:
      return "OK";
    case 201:
      return "Created";
    case 202:
      return "Accepted";
    case 203:
      return "Non-Authoritative Information";
    case 204:
      return "No Content";
    case 205:
      return "Reset Content";
    case 206:
      return "Partial Content";
    case 207:
      return "Multi-Status";
    case 208:
      return "Already Reported";
    case 226:
      return "IM Used";
    case 300:
      return "Multiple Choice";
    case 301:
      return "Moved Permanently";
    case 302:
      return "Found";
    case 303:
      return "See Other";
    case 304:
      return "Not Modified";
    case 305:
      return "Use Proxy";
    case 306:
      return "unused";
    case 307:
      return "Temporary Redirect";
    case 308:
      return "Permanent Redirect";
    case 400:
      return "Bad Request";
    case 401:
      return "Unauthorized";
    case 402:
      return "Payment Required";
    case 403:
      return "Forbidden";
    case 404:
      return "Not Found";
    case 405:
      return "Method Not Allowed";
    case 406:
      return "Not Acceptable";
    case 407:
      return "Proxy Authentication Required";
    case 408:
      return "Request Timeout";
    case 409:
      return "Conflict";
    case 410:
      return "Gone";
    case 411:
      return "Length Required";
    case 412:
      return "Precondition Failed";
    case 413:
      return "Payload Too Large";
    case 414:
      return "URI Too Long";
    case 415:
      return "Unsupported Media Type";
    case 416:
      return "Range Not Satisfiable";
    case 417:
      return "Expectation Failed";
    case 418:
      return "I'm a teapot";
    case 421:
      return "Misdirected Request";
    case 422:
      return "Unprocessable Entity";
    case 423:
      return "Locked";
    case 424:
      return "Failed Dependency";
    case 425:
      return "Too Early";
    case 426:
      return "Upgrade Required";
    case 428:
      return "Precondition Required";
    case 429:
      return "Too Many Requests";
    case 431:
      return "Request Header Fields Too Large";
    case 451:
      return "Unavailable For Legal Reasons";
    case 501:
      return "Not Implemented";
    case 502:
      return "Bad Gateway";
    case 503:
      return "Service Unavailable";
    case 504:
      return "Gateway Timeout";
    case 505:
      return "HTTP Version Not Supported";
    case 506:
      return "Variant Also Negotiates";
    case 507:
      return "Insufficient Storage";
    case 508:
      return "Loop Detected";
    case 510:
      return "Not Extended";
    case 511:
      return "Network Authentication Required";

    default:
    case 500:
      return "Internal Server Error";
  }
}

inline bool can_compress_content_type(const std::string &content_type) {
  using udl::operator""_t;

  auto tag = str2tag(content_type);

  switch (tag) {
    case "image/svg+xml"_t:
    case "application/javascript"_t:
    case "application/json"_t:
    case "application/xml"_t:
    case "application/protobuf"_t:
    case "application/xhtml+xml"_t:
      return true;

    default:
      return !content_type.rfind("text/", 0) && tag != "text/event-stream"_t;
  }
}

inline EncodingType encoding_type(const Request &req, const Response &res) {
  auto ret = detail::can_compress_content_type(res.get_header_value("Content-Type"));
  if (!ret) {
    return EncodingType::None;
  }

  const auto &s = req.get_header_value("Accept-Encoding");
  (void)(s);

  return EncodingType::None;
}

inline bool nocompressor::compress(const char *data, size_t data_length, bool /*last*/, Callback callback) {
  if (!data_length) {
    return true;
  }
  return callback(data, data_length);
}

inline bool has_header(const Headers &headers, const std::string &key) {
  return headers.find(key) != headers.end();
}

inline const char *get_header_value(const Headers &headers, const std::string &key, size_t id, const char *def) {
  auto rng = headers.equal_range(key);
  auto it  = rng.first;
  std::advance(it, static_cast<ssize_t>(id));
  if (it != rng.second) {
    return it->second.c_str();
  }
  return def;
}

template <typename T>
inline bool parse_header(const char *beg, const char *end, T fn) {
  // Skip trailing spaces and tabs.
  while (beg < end && is_space_or_tab(end[-1])) {
    end--;
  }

  auto p = beg;
  while (p < end && *p != ':') {
    p++;
  }

  if (p == end) {
    return false;
  }

  auto key_end = p;

  if (*p++ != ':') {
    return false;
  }

  while (p < end && is_space_or_tab(*p)) {
    p++;
  }

  if (p < end) {
    fn(std::string(beg, key_end), decode_url(std::string(p, end), false));
    return true;
  }

  return false;
}

inline bool read_headers(Stream &strm, Headers &headers) {
  const auto         bufsiz = 2048;
  char               buf[bufsiz];
  stream_line_reader line_reader(strm, buf, bufsiz);

  for (;;) {
    if (!line_reader.getline()) {
      return false;
    }

    // Check if the line ends with CRLF.
    auto line_terminator_len = 2;
    if (line_reader.end_with_crlf()) {
      // Blank line indicates end of headers.
      if (line_reader.size() == 2) {
        break;
      }
#ifdef CPPHTTPLIB_ALLOW_LF_AS_LINE_TERMINATOR
    } else {
      // Blank line indicates end of headers.
      if (line_reader.size() == 1) {
        break;
      }
      line_terminator_len = 1;
    }
#else
    } else {
      continue;  // Skip invalid line.
    }
#endif

    if (line_reader.size() > CPPHTTPLIB_HEADER_MAX_LENGTH) {
      return false;
    }

    // Exclude line terminator
    auto end = line_reader.ptr() + line_reader.size() - line_terminator_len;

    parse_header(line_reader.ptr(), end, [&](std::string &&key, std::string &&val) {
      headers.emplace(std::move(key), std::move(val));
    });
  }

  return true;
}

inline bool read_content_with_length(Stream &strm, uint64_t len, Progress progress, ContentReceiverWithProgress out) {
  char buf[CPPHTTPLIB_RECV_BUFSIZ];

  uint64_t r = 0;
  while (r < len) {
    auto read_len = static_cast<size_t>(len - r);
    auto n        = strm.read(buf, (std::min)(read_len, CPPHTTPLIB_RECV_BUFSIZ));
    if (n <= 0) {
      return false;
    }

    if (!out(buf, static_cast<size_t>(n), r, len)) {
      return false;
    }
    r += static_cast<uint64_t>(n);

    if (progress) {
      if (!progress(r, len)) {
        return false;
      }
    }
  }

  return true;
}

inline void skip_content_with_length(Stream &strm, uint64_t len) {
  char     buf[CPPHTTPLIB_RECV_BUFSIZ];
  uint64_t r = 0;
  while (r < len) {
    auto read_len = static_cast<size_t>(len - r);
    auto n        = strm.read(buf, (std::min)(read_len, CPPHTTPLIB_RECV_BUFSIZ));
    if (n <= 0) {
      return;
    }
    r += static_cast<uint64_t>(n);
  }
}

inline bool read_content_without_length(Stream &strm, ContentReceiverWithProgress out) {
  char     buf[CPPHTTPLIB_RECV_BUFSIZ];
  uint64_t r = 0;
  for (;;) {
    auto n = strm.read(buf, CPPHTTPLIB_RECV_BUFSIZ);
    if (n < 0) {
      return false;
    } else if (n == 0) {
      return true;
    }

    if (!out(buf, static_cast<size_t>(n), r, 0)) {
      return false;
    }
    r += static_cast<uint64_t>(n);
  }

  return true;
}

inline bool read_content_chunked(Stream &strm, ContentReceiverWithProgress out) {
  const auto bufsiz = 16;
  char       buf[bufsiz];

  stream_line_reader line_reader(strm, buf, bufsiz);

  if (!line_reader.getline()) {
    return false;
  }

  unsigned long chunk_len;
  while (true) {
    char *end_ptr;

    chunk_len = std::strtoul(line_reader.ptr(), &end_ptr, 16);

    if (end_ptr == line_reader.ptr()) {
      return false;
    }
    if (chunk_len == ULONG_MAX) {
      return false;
    }

    if (chunk_len == 0) {
      break;
    }

    if (!read_content_with_length(strm, chunk_len, nullptr, out)) {
      return false;
    }

    if (!line_reader.getline()) {
      return false;
    }

    if (strcmp(line_reader.ptr(), "\r\n")) {
      break;
    }

    if (!line_reader.getline()) {
      return false;
    }
  }

  if (chunk_len == 0) {
    // Reader terminator after chunks
    if (!line_reader.getline() || strcmp(line_reader.ptr(), "\r\n")) return false;
  }

  return true;
}

inline bool is_chunked_transfer_encoding(const Headers &headers) {
  return !strcasecmp(get_header_value(headers, "Transfer-Encoding", 0, ""), "chunked");
}

template <typename T, typename U>
bool prepare_content_receiver(T &x, int &status, ContentReceiverWithProgress receiver, bool decompress, U callback) {
  if (decompress) {
    std::string                   encoding = x.get_header_value("Content-Encoding");
    std::unique_ptr<decompressor> decompressor;

    if (encoding == "gzip" || encoding == "deflate") {
      status = 415;
      return false;
    } else if (encoding.find("br") != std::string::npos) {
      status = 415;
      return false;
    }

    if (decompressor) {
      if (decompressor->is_valid()) {
        ContentReceiverWithProgress out = [&](const char *buf, size_t n, uint64_t off, uint64_t len) {
          return decompressor->decompress(
              buf, n, [&](const char *buf2, size_t n2) { return receiver(buf2, n2, off, len); });
        };
        return callback(std::move(out));
      } else {
        status = 500;
        return false;
      }
    }
  }

  ContentReceiverWithProgress out = [&](const char *buf, size_t n, uint64_t off, uint64_t len) {
    return receiver(buf, n, off, len);
  };
  return callback(std::move(out));
}

template <typename T>
bool read_content(
    Stream                     &strm,
    T                          &x,
    size_t                      payload_max_length,
    int                        &status,
    Progress                    progress,
    ContentReceiverWithProgress receiver,
    bool                        decompress) {
  return prepare_content_receiver(
      x, status, std::move(receiver), decompress, [&](const ContentReceiverWithProgress &out) {
        auto ret                       = true;
        auto exceed_payload_max_length = false;

        if (is_chunked_transfer_encoding(x.headers)) {
          ret = read_content_chunked(strm, out);
        } else if (!has_header(x.headers, "Content-Length")) {
          ret = read_content_without_length(strm, out);
        } else {
          auto len = get_header_value<uint64_t>(x.headers, "Content-Length");
          if (len > payload_max_length) {
            exceed_payload_max_length = true;
            skip_content_with_length(strm, len);
            ret = false;
          } else if (len > 0) {
            ret = read_content_with_length(strm, len, std::move(progress), out);
          }
        }

        if (!ret) {
          status = exceed_payload_max_length ? 413 : 400;
        }
        return ret;
      });
}  // namespace detail

inline ssize_t write_headers(Stream &strm, const Headers &headers) {
  ssize_t write_len = 0;
  for (const auto &x : headers) {
    auto len = strm.write_format("%s: %s\r\n", x.first.c_str(), x.second.c_str());
    if (len < 0) {
      return len;
    }
    write_len += len;
  }
  auto len = strm.write("\r\n");
  if (len < 0) {
    return len;
  }
  write_len += len;
  return write_len;
}

inline bool write_data(Stream &strm, const char *d, size_t l) {
  size_t offset = 0;
  while (offset < l) {
    auto length = strm.write(d + offset, l - offset);
    if (length < 0) {
      return false;
    }
    offset += static_cast<size_t>(length);
  }
  return true;
}

template <typename T>
inline bool write_content(
    Stream                &strm,
    const ContentProvider &content_provider,
    size_t                 offset,
    size_t                 length,
    T                      is_shutting_down,
    Error                 &error) {
  size_t   end_offset = offset + length;
  auto     ok         = true;
  DataSink data_sink;

  data_sink.write = [&](const char *d, size_t l) -> bool {
    if (ok) {
      if (write_data(strm, d, l)) {
        offset += l;
      } else {
        ok = false;
      }
    }
    return ok;
  };

  data_sink.is_writable = [&](void) { return ok && strm.is_writable(); };

  while (offset < end_offset && !is_shutting_down()) {
    if (!content_provider(offset, end_offset - offset, data_sink)) {
      error = Error::Canceled;
      return false;
    }
    if (!ok) {
      error = Error::Write;
      return false;
    }
  }

  error = Error::Success;
  return true;
}

template <typename T>
inline bool write_content(
    Stream &strm, const ContentProvider &content_provider, size_t offset, size_t length, const T &is_shutting_down) {
  auto error = Error::Success;
  return write_content(strm, content_provider, offset, length, is_shutting_down, error);
}

template <typename T>
inline bool write_content_without_length(
    Stream &strm, const ContentProvider &content_provider, const T &is_shutting_down) {
  size_t   offset         = 0;
  auto     data_available = true;
  auto     ok             = true;
  DataSink data_sink;

  data_sink.write = [&](const char *d, size_t l) -> bool {
    if (ok) {
      offset += l;
      if (!write_data(strm, d, l)) {
        ok = false;
      }
    }
    return ok;
  };

  data_sink.done = [&](void) { data_available = false; };

  data_sink.is_writable = [&](void) { return ok && strm.is_writable(); };

  while (data_available && !is_shutting_down()) {
    if (!content_provider(offset, 0, data_sink)) {
      return false;
    }
    if (!ok) {
      return false;
    }
  }
  return true;
}

template <typename T, typename U>
inline bool write_content_chunked(
    Stream &strm, const ContentProvider &content_provider, const T &is_shutting_down, U &compressor, Error &error) {
  size_t   offset         = 0;
  auto     data_available = true;
  auto     ok             = true;
  DataSink data_sink;

  data_sink.write = [&](const char *d, size_t l) -> bool {
    if (ok) {
      data_available = l > 0;
      offset += l;

      std::string payload;
      if (compressor.compress(d, l, false, [&](const char *data, size_t data_len) {
            payload.append(data, data_len);
            return true;
          })) {
        if (!payload.empty()) {
          // Emit chunked response header and footer for each chunk
          auto chunk = from_i_to_hex(payload.size()) + "\r\n" + payload + "\r\n";
          if (!write_data(strm, chunk.data(), chunk.size())) {
            ok = false;
          }
        }
      } else {
        ok = false;
      }
    }
    return ok;
  };

  data_sink.done = [&](void) {
    if (!ok) {
      return;
    }

    data_available = false;

    std::string payload;
    if (!compressor.compress(nullptr, 0, true, [&](const char *data, size_t data_len) {
          payload.append(data, data_len);
          return true;
        })) {
      ok = false;
      return;
    }

    if (!payload.empty()) {
      // Emit chunked response header and footer for each chunk
      auto chunk = from_i_to_hex(payload.size()) + "\r\n" + payload + "\r\n";
      if (!write_data(strm, chunk.data(), chunk.size())) {
        ok = false;
        return;
      }
    }

    static const std::string done_marker("0\r\n\r\n");
    if (!write_data(strm, done_marker.data(), done_marker.size())) {
      ok = false;
    }
  };

  data_sink.is_writable = [&](void) { return ok && strm.is_writable(); };

  while (data_available && !is_shutting_down()) {
    if (!content_provider(offset, 0, data_sink)) {
      error = Error::Canceled;
      return false;
    }
    if (!ok) {
      error = Error::Write;
      return false;
    }
  }

  error = Error::Success;
  return true;
}

template <typename T, typename U>
inline bool write_content_chunked(
    Stream &strm, const ContentProvider &content_provider, const T &is_shutting_down, U &compressor) {
  auto error = Error::Success;
  return write_content_chunked(strm, content_provider, is_shutting_down, compressor, error);
}

template <typename T>
inline bool redirect(
    T &cli, Request &req, Response &res, const std::string &path, const std::string &location, Error &error) {
  Request new_req = req;
  new_req.path    = path;
  new_req.redirect_count_ -= 1;

  if (res.status == 303 && (req.method != "GET" && req.method != "HEAD")) {
    new_req.method = "GET";
    new_req.body.clear();
    new_req.headers.clear();
  }

  Response new_res;

  auto ret = cli.send(new_req, new_res, error);
  if (ret) {
    req          = new_req;
    res          = new_res;
    res.location = location;
  }
  return ret;
}

inline std::string params_to_query_str(const Params &params) {
  std::string query;

  for (auto it = params.begin(); it != params.end(); ++it) {
    if (it != params.begin()) {
      query += "&";
    }
    query += it->first;
    query += "=";
    query += encode_query_param(it->second);
  }
  return query;
}

inline void parse_query_text(const std::string &s, Params &params) {
  std::set<std::string> cache;
  split(s.data(), s.data() + s.size(), '&', [&](const char *b, const char *e) {
    std::string kv(b, e);
    if (cache.find(kv) != cache.end()) {
      return;
    }
    cache.insert(kv);

    std::string key;
    std::string val;
    split(b, e, '=', [&](const char *b2, const char *e2) {
      if (key.empty()) {
        key.assign(b2, e2);
      } else {
        val.assign(b2, e2);
      }
    });

    if (!key.empty()) {
      params.emplace(decode_url(key, true), decode_url(val, true));
    }
  });
}

inline bool parse_multipart_boundary(const std::string &content_type, std::string &boundary) {
  auto pos = content_type.find("boundary=");
  if (pos == std::string::npos) {
    return false;
  }
  boundary = content_type.substr(pos + 9);
  if (boundary.length() >= 2 && boundary.front() == '"' && boundary.back() == '"') {
    boundary = boundary.substr(1, boundary.size() - 2);
  }
  return !boundary.empty();
}

#ifdef CPPHTTPLIB_NO_EXCEPTIONS
inline bool parse_range_header(const std::string &s, Ranges &ranges) {
#else
inline bool parse_range_header(const std::string &s, Ranges &ranges) try {
#endif
  static auto re_first_range = std::regex(R"(bytes=(\d*-\d*(?:,\s*\d*-\d*)*))");
  std::smatch m;
  if (std::regex_match(s, m, re_first_range)) {
    auto pos              = static_cast<size_t>(m.position(1));
    auto len              = static_cast<size_t>(m.length(1));
    bool all_valid_ranges = true;
    split(&s[pos], &s[pos + len], ',', [&](const char *b, const char *e) {
      if (!all_valid_ranges) return;
      static auto re_another_range = std::regex(R"(\s*(\d*)-(\d*))");
      std::cmatch cm;
      if (std::regex_match(b, e, cm, re_another_range)) {
        ssize_t first = -1;
        if (!cm.str(1).empty()) {
          first = static_cast<ssize_t>(std::stoll(cm.str(1)));
        }

        ssize_t last = -1;
        if (!cm.str(2).empty()) {
          last = static_cast<ssize_t>(std::stoll(cm.str(2)));
        }

        if (first != -1 && last != -1 && first > last) {
          all_valid_ranges = false;
          return;
        }
        ranges.emplace_back(std::make_pair(first, last));
      }
    });
    return all_valid_ranges;
  }
  return false;
#ifdef CPPHTTPLIB_NO_EXCEPTIONS
}
#else
} catch (...) {
  return false;
}
#endif

class MultipartFormDataParser {
public:
  MultipartFormDataParser() = default;

  void set_boundary(std::string &&boundary) {
    boundary_           = boundary;
    dash_boundary_crlf_ = dash_ + boundary_ + crlf_;
    crlf_dash_boundary_ = crlf_ + dash_ + boundary_;
  }

  bool is_valid() const {
    return is_valid_;
  }

  bool parse(
      const char                   *buf,
      size_t                        n,
      const ContentReceiver        &content_callback,
      const MultipartContentHeader &header_callback) {
    // TODO: support 'filename*'
    static const std::regex re_content_disposition(
        R"~(^Content-Disposition:\s*form-data;\s*name="(.*?)"(?:;\s*filename="(.*?)")?(?:;\s*filename\*=\S+)?\s*$)~",
        std::regex_constants::icase);

    buf_append(buf, n);

    while (buf_size() > 0) {
      switch (state_) {
        case 0: {  // Initial boundary
          buf_erase(buf_find(dash_boundary_crlf_));
          if (dash_boundary_crlf_.size() > buf_size()) {
            return true;
          }
          if (!buf_start_with(dash_boundary_crlf_)) {
            return false;
          }
          buf_erase(dash_boundary_crlf_.size());
          state_ = 1;
          break;
        }
        case 1: {  // New entry
          clear_file_info();
          state_ = 2;
          break;
        }
        case 2: {  // Headers
          auto pos = buf_find(crlf_);
          if (pos > CPPHTTPLIB_HEADER_MAX_LENGTH) {
            return false;
          }
          while (pos < buf_size()) {
            // Empty line
            if (pos == 0) {
              if (!header_callback(file_)) {
                is_valid_ = false;
                return false;
              }
              buf_erase(crlf_.size());
              state_ = 3;
              break;
            }

            static const std::string header_name = "content-type:";
            const auto               header      = buf_head(pos);
            if (start_with_case_ignore(header, header_name)) {
              file_.content_type = trim_copy(header.substr(header_name.size()));
            } else {
              std::smatch m;
              if (std::regex_match(header, m, re_content_disposition)) {
                file_.name     = m[1];
                file_.filename = m[2];
              }
            }
            buf_erase(pos + crlf_.size());
            pos = buf_find(crlf_);
          }
          if (state_ != 3) {
            return true;
          }
          break;
        }
        case 3: {  // Body
          if (crlf_dash_boundary_.size() > buf_size()) {
            return true;
          }
          auto pos = buf_find(crlf_dash_boundary_);
          if (pos < buf_size()) {
            if (!content_callback(buf_data(), pos)) {
              is_valid_ = false;
              return false;
            }
            buf_erase(pos + crlf_dash_boundary_.size());
            state_ = 4;
          } else {
            auto len = buf_size() - crlf_dash_boundary_.size();
            if (len > 0) {
              if (!content_callback(buf_data(), len)) {
                is_valid_ = false;
                return false;
              }
              buf_erase(len);
            }
            return true;
          }
          break;
        }
        case 4: {  // Boundary
          if (crlf_.size() > buf_size()) {
            return true;
          }
          if (buf_start_with(crlf_)) {
            buf_erase(crlf_.size());
            state_ = 1;
          } else {
            if (dash_crlf_.size() > buf_size()) {
              return true;
            }
            if (buf_start_with(dash_crlf_)) {
              buf_erase(dash_crlf_.size());
              is_valid_ = true;
              buf_erase(buf_size());  // Remove epilogue
            } else {
              return true;
            }
          }
          break;
        }
      }
    }

    return true;
  }

private:
  void clear_file_info() {
    file_.name.clear();
    file_.filename.clear();
    file_.content_type.clear();
  }

  bool start_with_case_ignore(const std::string &a, const std::string &b) const {
    if (a.size() < b.size()) {
      return false;
    }
    for (size_t i = 0; i < b.size(); i++) {
      if (::tolower(a[i]) != ::tolower(b[i])) {
        return false;
      }
    }
    return true;
  }

  const std::string dash_      = "--";
  const std::string crlf_      = "\r\n";
  const std::string dash_crlf_ = "--\r\n";
  std::string       boundary_;
  std::string       dash_boundary_crlf_;
  std::string       crlf_dash_boundary_;

  size_t            state_    = 0;
  bool              is_valid_ = false;
  MultipartFormData file_;

  // Buffer
  bool start_with(const std::string &a, size_t spos, size_t epos, const std::string &b) const {
    if (epos - spos < b.size()) {
      return false;
    }
    for (size_t i = 0; i < b.size(); i++) {
      if (a[i + spos] != b[i]) {
        return false;
      }
    }
    return true;
  }

  size_t buf_size() const {
    return buf_epos_ - buf_spos_;
  }

  const char *buf_data() const {
    return &buf_[buf_spos_];
  }

  std::string buf_head(size_t l) const {
    return buf_.substr(buf_spos_, l);
  }

  bool buf_start_with(const std::string &s) const {
    return start_with(buf_, buf_spos_, buf_epos_, s);
  }

  size_t buf_find(const std::string &s) const {
    auto c = s.front();

    size_t off = buf_spos_;
    while (off < buf_epos_) {
      auto pos = off;
      while (true) {
        if (pos == buf_epos_) {
          return buf_size();
        }
        if (buf_[pos] == c) {
          break;
        }
        pos++;
      }

      auto remaining_size = buf_epos_ - pos;
      if (s.size() > remaining_size) {
        return buf_size();
      }

      if (start_with(buf_, pos, buf_epos_, s)) {
        return pos - buf_spos_;
      }

      off = pos + 1;
    }

    return buf_size();
  }

  void buf_append(const char *data, size_t n) {
    auto remaining_size = buf_size();
    if (remaining_size > 0 && buf_spos_ > 0) {
      for (size_t i = 0; i < remaining_size; i++) {
        buf_[i] = buf_[buf_spos_ + i];
      }
    }
    buf_spos_ = 0;
    buf_epos_ = remaining_size;

    if (remaining_size + n > buf_.size()) {
      buf_.resize(remaining_size + n);
    }

    for (size_t i = 0; i < n; i++) {
      buf_[buf_epos_ + i] = data[i];
    }
    buf_epos_ += n;
  }

  void buf_erase(size_t size) {
    buf_spos_ += size;
  }

  std::string buf_;
  size_t      buf_spos_ = 0;
  size_t      buf_epos_ = 0;
};

inline std::string to_lower(const char *beg, const char *end) {
  std::string out;
  auto        it = beg;
  while (it != end) {
    out += static_cast<char>(::tolower(*it));
    it++;
  }
  return out;
}

inline std::string make_multipart_data_boundary() {
  static const char data[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  // std::random_device might actually be deterministic on some
  // platforms, but due to lack of support in the c++ standard library,
  // doing better requires either some ugly hacks or breaking portability.
  std::random_device seed_gen;

  // Request 128 bits of entropy for initialization
  std::seed_seq seed_sequence{seed_gen(), seed_gen(), seed_gen(), seed_gen()};
  std::mt19937  engine(seed_sequence);

  std::string result = "--cpp-httplib-multipart-data-";

  for (auto i = 0; i < 16; i++) {
    result += data[engine() % (sizeof(data) - 1)];
  }

  return result;
}

inline bool is_multipart_boundary_chars_valid(const std::string &boundary) {
  auto valid = true;
  for (size_t i = 0; i < boundary.size(); i++) {
    auto c = boundary[i];
    if (!std::isalnum(c) && c != '-' && c != '_') {
      valid = false;
      break;
    }
  }
  return valid;
}

inline std::string serialize_multipart_formdata(
    const MultipartFormDataItems &items, const std::string &boundary, std::string &content_type) {
  std::string body;

  for (const auto &item : items) {
    body += "--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"" + item.name + "\"";
    if (!item.filename.empty()) {
      body += "; filename=\"" + item.filename + "\"";
    }
    body += "\r\n";
    if (!item.content_type.empty()) {
      body += "Content-Type: " + item.content_type + "\r\n";
    }
    body += "\r\n";
    body += item.content + "\r\n";
  }

  body += "--" + boundary + "--\r\n";

  content_type = "multipart/form-data; boundary=" + boundary;
  return body;
}

inline std::pair<size_t, size_t> get_range_offset_and_length(const Request &req, size_t content_length, size_t index) {
  auto r = req.ranges[index];

  if (r.first == -1 && r.second == -1) {
    return std::make_pair(0, content_length);
  }

  auto slen = static_cast<ssize_t>(content_length);

  if (r.first == -1) {
    r.first  = (std::max)(static_cast<ssize_t>(0), slen - r.second);
    r.second = slen - 1;
  }

  if (r.second == -1) {
    r.second = slen - 1;
  }
  return std::make_pair(r.first, static_cast<size_t>(r.second - r.first) + 1);
}

inline std::string make_content_range_header_field(size_t offset, size_t length, size_t content_length) {
  std::string field = "bytes ";
  field += std::to_string(offset);
  field += "-";
  field += std::to_string(offset + length - 1);
  field += "/";
  field += std::to_string(content_length);
  return field;
}

template <typename SToken, typename CToken, typename Content>
bool process_multipart_ranges_data(
    const Request     &req,
    Response          &res,
    const std::string &boundary,
    const std::string &content_type,
    SToken             stoken,
    CToken             ctoken,
    Content            content) {
  for (size_t i = 0; i < req.ranges.size(); i++) {
    ctoken("--");
    stoken(boundary);
    ctoken("\r\n");
    if (!content_type.empty()) {
      ctoken("Content-Type: ");
      stoken(content_type);
      ctoken("\r\n");
    }

    auto offsets = get_range_offset_and_length(req, res.body.size(), i);
    auto offset  = offsets.first;
    auto length  = offsets.second;

    ctoken("Content-Range: ");
    stoken(make_content_range_header_field(offset, length, res.body.size()));
    ctoken("\r\n");
    ctoken("\r\n");
    if (!content(offset, length)) {
      return false;
    }
    ctoken("\r\n");
  }

  ctoken("--");
  stoken(boundary);
  ctoken("--\r\n");

  return true;
}

inline bool make_multipart_ranges_data(
    const Request     &req,
    Response          &res,
    const std::string &boundary,
    const std::string &content_type,
    std::string       &data) {
  return process_multipart_ranges_data(
      req,
      res,
      boundary,
      content_type,
      [&](const std::string &token) { data += token; },
      [&](const std::string &token) { data += token; },
      [&](size_t offset, size_t length) {
        if (offset < res.body.size()) {
          data += res.body.substr(offset, length);
          return true;
        }
        return false;
      });
}

inline size_t get_multipart_ranges_data_length(
    const Request &req, Response &res, const std::string &boundary, const std::string &content_type) {
  size_t data_length = 0;

  process_multipart_ranges_data(
      req,
      res,
      boundary,
      content_type,
      [&](const std::string &token) { data_length += token.size(); },
      [&](const std::string &token) { data_length += token.size(); },
      [&](size_t /*offset*/, size_t length) {
        data_length += length;
        return true;
      });

  return data_length;
}

template <typename T>
inline bool write_multipart_ranges_data(
    Stream            &strm,
    const Request     &req,
    Response          &res,
    const std::string &boundary,
    const std::string &content_type,
    const T           &is_shutting_down) {
  return process_multipart_ranges_data(
      req,
      res,
      boundary,
      content_type,
      [&](const std::string &token) { strm.write(token); },
      [&](const std::string &token) { strm.write(token); },
      [&](size_t offset, size_t length) {
        return write_content(strm, res.content_provider_, offset, length, is_shutting_down);
      });
}

inline std::pair<size_t, size_t> get_range_offset_and_length(const Request &req, const Response &res, size_t index) {
  auto r = req.ranges[index];

  if (r.second == -1) {
    r.second = static_cast<ssize_t>(res.content_length_) - 1;
  }

  return std::make_pair(r.first, r.second - r.first + 1);
}

inline bool expect_content(const Request &req) {
  if (req.method == "POST" || req.method == "PUT" || req.method == "PATCH" || req.method == "PRI" ||
      req.method == "DELETE") {
    return true;
  }
  return false;
}

inline bool has_crlf(const std::string &s) {
  auto p = s.c_str();
  while (*p) {
    if (*p == '\r' || *p == '\n') {
      return true;
    }
    p++;
  }
  return false;
}

inline bool parse_www_authenticate(const Response &res, std::map<std::string, std::string> &auth, bool is_proxy) {
  auto auth_key = is_proxy ? "Proxy-Authenticate" : "WWW-Authenticate";
  if (res.has_header(auth_key)) {
    static auto re  = std::regex(R"~((?:(?:,\s*)?(.+?)=(?:"(.*?)"|([^,]*))))~");
    auto        s   = res.get_header_value(auth_key);
    auto        pos = s.find(' ');
    if (pos != std::string::npos) {
      auto type = s.substr(0, pos);
      if (type == "Basic") {
        return false;
      } else if (type == "Digest") {
        s        = s.substr(pos + 1);
        auto beg = std::sregex_iterator(s.begin(), s.end(), re);
        for (auto i = beg; i != std::sregex_iterator(); ++i) {
          auto m    = *i;
          auto key  = s.substr(static_cast<size_t>(m.position(1)), static_cast<size_t>(m.length(1)));
          auto val  = m.length(2) > 0 ? s.substr(static_cast<size_t>(m.position(2)), static_cast<size_t>(m.length(2)))
                                      : s.substr(static_cast<size_t>(m.position(3)), static_cast<size_t>(m.length(3)));
          auth[key] = val;
        }
        return true;
      }
    }
  }
  return false;
}

// https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c/440240#answer-440240
inline std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[static_cast<size_t>(std::rand()) % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

class ContentProviderAdapter {
public:
  explicit ContentProviderAdapter(ContentProviderWithoutLength &&content_provider)
    : content_provider_(content_provider) {
  }

  bool operator()(size_t offset, size_t, DataSink &sink) {
    return content_provider_(offset, sink);
  }

private:
  ContentProviderWithoutLength content_provider_;
};

}  // namespace detail

inline std::string hosted_at(const std::string &hostname) {
  std::vector<std::string> addrs;
  hosted_at(hostname, addrs);
  if (addrs.empty()) {
    return std::string();
  }
  return addrs[0];
}

inline void hosted_at(const std::string &hostname, std::vector<std::string> &addrs) {
  struct addrinfo  hints;
  struct addrinfo *result;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  if (getaddrinfo(hostname.c_str(), nullptr, &hints, &result)) {
#if defined __linux__ && !defined __ANDROID__
    res_init();
#endif
    return;
  }

  for (auto rp = result; rp; rp = rp->ai_next) {
    const auto &addr = *reinterpret_cast<struct sockaddr_storage *>(rp->ai_addr);
    std::string ip;
    int         dummy = -1;
    if (detail::get_remote_ip_and_port(addr, sizeof(struct sockaddr_storage), ip, dummy)) {
      addrs.push_back(ip);
    }
  }

  freeaddrinfo(result);
}

inline std::string append_query_params(const std::string &path, const Params &params) {
  std::string             path_with_query = path;
  const static std::regex re("[^?]+\\?.*");
  auto                    delm = std::regex_match(path, re) ? '&' : '?';
  path_with_query += delm + detail::params_to_query_str(params);
  return path_with_query;
}

// Header utilities
inline std::pair<std::string, std::string> make_range_header(Ranges ranges) {
  std::string field = "bytes=";
  auto        i     = 0;
  for (auto r : ranges) {
    if (i != 0) {
      field += ", ";
    }
    if (r.first != -1) {
      field += std::to_string(r.first);
    }
    field += '-';
    if (r.second != -1) {
      field += std::to_string(r.second);
    }
    i++;
  }
  return std::make_pair("Range", std::move(field));
}

inline std::pair<std::string, std::string> make_basic_authentication_header(
    const std::string &username, const std::string &password, bool is_proxy) {
  auto field = "Basic " + detail::base64_encode(username + ":" + password);
  auto key   = is_proxy ? "Proxy-Authorization" : "Authorization";
  return std::make_pair(key, std::move(field));
}

inline std::pair<std::string, std::string> make_bearer_token_authentication_header(
    const std::string &token, bool is_proxy = false) {
  auto field = "Bearer " + token;
  auto key   = is_proxy ? "Proxy-Authorization" : "Authorization";
  return std::make_pair(key, std::move(field));
}

// Request implementation
inline bool Request::has_header(const std::string &key) const {
  return detail::has_header(headers, key);
}

inline std::string Request::get_header_value(const std::string &key, size_t id) const {
  return detail::get_header_value(headers, key, id, "");
}

inline size_t Request::get_header_value_count(const std::string &key) const {
  auto r = headers.equal_range(key);
  return static_cast<size_t>(std::distance(r.first, r.second));
}

inline void Request::set_header(const std::string &key, const std::string &val) {
  if (!detail::has_crlf(key) && !detail::has_crlf(val)) {
    headers.emplace(key, val);
  }
}

inline bool Request::has_param(const std::string &key) const {
  return params.find(key) != params.end();
}

inline std::string Request::get_param_value(const std::string &key, size_t id) const {
  auto rng = params.equal_range(key);
  auto it  = rng.first;
  std::advance(it, static_cast<ssize_t>(id));
  if (it != rng.second) {
    return it->second;
  }
  return std::string();
}

inline size_t Request::get_param_value_count(const std::string &key) const {
  auto r = params.equal_range(key);
  return static_cast<size_t>(std::distance(r.first, r.second));
}

inline bool Request::is_multipart_form_data() const {
  const auto &content_type = get_header_value("Content-Type");
  return !content_type.rfind("multipart/form-data", 0);
}

inline bool Request::has_file(const std::string &key) const {
  return files.find(key) != files.end();
}

inline MultipartFormData Request::get_file_value(const std::string &key) const {
  auto it = files.find(key);
  if (it != files.end()) {
    return it->second;
  }
  return MultipartFormData();
}

// Response implementation
inline bool Response::has_header(const std::string &key) const {
  return headers.find(key) != headers.end();
}

inline std::string Response::get_header_value(const std::string &key, size_t id) const {
  return detail::get_header_value(headers, key, id, "");
}

inline size_t Response::get_header_value_count(const std::string &key) const {
  auto r = headers.equal_range(key);
  return static_cast<size_t>(std::distance(r.first, r.second));
}

inline void Response::set_header(const std::string &key, const std::string &val) {
  if (!detail::has_crlf(key) && !detail::has_crlf(val)) {
    headers.emplace(key, val);
  }
}

inline void Response::set_redirect(const std::string &url, int stat) {
  if (!detail::has_crlf(url)) {
    set_header("Location", url);
    if (300 <= stat && stat < 400) {
      this->status = stat;
    } else {
      this->status = 302;
    }
  }
}

inline void Response::set_content(const char *s, size_t n, const std::string &content_type) {
  body.assign(s, n);

  auto rng = headers.equal_range("Content-Type");
  headers.erase(rng.first, rng.second);
  set_header("Content-Type", content_type);
}

inline void Response::set_content(const std::string &s, const std::string &content_type) {
  set_content(s.data(), s.size(), content_type);
}

inline void Response::set_content_provider(
    size_t                          in_length,
    const std::string              &content_type,
    ContentProvider                 provider,
    ContentProviderResourceReleaser resource_releaser) {
  assert(in_length > 0);
  set_header("Content-Type", content_type);
  content_length_                     = in_length;
  content_provider_                   = std::move(provider);
  content_provider_resource_releaser_ = resource_releaser;
  is_chunked_content_provider_        = false;
}

inline void Response::set_content_provider(
    const std::string              &content_type,
    ContentProviderWithoutLength    provider,
    ContentProviderResourceReleaser resource_releaser) {
  set_header("Content-Type", content_type);
  content_length_                     = 0;
  content_provider_                   = detail::ContentProviderAdapter(std::move(provider));
  content_provider_resource_releaser_ = resource_releaser;
  is_chunked_content_provider_        = false;
}

inline void Response::set_chunked_content_provider(
    const std::string              &content_type,
    ContentProviderWithoutLength    provider,
    ContentProviderResourceReleaser resource_releaser) {
  set_header("Content-Type", content_type);
  content_length_                     = 0;
  content_provider_                   = detail::ContentProviderAdapter(std::move(provider));
  content_provider_resource_releaser_ = resource_releaser;
  is_chunked_content_provider_        = true;
}

// Result implementation
inline bool Result::has_request_header(const std::string &key) const {
  return request_headers_.find(key) != request_headers_.end();
}

inline std::string Result::get_request_header_value(const std::string &key, size_t id) const {
  return detail::get_header_value(request_headers_, key, id, "");
}

inline size_t Result::get_request_header_value_count(const std::string &key) const {
  auto r = request_headers_.equal_range(key);
  return static_cast<size_t>(std::distance(r.first, r.second));
}

// Stream implementation
inline ssize_t Stream::write(const char *ptr) {
  return write(ptr, strlen(ptr));
}

inline ssize_t Stream::write(const std::string &s) {
  return write(s.data(), s.size());
}

namespace detail {

// Socket stream implementation
inline SocketStream::SocketStream(
    socket_t sock,
    time_t   read_timeout_sec,
    time_t   read_timeout_usec,
    time_t   write_timeout_sec,
    time_t   write_timeout_usec)
  : sock_(sock)
  , read_timeout_sec_(read_timeout_sec)
  , read_timeout_usec_(read_timeout_usec)
  , write_timeout_sec_(write_timeout_sec)
  , write_timeout_usec_(write_timeout_usec)
  , read_buff_(read_buff_size_, 0) {
}

inline SocketStream::~SocketStream() {
}

inline bool SocketStream::is_readable() const {
  return select_read(sock_, read_timeout_sec_, read_timeout_usec_) > 0;
}

inline bool SocketStream::is_writable() const {
  return select_write(sock_, write_timeout_sec_, write_timeout_usec_) > 0 && is_socket_alive(sock_);
}

inline ssize_t SocketStream::read(char *ptr, size_t size) {
  size = (std::min)(size, static_cast<size_t>((std::numeric_limits<ssize_t>::max)()));

  if (read_buff_off_ < read_buff_content_size_) {
    auto remaining_size = read_buff_content_size_ - read_buff_off_;
    if (size <= remaining_size) {
      memcpy(ptr, read_buff_.data() + read_buff_off_, size);
      read_buff_off_ += size;
      return static_cast<ssize_t>(size);
    } else {
      memcpy(ptr, read_buff_.data() + read_buff_off_, remaining_size);
      read_buff_off_ += remaining_size;
      return static_cast<ssize_t>(remaining_size);
    }
  }

  if (!is_readable()) {
    return -1;
  }

  read_buff_off_          = 0;
  read_buff_content_size_ = 0;

  if (size < read_buff_size_) {
    auto n = read_socket(sock_, read_buff_.data(), read_buff_size_, CPPHTTPLIB_RECV_FLAGS);
    if (n <= 0) {
      return n;
    } else if (n <= static_cast<ssize_t>(size)) {
      memcpy(ptr, read_buff_.data(), static_cast<size_t>(n));
      return n;
    } else {
      memcpy(ptr, read_buff_.data(), size);
      read_buff_off_          = size;
      read_buff_content_size_ = static_cast<size_t>(n);
      return static_cast<ssize_t>(size);
    }
  } else {
    return read_socket(sock_, ptr, size, CPPHTTPLIB_RECV_FLAGS);
  }
}

inline ssize_t SocketStream::write(const char *ptr, size_t size) {
  if (!is_writable()) {
    return -1;
  }

  return send_socket(sock_, ptr, size, CPPHTTPLIB_SEND_FLAGS);
}

inline void SocketStream::get_remote_ip_and_port(std::string &ip, int &port) const {
  return detail::get_remote_ip_and_port(sock_, ip, port);
}

inline socket_t SocketStream::socket() const {
  return sock_;
}

// Buffer stream implementation
inline bool BufferStream::is_readable() const {
  return true;
}

inline bool BufferStream::is_writable() const {
  return true;
}

inline ssize_t BufferStream::read(char *ptr, size_t size) {
#if defined(_MSC_VER) && _MSC_VER < 1910
  auto len_read = buffer._Copy_s(ptr, size, size, position);
#else
  auto len_read = buffer.copy(ptr, size, position);
#endif
  position += static_cast<size_t>(len_read);
  return static_cast<ssize_t>(len_read);
}

inline ssize_t BufferStream::write(const char *ptr, size_t size) {
  buffer.append(ptr, size);
  return static_cast<ssize_t>(size);
}

inline void BufferStream::get_remote_ip_and_port(std::string & /*ip*/, int & /*port*/) const {
}

inline socket_t BufferStream::socket() const {
  return 0;
}

inline const std::string &BufferStream::get_buffer() const {
  return buffer;
}

}  // namespace detail

// HTTP server implementation
inline Server::Server()
  : svr_sock_(INVALID_SOCKET)
  , is_running_(false) {
  signal(SIGPIPE, SIG_IGN);
}

inline Server::~Server() {
}

inline bool Server::set_base_dir(const std::string &dir, const std::string &mount_point) {
  return set_mount_point(mount_point, dir);
}

inline bool Server::set_mount_point(const std::string &mount_point, const std::string &dir, Headers headers) {
  if (detail::is_dir(dir)) {
    std::string mnt = !mount_point.empty() ? mount_point : "/";
    if (!mnt.empty() && mnt[0] == '/') {
      base_dirs_.push_back({mnt, dir, std::move(headers)});
      return true;
    }
  }
  return false;
}

inline bool Server::remove_mount_point(const std::string &mount_point) {
  for (auto it = base_dirs_.begin(); it != base_dirs_.end(); ++it) {
    if (it->mount_point == mount_point) {
      base_dirs_.erase(it);
      return true;
    }
  }
  return false;
}

inline Server &Server::set_file_extension_and_mimetype_mapping(const std::string &ext, const std::string &mime) {
  file_extension_and_mimetype_map_[ext] = mime;
  return *this;
}

inline Server &Server::set_file_request_handler(Handler handler) {
  file_request_handler_ = std::move(handler);
  return *this;
}

inline Server &Server::set_error_handler(HandlerWithResponse handler) {
  error_handler_ = std::move(handler);
  return *this;
}

inline Server &Server::set_error_handler(Handler handler) {
  error_handler_ = [handler](const Request &req, Response &res) {
    handler(req, res);
    return HandlerResponse::Handled;
  };
  return *this;
}

inline Server &Server::set_exception_handler(ExceptionHandler handler) {
  exception_handler_ = std::move(handler);
  return *this;
}

inline Server &Server::set_pre_routing_handler(HandlerWithResponse handler) {
  pre_routing_handler_ = std::move(handler);
  return *this;
}

inline Server &Server::set_post_routing_handler(Handler handler) {
  post_routing_handler_ = std::move(handler);
  return *this;
}

inline Server &Server::set_expect_100_continue_handler(Expect100ContinueHandler handler) {
  expect_100_continue_handler_ = std::move(handler);

  return *this;
}

inline Server &Server::set_address_family(int family) {
  address_family_ = family;
  return *this;
}

inline Server &Server::set_tcp_nodelay(bool on) {
  tcp_nodelay_ = on;
  return *this;
}

inline Server &Server::set_socket_options(SocketOptions socket_options) {
  socket_options_ = std::move(socket_options);
  return *this;
}

inline Server &Server::set_default_headers(Headers headers) {
  default_headers_ = std::move(headers);
  return *this;
}

inline Server &Server::set_keep_alive_max_count(size_t count) {
  keep_alive_max_count_ = count;
  return *this;
}

inline Server &Server::set_keep_alive_timeout(time_t sec) {
  keep_alive_timeout_sec_ = sec;
  return *this;
}

inline Server &Server::set_read_timeout(time_t sec, time_t usec) {
  read_timeout_sec_  = sec;
  read_timeout_usec_ = usec;
  return *this;
}

inline Server &Server::set_write_timeout(time_t sec, time_t usec) {
  write_timeout_sec_  = sec;
  write_timeout_usec_ = usec;
  return *this;
}

inline Server &Server::set_idle_interval(time_t sec, time_t usec) {
  idle_interval_sec_  = sec;
  idle_interval_usec_ = usec;
  return *this;
}

inline Server &Server::set_payload_max_length(size_t length) {
  payload_max_length_ = length;
  return *this;
}

inline bool Server::bind_to_port(const std::string &host, int port, int socket_flags) {
  if (bind_internal(host, port, socket_flags) < 0) return false;
  return true;
}
inline int Server::bind_to_any_port(const std::string &host, int socket_flags) {
  return bind_internal(host, 0, socket_flags);
}

inline bool Server::listen_after_bind() {
  return listen_internal();
}

inline bool Server::listen(const std::string &host, int port, int socket_flags) {
  return bind_to_port(host, port, socket_flags) && listen_internal();
}

inline bool Server::is_running() const {
  return is_running_;
}

inline void Server::stop() {
  if (is_running_) {
    assert(svr_sock_ != INVALID_SOCKET);
    std::atomic<socket_t> sock(svr_sock_.exchange(INVALID_SOCKET));
    detail::shutdown_socket(sock);
    detail::close_socket(sock);
  }
}

inline bool Server::parse_request_line(const char *s, Request &req) {
  auto len = strlen(s);
  if (len < 2 || s[len - 2] != '\r' || s[len - 1] != '\n') {
    return false;
  }
  len -= 2;

  {
    size_t count = 0;

    detail::split(s, s + len, ' ', [&](const char *b, const char *e) {
      switch (count) {
        case 0:
          req.method = std::string(b, e);
          break;
        case 1:
          req.target = std::string(b, e);
          break;
        case 2:
          req.version = std::string(b, e);
          break;
        default:
          break;
      }
      count++;
    });

    if (count != 3) {
      return false;
    }
  }

  static const std::set<std::string> methods{
      "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH", "PRI"};

  if (methods.find(req.method) == methods.end()) {
    return false;
  }

  if (req.version != "HTTP/1.1" && req.version != "HTTP/1.0") {
    return false;
  }

  {
    // Skip URL fragment
    for (size_t i = 0; i < req.target.size(); i++) {
      if (req.target[i] == '#') {
        req.target.erase(i);
        break;
      }
    }

    size_t count = 0;

    detail::split(req.target.data(), req.target.data() + req.target.size(), '?', [&](const char *b, const char *e) {
      switch (count) {
        case 0:
          req.path = detail::decode_url(std::string(b, e), false);
          break;
        case 1: {
          if (e - b > 0) {
            detail::parse_query_text(std::string(b, e), req.params);
          }
          break;
        }
        default:
          break;
      }
      count++;
    });

    if (count > 2) {
      return false;
    }
  }

  return true;
}

inline bool Server::write_response(Stream &strm, bool close_connection, const Request &req, Response &res) {
  return write_response_core(strm, close_connection, req, res, false);
}

inline bool Server::write_response_with_content(
    Stream &strm, bool close_connection, const Request &req, Response &res) {
  return write_response_core(strm, close_connection, req, res, true);
}

inline bool Server::write_response_core(
    Stream &strm, bool close_connection, const Request &req, Response &res, bool need_apply_ranges) {
  assert(res.status != -1);

  std::string content_type;
  std::string boundary;

  // Prepare additional headers
  if (close_connection || req.get_header_value("Connection") == "close") {
    res.set_header("Connection", "close");
  } else {
    std::stringstream ss;
    ss << "timeout=" << keep_alive_timeout_sec_ << ", max=" << keep_alive_max_count_;
    res.set_header("Keep-Alive", ss.str());
  }

  if (!res.has_header("Content-Type") && (!res.body.empty() || res.content_length_ > 0 || res.content_provider_)) {
    res.set_header("Content-Type", "text/plain");
  }

  if (!res.has_header("Content-Length") && res.body.empty() && !res.content_length_ && !res.content_provider_) {
    res.set_header("Content-Length", "0");
  }

  if (!res.has_header("Accept-Ranges") && req.method == "HEAD") {
    res.set_header("Accept-Ranges", "bytes");
  }

  if (post_routing_handler_) {
    post_routing_handler_(req, res);
  }

  // Response line and headers
  {
    detail::BufferStream bstrm;

    if (!bstrm.write_format("HTTP/1.1 %d %s\r\n", res.status, detail::status_message(res.status))) {
      return false;
    }

    if (!detail::write_headers(bstrm, res.headers)) {
      return false;
    }

    // Flush buffer
    auto &data = bstrm.get_buffer();
    detail::write_data(strm, data.data(), data.size());
  }

  // Body
  auto ret = true;
  if (req.method != "HEAD") {
    if (!res.body.empty()) {
      if (!detail::write_data(strm, res.body.data(), res.body.size())) {
        ret = false;
      }
    } else if (res.content_provider_) {
      if (write_content_with_provider(strm, req, res, boundary, content_type)) {
        res.content_provider_success_ = true;
      } else {
        res.content_provider_success_ = false;
        ret                           = false;
      }
    }
  }

  return ret;
}

inline bool Server::write_content_with_provider(
    Stream &strm, const Request &req, Response &res, const std::string &boundary, const std::string &content_type) {
  auto is_shutting_down = [this]() { return this->svr_sock_ == INVALID_SOCKET; };

  if (res.content_length_ > 0) {
    if (req.ranges.empty()) {
      return detail::write_content(strm, res.content_provider_, 0, res.content_length_, is_shutting_down);
    } else if (req.ranges.size() == 1) {
      auto offsets = detail::get_range_offset_and_length(req, res.content_length_, 0);
      auto offset  = offsets.first;
      auto length  = offsets.second;
      return detail::write_content(strm, res.content_provider_, offset, length, is_shutting_down);
    } else {
      return detail::write_multipart_ranges_data(strm, req, res, boundary, content_type, is_shutting_down);
    }
  } else {
    if (res.is_chunked_content_provider_) {
      auto type = detail::encoding_type(req, res);

      std::unique_ptr<detail::compressor> compressor;
      compressor = detail::make_unique<detail::nocompressor>();
      assert(compressor != nullptr);

      return detail::write_content_chunked(strm, res.content_provider_, is_shutting_down, *compressor);
    } else {
      return detail::write_content_without_length(strm, res.content_provider_, is_shutting_down);
    }
  }
}

inline bool Server::read_content(Stream &strm, Request &req, Response &res) {
  MultipartFormDataMap::iterator cur;
  auto                           file_count = 0;
  if (read_content_core(
          strm,
          req,
          res,
          // Regular
          [&](const char *buf, size_t n) {
            if (req.body.size() + n > req.body.max_size()) {
              return false;
            }
            req.body.append(buf, n);
            return true;
          },
          // Multipart
          [&](const MultipartFormData &file) {
            if (file_count++ == CPPHTTPLIB_MULTIPART_FORM_DATA_FILE_MAX_COUNT) {
              return false;
            }
            cur = req.files.emplace(file.name, file);
            return true;
          },
          [&](const char *buf, size_t n) {
            auto &content = cur->second.content;
            if (content.size() + n > content.max_size()) {
              return false;
            }
            content.append(buf, n);
            return true;
          })) {
    const auto &content_type = req.get_header_value("Content-Type");
    if (!content_type.find("application/x-www-form-urlencoded")) {
      if (req.body.size() > CPPHTTPLIB_FORM_URL_ENCODED_PAYLOAD_MAX_LENGTH) {
        res.status = 413;  // NOTE: should be 414?
        return false;
      }
      detail::parse_query_text(req.body, req.params);
    }
    return true;
  }
  return false;
}

inline bool Server::read_content_with_content_receiver(
    Stream                &strm,
    Request               &req,
    Response              &res,
    ContentReceiver        receiver,
    MultipartContentHeader multipart_header,
    ContentReceiver        multipart_receiver) {
  return read_content_core(
      strm, req, res, std::move(receiver), std::move(multipart_header), std::move(multipart_receiver));
}

inline bool Server::read_content_core(
    Stream                &strm,
    Request               &req,
    Response              &res,
    ContentReceiver        receiver,
    MultipartContentHeader mulitpart_header,
    ContentReceiver        multipart_receiver) {
  detail::MultipartFormDataParser multipart_form_data_parser;
  ContentReceiverWithProgress     out;

  if (req.is_multipart_form_data()) {
    const auto &content_type = req.get_header_value("Content-Type");
    std::string boundary;
    if (!detail::parse_multipart_boundary(content_type, boundary)) {
      res.status = 400;
      return false;
    }

    multipart_form_data_parser.set_boundary(std::move(boundary));
    out = [&](const char *buf, size_t n, uint64_t /*off*/, uint64_t /*len*/) {
      /* For debug
      size_t pos = 0;
      while (pos < n) {
        auto read_size = (std::min)<size_t>(1, n - pos);
        auto ret = multipart_form_data_parser.parse(
            buf + pos, read_size, multipart_receiver, mulitpart_header);
        if (!ret) { return false; }
        pos += read_size;
      }
      return true;
      */
      return multipart_form_data_parser.parse(buf, n, multipart_receiver, mulitpart_header);
    };
  } else {
    out = [receiver](const char *buf, size_t n, uint64_t /*off*/, uint64_t /*len*/) { return receiver(buf, n); };
  }

  if (req.method == "DELETE" && !req.has_header("Content-Length")) {
    return true;
  }

  if (!detail::read_content(strm, req, payload_max_length_, res.status, nullptr, out, true)) {
    return false;
  }

  if (req.is_multipart_form_data()) {
    if (!multipart_form_data_parser.is_valid()) {
      res.status = 400;
      return false;
    }
  }

  return true;
}

inline bool Server::handle_file_request(const Request &req, Response &res, bool head) {
  for (const auto &entry : base_dirs_) {
    // Prefix match
    if (!req.path.compare(0, entry.mount_point.size(), entry.mount_point)) {
      std::string sub_path = "/" + req.path.substr(entry.mount_point.size());
      if (detail::is_valid_path(sub_path)) {
        auto path = entry.base_dir + sub_path;
        if (path.back() == '/') {
          path += "index.html";
        }

        if (detail::is_file(path)) {
          detail::read_file(path, res.body);
          auto type = detail::find_content_type(path, file_extension_and_mimetype_map_);
          if (type) {
            res.set_header("Content-Type", type);
          }
          for (const auto &kv : entry.headers) {
            res.set_header(kv.first.c_str(), kv.second);
          }
          res.status = req.has_header("Range") ? 206 : 200;
          if (!head && file_request_handler_) {
            file_request_handler_(req, res);
          }
          return true;
        }
      }
    }
  }
  return false;
}

inline socket_t Server::create_server_socket(
    const std::string &host, int port, int socket_flags, SocketOptions socket_options) const {
  return detail::create_socket(
      host,
      std::string(),
      port,
      address_family_,
      socket_flags,
      tcp_nodelay_,
      std::move(socket_options),
      [](socket_t sock, struct addrinfo &ai) -> bool {
        if (::bind(sock, ai.ai_addr, static_cast<socklen_t>(ai.ai_addrlen))) {
          return false;
        }
        if (::listen(sock, CPPHTTPLIB_LISTEN_BACKLOG)) {
          return false;
        }
        return true;
      });
}

inline int Server::bind_internal(const std::string &host, int port, int socket_flags) {
  if (!is_valid()) {
    return -1;
  }

  svr_sock_ = create_server_socket(host, port, socket_flags, socket_options_);
  if (svr_sock_ == INVALID_SOCKET) {
    return -1;
  }

  if (port == 0) {
    struct sockaddr_storage addr;
    socklen_t               addr_len = sizeof(addr);
    if (getsockname(svr_sock_, reinterpret_cast<struct sockaddr *>(&addr), &addr_len) == -1) {
      return -1;
    }
    if (addr.ss_family == AF_INET) {
      return ntohs(reinterpret_cast<struct sockaddr_in *>(&addr)->sin_port);
    } else if (addr.ss_family == AF_INET6) {
      return ntohs(reinterpret_cast<struct sockaddr_in6 *>(&addr)->sin6_port);
    } else {
      return -1;
    }
  } else {
    return port;
  }
}

inline bool Server::listen_internal() {
  auto ret    = true;
  is_running_ = true;

  {
    while (svr_sock_ != INVALID_SOCKET) {
      if (idle_interval_sec_ > 0 || idle_interval_usec_ > 0) {
        auto val = detail::select_read(svr_sock_, idle_interval_sec_, idle_interval_usec_);
        if (val == 0) {  // Timeout
          continue;
        }
      }
      socket_t sock = accept(svr_sock_, nullptr, nullptr);

      if (sock == INVALID_SOCKET) {
        if (errno == EMFILE) {
          // The per-process limit of open file descriptors has been reached.
          // Try to accept new connections after a short sleep.
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
          continue;
        }
        if (svr_sock_ != INVALID_SOCKET) {
          detail::close_socket(svr_sock_);
          ret = false;
        } else {
          ;  // The server socket was closed by user.
        }
        break;
      }

      {
        timeval tv;
        tv.tv_sec  = static_cast<long>(read_timeout_sec_);
        tv.tv_usec = static_cast<decltype(tv.tv_usec)>(read_timeout_usec_);
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
      }
      {
        timeval tv;
        tv.tv_sec  = static_cast<long>(write_timeout_sec_);
        tv.tv_usec = static_cast<decltype(tv.tv_usec)>(write_timeout_usec_);
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));
      }

      new_task_queue->submit([=, this]() { process_and_close_socket(sock); });
    }

    new_task_queue->join();
  }

  is_running_ = false;
  return ret;
}

inline bool Server::routing(Request &req, Response &res, Stream &strm) {
  if (pre_routing_handler_ && pre_routing_handler_(req, res) == HandlerResponse::Handled) {
    return true;
  }

  // Regular handler
  if (req.method == "GET" && req.path == "/ping") {
    return true;
  }

  if (req.method == "POST") {
    return dispatch_request(req, res);
  }

  res.status = 400;
  return false;
}

inline bool Server::dispatch_request(Request &req, Response &res) {
  std::unique_ptr<dsac::abd> handler = dsac::abd::factory::construct(req.path);
  if (handler != nullptr) {
    handler->handle(req.body);
    return true;
  }
  return false;
}

inline bool Server::process_request(
    Stream &strm, bool close_connection, bool &connection_closed, const std::function<void(Request &)> &setup_request) {
  std::array<char, 2048> buf{};

  detail::stream_line_reader line_reader(strm, buf.data(), buf.size());

  // Connection has been closed on client
  if (!line_reader.getline()) {
    return false;
  }

  Request  req;
  Response res;

  res.version = "HTTP/1.1";

  for (const auto &header : default_headers_) {
    if (res.headers.find(header.first) == res.headers.end()) {
      res.headers.insert(header);
    }
  }

  // Check if the request URI doesn't exceed the limit
  if (line_reader.size() > CPPHTTPLIB_REQUEST_URI_MAX_LENGTH) {
    Headers dummy;
    detail::read_headers(strm, dummy);
    res.status = 414;
    return write_response(strm, close_connection, req, res);
  }

  // Request line and headers
  if (!parse_request_line(line_reader.ptr(), req) || !detail::read_headers(strm, req.headers)) {
    res.status = 400;
    return write_response(strm, close_connection, req, res);
  }

  if (req.get_header_value("Connection") == "close") {
    connection_closed = true;
  }

  if (req.version == "HTTP/1.0" && req.get_header_value("Connection") != "Keep-Alive") {
    connection_closed = true;
  }

  strm.get_remote_ip_and_port(req.remote_addr, req.remote_port);
  req.set_header("REMOTE_ADDR", req.remote_addr);
  req.set_header("REMOTE_PORT", std::to_string(req.remote_port));

  if (req.has_header("Range")) {
    const auto &range_header_value = req.get_header_value("Range");
    if (!detail::parse_range_header(range_header_value, req.ranges)) {
      res.status = 416;
      return write_response(strm, close_connection, req, res);
    }
  }

  if (setup_request) {
    setup_request(req);
  }

  if (req.get_header_value("Expect") == "100-continue") {
    auto status = 100;
    if (expect_100_continue_handler_) {
      status = expect_100_continue_handler_(req, res);
    }
    switch (status) {
      case 100:
      case 417:
        strm.write_format("HTTP/1.1 %d %s\r\n\r\n", status, detail::status_message(status));
        break;
      default:
        return write_response(strm, close_connection, req, res);
    }
  }

  // Rounting
  bool routed = false;

  try {
    routed = routing(req, res, strm);
  } catch (std::exception &e) {
    if (exception_handler_) {
      auto ep = std::current_exception();
      exception_handler_(req, res, ep);
      routed = true;
    } else {
      res.status = 500;
      res.set_header("EXCEPTION_WHAT", e.what());
    }
  } catch (...) {
    if (exception_handler_) {
      auto ep = std::current_exception();
      exception_handler_(req, res, ep);
      routed = true;
    } else {
      res.status = 500;
      res.set_header("EXCEPTION_WHAT", "UNKNOWN");
    }
  }

  if (routed) {
    if (res.status == -1) {
      res.status = req.ranges.empty() ? 200 : 206;
    }
    return write_response_with_content(strm, close_connection, req, res);
  } else {
    if (res.status == -1) {
      res.status = 404;
    }
    return write_response(strm, close_connection, req, res);
  }
}

inline bool Server::is_valid() const {
  return true;
}

inline bool Server::process_and_close_socket(socket_t sock) {
  auto ret = detail::process_server_socket(
      svr_sock_,
      sock,
      keep_alive_max_count_,
      keep_alive_timeout_sec_,
      read_timeout_sec_,
      read_timeout_usec_,
      write_timeout_sec_,
      write_timeout_usec_,
      [this](Stream &strm, bool close_connection, bool &connection_closed) {
        return process_request(strm, close_connection, connection_closed, nullptr);
      });

  detail::shutdown_socket(sock);
  detail::close_socket(sock);
  return ret;
}

// HTTP client implementation
inline ClientImpl::ClientImpl(const std::string &host)
  : ClientImpl(host, 80, std::string(), std::string()) {
}

inline ClientImpl::ClientImpl(const std::string &host, int port)
  : ClientImpl(host, port, std::string(), std::string()) {
}

inline ClientImpl::ClientImpl(
    const std::string &host, int port, const std::string &client_cert_path, const std::string &client_key_path)
  : host_(host)
  , port_(port)
  , host_and_port_(adjust_host_string(host) + ":" + std::to_string(port))
  , client_cert_path_(client_cert_path)
  , client_key_path_(client_key_path) {
}

inline ClientImpl::~ClientImpl() {
  std::lock_guard<std::mutex> guard(socket_mutex_);
  shutdown_socket(socket_);
  close_socket(socket_);
}

inline bool ClientImpl::is_valid() const {
  return true;
}

inline void ClientImpl::copy_settings(const ClientImpl &rhs) {
  client_cert_path_              = rhs.client_cert_path_;
  client_key_path_               = rhs.client_key_path_;
  connection_timeout_sec_        = rhs.connection_timeout_sec_;
  read_timeout_sec_              = rhs.read_timeout_sec_;
  read_timeout_usec_             = rhs.read_timeout_usec_;
  write_timeout_sec_             = rhs.write_timeout_sec_;
  write_timeout_usec_            = rhs.write_timeout_usec_;
  basic_auth_username_           = rhs.basic_auth_username_;
  basic_auth_password_           = rhs.basic_auth_password_;
  bearer_token_auth_token_       = rhs.bearer_token_auth_token_;
  keep_alive_                    = rhs.keep_alive_;
  follow_location_               = rhs.follow_location_;
  url_encode_                    = rhs.url_encode_;
  address_family_                = rhs.address_family_;
  tcp_nodelay_                   = rhs.tcp_nodelay_;
  socket_options_                = rhs.socket_options_;
  compress_                      = rhs.compress_;
  decompress_                    = rhs.decompress_;
  interface_                     = rhs.interface_;
  proxy_host_                    = rhs.proxy_host_;
  proxy_port_                    = rhs.proxy_port_;
  proxy_basic_auth_username_     = rhs.proxy_basic_auth_username_;
  proxy_basic_auth_password_     = rhs.proxy_basic_auth_password_;
  proxy_bearer_token_auth_token_ = rhs.proxy_bearer_token_auth_token_;
}

inline socket_t ClientImpl::create_client_socket(Error &error) const {
  if (!proxy_host_.empty() && proxy_port_ != -1) {
    return detail::create_client_socket(
        proxy_host_,
        std::string(),
        proxy_port_,
        address_family_,
        tcp_nodelay_,
        socket_options_,
        connection_timeout_sec_,
        connection_timeout_usec_,
        read_timeout_sec_,
        read_timeout_usec_,
        write_timeout_sec_,
        write_timeout_usec_,
        interface_,
        error);
  }

  // Check is custom IP specified for host_
  std::string ip;
  auto        it = addr_map_.find(host_);
  if (it != addr_map_.end()) ip = it->second;

  return detail::create_client_socket(
      host_,
      ip,
      port_,
      address_family_,
      tcp_nodelay_,
      socket_options_,
      connection_timeout_sec_,
      connection_timeout_usec_,
      read_timeout_sec_,
      read_timeout_usec_,
      write_timeout_sec_,
      write_timeout_usec_,
      interface_,
      error);
}

inline bool ClientImpl::create_and_connect_socket(Socket &socket, Error &error) {
  auto sock = create_client_socket(error);
  if (sock == INVALID_SOCKET) {
    return false;
  }
  socket.sock = sock;
  return true;
}

inline void ClientImpl::shutdown_socket(Socket &socket) {
  if (socket.sock == INVALID_SOCKET) {
    return;
  }
  detail::shutdown_socket(socket.sock);
}

inline void ClientImpl::close_socket(Socket &socket) {
  assert(socket_requests_in_flight_ == 0 || socket_requests_are_from_thread_ == std::this_thread::get_id());

  if (socket.sock == INVALID_SOCKET) {
    return;
  }
  detail::close_socket(socket.sock);
  socket.sock = INVALID_SOCKET;
}

inline bool ClientImpl::read_response_line(Stream &strm, const Request &req, Response &res) {
  std::array<char, 2048> buf{};

  detail::stream_line_reader line_reader(strm, buf.data(), buf.size());

  if (!line_reader.getline()) {
    return false;
  }

  const static std::regex re("(HTTP/1\\.[01]) (\\d{3})(?: (.*?))?\r?\n");

  std::cmatch m;
  if (!std::regex_match(line_reader.ptr(), m, re)) {
    return req.method == "CONNECT";
  }
  res.version = std::string(m[1]);
  res.status  = std::stoi(std::string(m[2]));
  res.reason  = std::string(m[3]);

  // Ignore '100 Continue'
  while (res.status == 100) {
    if (!line_reader.getline()) {
      return false;
    }  // CRLF
    if (!line_reader.getline()) {
      return false;
    }  // next response line

    if (!std::regex_match(line_reader.ptr(), m, re)) {
      return false;
    }
    res.version = std::string(m[1]);
    res.status  = std::stoi(std::string(m[2]));
    res.reason  = std::string(m[3]);
  }

  return true;
}

inline bool ClientImpl::send(Request &req, Response &res, Error &error) {
  std::lock_guard<std::recursive_mutex> request_mutex_guard(request_mutex_);

  {
    std::lock_guard<std::mutex> guard(socket_mutex_);

    socket_should_be_closed_when_request_is_done_ = false;

    auto is_alive = false;
    if (socket_.is_open()) {
      is_alive = detail::is_socket_alive(socket_.sock);
      if (!is_alive) {
        const bool shutdown_gracefully = false;
        shutdown_socket(socket_);
        close_socket(socket_);
      }
    }

    if (!is_alive) {
      if (!create_and_connect_socket(socket_, error)) {
        return false;
      }
    }

    if (socket_requests_in_flight_ > 1) {
      assert(socket_requests_are_from_thread_ == std::this_thread::get_id());
    }
    socket_requests_in_flight_ += 1;
    socket_requests_are_from_thread_ = std::this_thread::get_id();
  }

  for (const auto &header : default_headers_) {
    if (req.headers.find(header.first) == req.headers.end()) {
      req.headers.insert(header);
    }
  }

  auto close_connection = !keep_alive_;
  auto ret =
      process_socket(socket_, [&](Stream &strm) { return handle_request(strm, req, res, close_connection, error); });

  {
    std::lock_guard<std::mutex> guard(socket_mutex_);
    socket_requests_in_flight_ -= 1;
    if (socket_requests_in_flight_ <= 0) {
      assert(socket_requests_in_flight_ == 0);
      socket_requests_are_from_thread_ = std::thread::id();
    }

    if (socket_should_be_closed_when_request_is_done_ || close_connection || !ret) {
      shutdown_socket(socket_);
      close_socket(socket_);
    }
  }

  if (!ret) {
    if (error == Error::Success) {
      error = Error::Unknown;
    }
  }

  return ret;
}

inline Result ClientImpl::send(const Request &req) {
  auto req2 = req;
  return send_(std::move(req2));
}

inline Result ClientImpl::send_(Request &&req) {
  auto res   = detail::make_unique<Response>();
  auto error = Error::Success;
  auto ret   = send(req, *res, error);
  return Result{ret ? std::move(res) : nullptr, error, std::move(req.headers)};
}

inline bool ClientImpl::handle_request(Stream &strm, Request &req, Response &res, bool close_connection, Error &error) {
  if (req.path.empty()) {
    error = Error::Connection;
    return false;
  }

  auto req_save = req;

  bool ret;

  if (!proxy_host_.empty() && proxy_port_ != -1) {
    auto req2 = req;
    req2.path = "http://" + host_and_port_ + req.path;
    ret       = process_request(strm, req2, res, close_connection, error);
    req       = req2;
    req.path  = req_save.path;
  } else {
    ret = process_request(strm, req, res, close_connection, error);
  }

  if (!ret) {
    return false;
  }

  if (300 < res.status && res.status < 400 && follow_location_) {
    req = req_save;
    ret = redirect(req, res, error);
  }

  return ret;
}

inline bool ClientImpl::redirect(Request &req, Response &res, Error &error) {
  if (req.redirect_count_ == 0) {
    error = Error::ExceedRedirectCount;
    return false;
  }

  auto location = detail::decode_url(res.get_header_value("location"), true);
  if (location.empty()) {
    return false;
  }

  const static std::regex re(
      R"((?:(https?):)?(?://(?:\[([\d:]+)\]|([^:/?#]+))(?::(\d+))?)?([^?#]*(?:\?[^#]*)?)(?:#.*)?)");

  std::smatch m;
  if (!std::regex_match(location, m, re)) {
    return false;
  }

  auto scheme = "http";

  auto next_scheme = m[1].str();
  auto next_host   = m[2].str();
  if (next_host.empty()) {
    next_host = m[3].str();
  }
  auto port_str  = m[4].str();
  auto next_path = m[5].str();

  auto next_port = port_;
  if (!port_str.empty()) {
    next_port = std::stoi(port_str);
  } else if (!next_scheme.empty()) {
    next_port = next_scheme == "https" ? 443 : 80;
  }

  if (next_scheme.empty()) {
    next_scheme = scheme;
  }
  if (next_host.empty()) {
    next_host = host_;
  }
  if (next_path.empty()) {
    next_path = "/";
  }

  if (next_scheme == scheme && next_host == host_ && next_port == port_) {
    return detail::redirect(*this, req, res, next_path, location, error);
  } else {
    if (next_scheme == "https") {
      return false;
    } else {
      ClientImpl cli(next_host.c_str(), next_port);
      cli.copy_settings(*this);
      return detail::redirect(cli, req, res, next_path, location, error);
    }
  }
}

inline bool ClientImpl::write_content_with_provider(Stream &strm, const Request &req, Error &error) {
  auto is_shutting_down = []() { return false; };

  if (req.is_chunked_content_provider_) {
    std::unique_ptr<detail::compressor> compressor;
    { compressor = detail::make_unique<detail::nocompressor>(); }

    return detail::write_content_chunked(strm, req.content_provider_, is_shutting_down, *compressor, error);
  } else {
    return detail::write_content(strm, req.content_provider_, 0, req.content_length_, is_shutting_down, error);
  }
}  // namespace httplib

inline bool ClientImpl::write_request(Stream &strm, Request &req, bool close_connection, Error &error) {
  // Prepare additional headers
  if (close_connection) {
    if (!req.has_header("Connection")) {
      req.headers.emplace("Connection", "close");
    }
  }

  if (!req.has_header("Host")) {
    if (port_ == 80) {
      req.headers.emplace("Host", host_);
    } else {
      req.headers.emplace("Host", host_and_port_);
    }
  }

  if (!req.has_header("Accept")) {
    req.headers.emplace("Accept", "*/*");
  }

  if (!req.has_header("User-Agent")) {
    auto agent = std::string("cpp-httplib/") + CPPHTTPLIB_VERSION;
    req.headers.emplace("User-Agent", agent);
  }

  if (req.body.empty()) {
    if (req.content_provider_) {
      if (!req.is_chunked_content_provider_) {
        if (!req.has_header("Content-Length")) {
          auto length = std::to_string(req.content_length_);
          req.headers.emplace("Content-Length", length);
        }
      }
    } else {
      if (req.method == "POST" || req.method == "PUT" || req.method == "PATCH") {
        req.headers.emplace("Content-Length", "0");
      }
    }
  } else {
    if (!req.has_header("Content-Type")) {
      req.headers.emplace("Content-Type", "text/plain");
    }

    if (!req.has_header("Content-Length")) {
      auto length = std::to_string(req.body.size());
      req.headers.emplace("Content-Length", length);
    }
  }

  if (!basic_auth_password_.empty() || !basic_auth_username_.empty()) {
    if (!req.has_header("Authorization")) {
      req.headers.insert(make_basic_authentication_header(basic_auth_username_, basic_auth_password_, false));
    }
  }

  if (!proxy_basic_auth_username_.empty() && !proxy_basic_auth_password_.empty()) {
    if (!req.has_header("Proxy-Authorization")) {
      req.headers.insert(
          make_basic_authentication_header(proxy_basic_auth_username_, proxy_basic_auth_password_, true));
    }
  }

  if (!bearer_token_auth_token_.empty()) {
    if (!req.has_header("Authorization")) {
      req.headers.insert(make_bearer_token_authentication_header(bearer_token_auth_token_, false));
    }
  }

  if (!proxy_bearer_token_auth_token_.empty()) {
    if (!req.has_header("Proxy-Authorization")) {
      req.headers.insert(make_bearer_token_authentication_header(proxy_bearer_token_auth_token_, true));
    }
  }

  // Request line and headers
  {
    detail::BufferStream bstrm;

    const auto &path = url_encode_ ? detail::encode_url(req.path) : req.path;
    bstrm.write_format("%s %s HTTP/1.1\r\n", req.method.c_str(), path.c_str());

    detail::write_headers(bstrm, req.headers);

    // Flush buffer
    auto &data = bstrm.get_buffer();
    if (!detail::write_data(strm, data.data(), data.size())) {
      error = Error::Write;
      return false;
    }
  }

  // Body
  if (req.body.empty()) {
    return write_content_with_provider(strm, req, error);
  }

  if (!detail::write_data(strm, req.body.data(), req.body.size())) {
    error = Error::Write;
    return false;
  }

  return true;
}

inline std::unique_ptr<Response> ClientImpl::send_with_content_provider(
    Request                     &req,
    const char                  *body,
    size_t                       content_length,
    ContentProvider              content_provider,
    ContentProviderWithoutLength content_provider_without_length,
    const std::string           &content_type,
    Error                       &error) {
  if (!content_type.empty()) {
    req.headers.emplace("Content-Type", content_type);
  }

  {
    if (content_provider) {
      req.content_length_              = content_length;
      req.content_provider_            = std::move(content_provider);
      req.is_chunked_content_provider_ = false;
    } else if (content_provider_without_length) {
      req.content_length_              = 0;
      req.content_provider_            = detail::ContentProviderAdapter(std::move(content_provider_without_length));
      req.is_chunked_content_provider_ = true;
      req.headers.emplace("Transfer-Encoding", "chunked");
    } else {
      req.body.assign(body, content_length);
      ;
    }
  }

  auto res = detail::make_unique<Response>();
  return send(req, *res, error) ? std::move(res) : nullptr;
}

inline Result ClientImpl::send_with_content_provider(
    const std::string           &method,
    const std::string           &path,
    const Headers               &headers,
    const char                  *body,
    size_t                       content_length,
    ContentProvider              content_provider,
    ContentProviderWithoutLength content_provider_without_length,
    const std::string           &content_type) {
  Request req;
  req.method  = method;
  req.headers = headers;
  req.path    = path;

  auto error = Error::Success;

  auto res = send_with_content_provider(
      req,
      body,
      content_length,
      std::move(content_provider),
      std::move(content_provider_without_length),
      content_type,
      error);

  return Result{std::move(res), error, std::move(req.headers)};
}

inline std::string ClientImpl::adjust_host_string(const std::string &host) const {
  if (host.find(':') != std::string::npos) {
    return "[" + host + "]";
  }
  return host;
}

inline bool ClientImpl::process_request(
    Stream &strm, Request &req, Response &res, bool close_connection, Error &error) {
  // Send request
  if (!write_request(strm, req, close_connection, error)) {
    return false;
  }

  // Receive response and headers
  if (!read_response_line(strm, req, res) || !detail::read_headers(strm, res.headers)) {
    error = Error::Read;
    return false;
  }

  // Body
  if ((res.status != 204) && req.method != "HEAD" && req.method != "CONNECT") {
    auto redirect = 300 < res.status && res.status < 400 && follow_location_;

    if (req.response_handler && !redirect) {
      if (!req.response_handler(res)) {
        error = Error::Canceled;
        return false;
      }
    }

    auto out =
        req.content_receiver
            ? static_cast<ContentReceiverWithProgress>([&](const char *buf, size_t n, uint64_t off, uint64_t len) {
                if (redirect) {
                  return true;
                }
                auto ret = req.content_receiver(buf, n, off, len);
                if (!ret) {
                  error = Error::Canceled;
                }
                return ret;
              })
            : static_cast<ContentReceiverWithProgress>(
                  [&](const char *buf, size_t n, uint64_t /*off*/, uint64_t /*len*/) {
                    if (res.body.size() + n > res.body.max_size()) {
                      return false;
                    }
                    res.body.append(buf, n);
                    return true;
                  });

    auto progress = [&](uint64_t current, uint64_t total) {
      if (!req.progress || redirect) {
        return true;
      }
      auto ret = req.progress(current, total);
      if (!ret) {
        error = Error::Canceled;
      }
      return ret;
    };

    int dummy_status;
    if (!detail::read_content(
            strm,
            res,
            (std::numeric_limits<size_t>::max)(),
            dummy_status,
            std::move(progress),
            std::move(out),
            decompress_)) {
      if (error != Error::Canceled) {
        error = Error::Read;
      }
      return false;
    }
  }

  if (res.get_header_value("Connection") == "close" ||
      (res.version == "HTTP/1.0" && res.reason != "Connection established")) {
    std::lock_guard<std::mutex> guard(socket_mutex_);
    shutdown_socket(socket_);
    close_socket(socket_);
  }

  return true;
}

inline bool ClientImpl::process_socket(const Socket &socket, std::function<bool(Stream &strm)> callback) {
  return detail::process_client_socket(
      socket.sock, read_timeout_sec_, read_timeout_usec_, write_timeout_sec_, write_timeout_usec_, std::move(callback));
}

inline size_t ClientImpl::is_socket_open() const {
  std::lock_guard<std::mutex> guard(socket_mutex_);
  return socket_.is_open();
}

inline socket_t ClientImpl::socket() const {
  return socket_.sock;
}

inline void ClientImpl::stop() {
  std::lock_guard<std::mutex> guard(socket_mutex_);

  // If there is anything ongoing right now, the ONLY thread-safe thing we can
  // do is to shutdown_socket, so that threads using this socket suddenly
  // discover they can't read/write any more and error out. Everything else
  // (closing the socket, shutting down) is unsafe because these actions are
  // not thread-safe.
  if (socket_requests_in_flight_ > 0) {
    shutdown_socket(socket_);

    // Aside from that, we set a flag for the socket to be closed when we're
    // done.
    socket_should_be_closed_when_request_is_done_ = true;
    return;
  }

  shutdown_socket(socket_);
  close_socket(socket_);
}

inline void ClientImpl::set_connection_timeout(time_t sec, time_t usec) {
  connection_timeout_sec_  = sec;
  connection_timeout_usec_ = usec;
}

inline void ClientImpl::set_read_timeout(time_t sec, time_t usec) {
  read_timeout_sec_  = sec;
  read_timeout_usec_ = usec;
}

inline void ClientImpl::set_write_timeout(time_t sec, time_t usec) {
  write_timeout_sec_  = sec;
  write_timeout_usec_ = usec;
}

inline void ClientImpl::set_basic_auth(const std::string &username, const std::string &password) {
  basic_auth_username_ = username;
  basic_auth_password_ = password;
}

inline void ClientImpl::set_bearer_token_auth(const std::string &token) {
  bearer_token_auth_token_ = token;
}

inline void ClientImpl::set_keep_alive(bool on) {
  keep_alive_ = on;
}

inline void ClientImpl::set_follow_location(bool on) {
  follow_location_ = on;
}

inline void ClientImpl::set_url_encode(bool on) {
  url_encode_ = on;
}

inline void ClientImpl::set_hostname_addr_map(std::map<std::string, std::string> addr_map) {
  addr_map_ = std::move(addr_map);
}

inline void ClientImpl::set_default_headers(Headers headers) {
  default_headers_ = std::move(headers);
}

inline void ClientImpl::set_address_family(int family) {
  address_family_ = family;
}

inline void ClientImpl::set_tcp_nodelay(bool on) {
  tcp_nodelay_ = on;
}

inline void ClientImpl::set_socket_options(SocketOptions socket_options) {
  socket_options_ = std::move(socket_options);
}

inline void ClientImpl::set_compress(bool on) {
  compress_ = on;
}

inline void ClientImpl::set_decompress(bool on) {
  decompress_ = on;
}

inline void ClientImpl::set_interface(const std::string &intf) {
  interface_ = intf;
}

inline void ClientImpl::set_proxy(const std::string &host, int port) {
  proxy_host_ = host;
  proxy_port_ = port;
}

inline void ClientImpl::set_proxy_basic_auth(const std::string &username, const std::string &password) {
  proxy_basic_auth_username_ = username;
  proxy_basic_auth_password_ = password;
}

inline void ClientImpl::set_proxy_bearer_token_auth(const std::string &token) {
  proxy_bearer_token_auth_token_ = token;
}

// Universal client implementation
inline Client::Client(const std::string &scheme_host_port)
  : Client(scheme_host_port, std::string(), std::string()) {
}

inline Client::Client(
    const std::string &scheme_host_port, const std::string &client_cert_path, const std::string &client_key_path) {
  const static std::regex re(R"((?:([a-z]+):\/\/)?(?:\[([\d:]+)\]|([^:/?#]+))(?::(\d+))?)");

  std::smatch m;
  if (std::regex_match(scheme_host_port, m, re)) {
    auto scheme = m[1].str();

    if (!scheme.empty() && scheme != "http") {
      std::string msg = "'" + scheme + "' scheme is not supported.";
      throw std::invalid_argument(msg);
    }

    auto host = m[2].str();
    if (host.empty()) {
      host = m[3].str();
    }

    auto port_str = m[4].str();
    auto port     = !port_str.empty() ? std::stoi(port_str) : 80;

    cli_ = detail::make_unique<ClientImpl>(host, port, client_cert_path, client_key_path);
  } else {
    cli_ = detail::make_unique<ClientImpl>(scheme_host_port, 80, client_cert_path, client_key_path);
  }
}

inline Client::Client(const std::string &host, int port)
  : cli_(detail::make_unique<ClientImpl>(host, port)) {
}

inline Client::Client(
    const std::string &host, int port, const std::string &client_cert_path, const std::string &client_key_path)
  : cli_(detail::make_unique<ClientImpl>(host, port, client_cert_path, client_key_path)) {
}

inline Client::~Client() {
}

inline bool Client::is_valid() const {
  return cli_ != nullptr && cli_->is_valid();
}

inline bool Client::send(Request &req, Response &res, Error &error) {
  return cli_->send(req, res, error);
}

inline Result Client::send(const Request &req) {
  return cli_->send(req);
}

inline size_t Client::is_socket_open() const {
  return cli_->is_socket_open();
}

inline socket_t Client::socket() const {
  return cli_->socket();
}

inline void Client::stop() {
  cli_->stop();
}

inline void Client::set_hostname_addr_map(std::map<std::string, std::string> addr_map) {
  cli_->set_hostname_addr_map(std::move(addr_map));
}

inline void Client::set_default_headers(Headers headers) {
  cli_->set_default_headers(std::move(headers));
}

inline void Client::set_address_family(int family) {
  cli_->set_address_family(family);
}

inline void Client::set_tcp_nodelay(bool on) {
  cli_->set_tcp_nodelay(on);
}

inline void Client::set_socket_options(SocketOptions socket_options) {
  cli_->set_socket_options(std::move(socket_options));
}

inline void Client::set_connection_timeout(time_t sec, time_t usec) {
  cli_->set_connection_timeout(sec, usec);
}

inline void Client::set_read_timeout(time_t sec, time_t usec) {
  cli_->set_read_timeout(sec, usec);
}

inline void Client::set_write_timeout(time_t sec, time_t usec) {
  cli_->set_write_timeout(sec, usec);
}

inline void Client::set_basic_auth(const std::string &username, const std::string &password) {
  cli_->set_basic_auth(username, password);
}
inline void Client::set_bearer_token_auth(const std::string &token) {
  cli_->set_bearer_token_auth(token);
}

inline void Client::set_keep_alive(bool on) {
  cli_->set_keep_alive(on);
}
inline void Client::set_follow_location(bool on) {
  cli_->set_follow_location(on);
}

inline void Client::set_url_encode(bool on) {
  cli_->set_url_encode(on);
}

inline void Client::set_compress(bool on) {
  cli_->set_compress(on);
}

inline void Client::set_decompress(bool on) {
  cli_->set_decompress(on);
}

inline void Client::set_interface(const std::string &intf) {
  cli_->set_interface(intf);
}

inline void Client::set_proxy(const std::string &host, int port) {
  cli_->set_proxy(host, port);
}
inline void Client::set_proxy_basic_auth(const std::string &username, const std::string &password) {
  cli_->set_proxy_basic_auth(username, password);
}
inline void Client::set_proxy_bearer_token_auth(const std::string &token) {
  cli_->set_proxy_bearer_token_auth(token);
}

}  // namespace httplib
