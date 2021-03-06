// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/websockets/websocket_handshake_response_info.h"

#include <string>

#include "base/time/time.h"
#include "net/http/http_response_headers.h"
#include "url/gurl.h"

namespace net {

WebSocketHandshakeResponseInfo::WebSocketHandshakeResponseInfo(
    const GURL& url,
    scoped_refptr<HttpResponseHeaders> headers,
    base::Time response_time)
    : url(url), headers(std::move(headers)), response_time(response_time) {}

WebSocketHandshakeResponseInfo::~WebSocketHandshakeResponseInfo() = default;

}  // namespace net
