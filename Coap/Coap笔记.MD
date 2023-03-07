<div align="center">
	  <h1>Coap笔记</h1>
  <hr/>
</div>

<div id="dir"><h2>目录</h2>
<ul>
    <li><a href="#howToUse">如何使用?</a></li>
    <li><a href="https://github.com/MrHulu/Solution-Selector/release">下载地址</a></li>
    <li><a href="#struct">源码结构</a></li>
</ul>
</div>

<hr/>
<h2>Server流程</h2>

<div id="开始与结束"><hr/>
<h3>1.开始与结束</h3>
	<pre>
	必须有coap_startup(), 没什么意义，就是要有
    RAII: 有coap_startup()后, 结束必须要有coap_cleanup(); 
	</pre>
</div>

<div id="context"><hr/>
<h3>2.context创建释放上下文</h3>
	<pre>
	<core class="lang-C++">coap_context_t *coap_new_context(const coap_address_t *listen_addr);</core>
    例子: 
	<core class="lang-C++">auto cxt = coap_new_context(nullptr); //通过判断cxt是否为null来确定是否正常, 如果是空, 问题严重
    
	void coap_free_context(coap_context_t *context);</core>
    解释: 此函数清除接收队列和发送队列中的所有条目，并删除已注册的资源，并释放连接的终结点。
    RAII: 有coap_new_context()就必须要有coap_free_context() 
	</pre>
</div>

<div id="网络"><hr/>
<h3>3.网络</h3>
	<pre>
	<code class="lang-C++">
        coap_addr_info_t *coap_resolve_address_info(const coap_str_const_t *server, uint16_t port, uint16_t secure_port, int ai_hints_flags, int scheme_hint_bits);
    </code>
    解释: 将指定的服务器解析为一组coap_address_t, 可用于bind()或connect()。
    参数:
        server - 要解析的地址。
        port - 要使用的不安全的协议端口。默认是5683
        secure_port - 要使用的安全协议端口。一般是port+1, 默认是5684
        ai_hints_flags - 用于内部getaddrinfo()的AI_*提示标志。
        scheme_hint_bits - 要返回哪些方案的信息。一个或多个COAP_URI_SCHEME_*_BIT或一起。
    返回： 一个或多个coap_addr_info_t的链接集, 如果出错则为NULL。
    例子: 
    <code class="lang-C++">
	    auto info_list = coap_resolve_address_info(, 5683, 5684, AI_PASSIVE | AI_NUMERICHOST, COAP_URI_SCHEME_ALL_COAP_BITS);
	    void coap_free_address_info(coap_addr_info_t *info_list)
	</code>
    解释: Free off the one or more linked sets of coap_addr_info_t returned from coap_resolve_address_info().
    参数: info_list - The set of coap_addr_info_t to free off.
    RAII: coap_resolve_address_info()就必须要有coap_free_address_info()

    <code class="lang-C++">
        void coap_address_init(coap_address_t * 	addr);
    </code>
    解释：将给定的coap_address_t对象addr重置为其默认值。特别是，成员的大小必须被初始化为存储地址的可用大小。
    参数：addr 要初始化的coap_address_t对象。
    例子：
    <code class="lang-C++">
        coap_address_init(&listen_addr);
        listen_addr.addr.sin.sin_family = AF_INET;
        listen_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
        listen_addr.addr.sin.sin_port = htons(5683);
    </code>
	</pre>
</div>

<div id="endpoint"><hr/>
<h3>4.endpoint端点</h3>
	<pre>
	<code class="lang-C++"> 
        coap_endpoint_t *coap_new_endpoint(coap_context_t *context, const coap_address_t *listen_addr, coap_proto_t proto);
    </code>
	解释：创建一个新的端点，用于与对等体进行通信。
    参数：
        context 将拥有新端点的coap上下文。
        listen_addr 该端点将侦听传入的请求或发起传出的请求的地址。使用NULL来指定不接受任何传入的请求，并使用一个随机的端点。
        proto 该端点使用的协议
    返回: 一个端点
    例子:
	<code class="lang-C++"> 
	    auto endpoint = coap_new_endpoint(ctx, &info->addr, COAP_PROTO_UDP); //通过判断endpoint是否为null来确定是否正常, 如果是空, 问题严重
	</code>
	</pre>
</div>

<div id="resourceInit"><hr/>
<h3>5.resource资源初始化</h3>
	<pre>
	<code class="lang-C++"> 
        coap_resource_t *coap_resource_init(coap_str_const_t *uri_path, int flags)
	</code>
    解释: 创建一个新的资源对象，并将链接字段初始化为字符串uri_path。这个函数返回新的coap_resource_t对象。
    如果在flags中设置了COAP_RESOURCE_FLAGS_RELEASE_URI时，字符串将被coap_delete_resource()释放掉，
    那么coap_str_const_t的's'变量必须指向常量文本，或者指向分配的coap_str_const_t参数中的数据。
    参数:
        uri_path - 新资源的字符串URI路径。通常不需要前面的'/'，例如，只需要 "full/path/for/resource"。
        flags - 用于内存管理、观察处理和组播支持的标志，由零个或多个COAP_RESOURCE_FLAGS_*组成，放在一起。
                如果flags没有设置COAP_RESOURCE_FLAGS_NOTIFY_CON，那么就假定COAP_RESOURCE_FLAGS_NOTIFY_NON，
                观察通知将以不可确认的方式发送，但每5个通知将是可确认的。如果设置了COAP_RESOURCE_FLAGS_NOTIFY_NON_ALWAYS，观察通知将总是以非确认方式发送。
    返回: 一个指向新对象的指针，如果出错则为NULL。

    <code class="lang-C++">
        coap_resource_t *coap_resource_unknown_init2(coap_method_handler_t put_handler, int flags)
    </code>
    解释: 
        为未知资源处理程序创建一个新的资源对象，支持PUT和对多播请求包的可配置控制。
        与通过使用coap_register_handler()向coap_resource_init()创建的资源添加额外的处理程序一样，POST、GET、DELETE等处理程序也可以被添加到这个资源中。
        应用程序有责任通过使用coap_resource_init()创建新的资源来管理未知的资源（应该为资源的删除指定一个DELETE处理程序），或者通过维护一个活跃的资源列表。
        注意：每个上下文只能有一个未知资源处理程序--附加一个新的处理程序会覆盖之前的定义。
        注意：不可能通过GET请求来观察未知资源--需要由PUT（或POST）处理程序创建一个单独的资源，并使该资源可被观察。这个函数返回新的coap_resource_t对象。
    参数:
        put_handler - PUT处理程序，用于注册未知Uri-Path的资源。
        flags - 零个或多个COAP_RESOURCE_FLAGS_*MCAST*组合在一起，表示如何处理多播数据包。

    返回:  一个指向新对象的指针，如果出错则为NULL。
	</pre>
</div>
	
<div id="resourceAtt"><hr/>
<h3>6.resource资源添加属性</h3>
	<pre>
	<code class="lang-C++"> 
        coap_attr_t *coap_add_attr(coap_resource_t *resource, coap_str_const_t *name, coap_str_const_t *value, int flags)
    </code>
	解释:在给定的资源上注册一个新属性。由于该属性的coap_str_const_字段将指向name和value，调用者必须确保这些指针在该属性的有效期内有效。
    如果名称和/或值字符串将在属性移除时通过设置COAP_ATTR_FLAGS_RELEASE_NAME或COAP_ATTR_FLAGS_RELEASE_VALUE来释放，
    那么coap_str_const_t的's'变量必须指向常量文本，或者指向分配的coap_str_const_t参数的数据。
    参数:
        resource - 用于注册该属性的资源。
        name - 属性的名称，是一个字符串。
        value - 属性的值，是一个字符串，如果没有则为NULL。
        flags - 内存管理的标志（特别是内存的释放）。可能的值。
            COAP_ATTR_FLAGS_RELEASE_NAME 如果这个标志被设置，当属性被删除时，传递给coap_add_attr_release()的名字被释放 
            COAP_ATTR_FLAGS_RELEASE_VALUE 如果这个标志被设置，当属性被删除时，传递给coap_add_attr_release()的值被释放
    返回:一个指向新属性的指针，或者错误时为NULL。
	</pre>
</div>

<div id="resourceReg"><hr/>
<h3>7.resource资源注册回调函数</h3>
	<pre>
	<code class="lang-C++"> 
        void coap_register_request_handler(coap_resource_t *resource, coap_request_t method, coap_method_handler_t handler)
    </code>
	解释：将指定的处理程序注册为请求类型方法的消息处理程序。
    参数:
        resource - 处理程序应被注册的资源。
        method - 要处理的CoAP请求方法。COAP_REQUEST_GET, COAP_REQUEST_POST, COAP_REQUEST_PUT, COAP_REQUEST_DELETE这四种足以
        handler - 要在资源上注册的处理程序。
	</pre>
</div>


<div id="resourceRelatedFunc"><hr/>
<h3>8.一些回调会用上的函数</h3>
    <h4>设置资源是否可被观察</h4>
	<pre>
	<code class="lang-C++"> 
        void coap_resource_set_get_observable(coap_resource_t *resource, int mode)
    </code>
    解释：
        设置一个资源是否是可观察的。
        如果资源是可观察的，并且客户端在请求包中设置了COAP_OPTION_OBSERVE，那么只要资源的状态发生变化（调用coap_resource_trigger_observe()），就会发送一个观察者响应。
    参数:
        resource - The CoAP resource to use.
        mode – 如果要设置Observable则为1，否则为0。
    用法：一般在资源初始化时调用
    </pre>

    <h4>从请求中获取资源路径</h4>
    <pre>
    <code class="lang-C++"> 
        coap_string_t *coap_get_uri_path( const coap_pdu_t *request)
    </code>
    解释：从请求的PDU中提取uri_path字符串。
    参数: request - 请求PDU。
    用法：一般在post或者put的回调函数里被调用，判断资源是否还存在
    RAII: void coap_delete_string(coap_string_t *string)

    <h4>从请求中获取数据</h4>
	<code class="lang-C++"> 
        int coap_get_data_large(const coap_pdu_t *pdu, size_t *len, const uint8_t **data, size_t *offset, size_t *total)
    </code>
    解释： 检索一个PDU的数据，支持跨越多个PDU的大型数据体。注意：当PDU被销毁时，返回时指向的数据会被销毁。
    参数:
        pdu - 指定的PDU。
        len - 返回当前数据的长度。
        data - 返回当前数据的ptr。
        offset - 返回当前数据从包含许多块的主体开始的偏移量（RFC7959）。
        total - 返回主体的总大小。如果offset + length < total，那么后面还有更多的数据。
    返回：如果len、data、offset和total被正确填写，则为1；如果没有数据，则为0。
    用法：一般在post或者put的回调函数里被调用
	</pre>
</div>

<div id="GetCallback"><hr/>
<h3>9.resource Get 回调函数</h3>
	<pre>
	<code class="lang-C++"> 
        typedef void(* coap_method_handler_t) (coap_resource_t *, coap_session_t *, const coap_pdu_t *, const coap_string_t *, coap_pdu_t *)
    </code>
	解释: 回调函数需要自己实现
    例子1:
	<code class="lang-C++"> 
        // libcoap coap-server.cc
        static void hnd_get_index(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, 
                                    const coap_string_t *query COAP_UNUSED, coap_pdu_t *response)
        {
            // TODO: 应该如此修改
            // auto code = coap_pdu_get_code(request);
            // uri_path = coap_get_uri_path(request); // 拿到路径, 不过好像也没什么作用？
            // if(code == COAP_REQUEST_CODE_GET) //判断是否get
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
            // 块传输
            coap_add_data_large_response(resource, session, request, response,
                                        query, COAP_MEDIATYPE_TEXT_PLAIN,
                                        0x2ffff, 0, strlen(INDEX),
                                        (const uint8_t *)INDEX, NULL, NULL);
            // 非块传输
            coap_add_option(response,
                      COAP_OPTION_CONTENT_FORMAT,
                      coap_encode_var_safe(buf, sizeof(buf), COAP_MEDIATYPE_TEXT_PLAIN),
                      buf); //buf 只是一个壳子
            coap_add_data(response, 5, (const uint8_t *)"world");
            
        }
	</code>
    例子2:
	<code class="lang-C++"> 
    // libcoap coap-server.cc
    static void hnd_get_fetch_time(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, 
                                    const coap_string_t *query, coap_pdu_t *response)
    {
        unsigned char buf[40];
        size_t len;
        (void)request; coap_pdu_code_t code = coap_pdu_get_code(request);
        coap_str_const_t *ticks = coap_make_str_const("ticks");

        if (my_clock_base) {
            /* calculate current time */
            coap_tick_t t; coap_ticks(&t);
            auto now = my_clock_base + (t / COAP_TICKS_PER_SECOND);

            // TODO: GET DATA好像没有什么作用
            /* coap_get_data() sets size to 0 on error */
            size_t size; const uint8_t *data;
            (void)coap_get_data(request, &size, &data);

            if (code == COAP_REQUEST_CODE_GET && query != NULL &&
                coap_string_equal(query, ticks)) {
                /* parameter is in query, output ticks */
                len = snprintf((char *)buf, sizeof(buf), "%u", (unsigned int)now);
            } else { 
                /* output human-readable time */
                struct tm *tmp;
                tmp = gmtime(&now);
                if (!tmp)
                {
                    /* If 'now' is not valid */
                    coap_pdu_set_code(response, COAP_RESPONSE_CODE_NOT_FOUND);
                    return;
                }
                else
                {
                    len = strftime((char *)buf, sizeof(buf), "%b %d %H:%M:%S", tmp);
                }
            }
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
            coap_add_data_large_response(resource, session, request, response,
                                        query, COAP_MEDIATYPE_TEXT_PLAIN, 1, 0,
                                        len,
                                        buf, NULL, NULL);
        }
        else {
            /* if my_clock_base was deleted, we pretend to have no such resource */
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_NOT_FOUND);
        }
    }
    </code>
    </pre>
</div>

<div id="PostCallback"><hr/>
<h3>10.resource Post 回调函数</h3>
    <pre>
	<code class="lang-C++"> 
        typedef void(* coap_method_handler_t) (coap_resource_t *, coap_session_t *, const coap_pdu_t *, const coap_string_t *, coap_pdu_t *)
    </code>
	解释: 回调函数需要自己实现

    </pre>
</div>

<div id="PutCallback"><hr/>
<h3>11.resource Put 回调函数</h3>
    <pre>
    例子：
    <code class="lang-C++">
        // libcoap coap-server.cc
        static void hnd_put_example_data(coap_resource_t *resource,
                            coap_session_t *session,
                            const coap_pdu_t *request,
                            const coap_string_t *query COAP_UNUSED,
                            coap_pdu_t *response)
        {
            size_t size;
            const uint8_t *data;
            coap_opt_iterator_t opt_iter;
            coap_opt_t *option;
            size_t offset;
            size_t total;
            coap_binary_t *data_so_far;

            if (coap_get_data_large(request, &size, &data, &offset, &total) &&  size != total) {
                /*
                * A part of the data has been received (COAP_BLOCK_SINGLE_BODY not set).
                * However, total unfortunately is only an indication, so it is not safe to
                * allocate a block based on total.  As per
                * https://rfc-editor.org/rfc/rfc7959#section-4
                *   o  In a request carrying a Block1 Option, to indicate the current
                *         estimate the client has of the total size of the resource
                *         representation, measured in bytes ("size indication").
                *
                * coap_cache_ignore_options() must have previously been called with at
                * least COAP_OPTION_BLOCK1 set as the option value will change per block.
                */
                coap_cache_entry_t *cache_entry = coap_cache_get_by_pdu(session, request, OAP_CACHE_IS_SESSION_BASED);
                if (offset == 0) {
                    if (!cache_entry) {
                        /*
                        * Set idle_timeout parameter to COAP_MAX_TRANSMIT_WAIT if you want
                        * early removal on transmission failure. 0 means only delete when
                        * the session is deleted as session_based is set here.
                        */
                        cache_entry = coap_new_cache_entry(session, request,
                                                        COAP_CACHE_NOT_RECORD_PDU,
                                                        COAP_CACHE_IS_SESSION_BASED, 0);
                    } else {
                        data_so_far = coap_cache_get_app_data(cache_entry);
                        if (data_so_far)
                        {
                        coap_delete_binary(data_so_far);
                        data_so_far = NULL;
                        }
                        coap_cache_set_app_data(cache_entry, NULL, NULL);
                    }
                }
                if (!cache_entry) {
                    if (offset == 0)  {
                        coap_log_warn("Unable to create a new cache entry\n");
                    } else {
                        coap_log_warn( "No cache entry available for the non-first BLOCK\n");
                    }
                    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
                    return;
                }
                if (size) {
                    /* Add in the new data to cache entry */
                    data_so_far = coap_cache_get_app_data(cache_entry);
                    data_so_far = coap_block_build_body(data_so_far, size, data,
                                                        offset, total);
                    /* Yes, data_so_far can be NULL if error */
                    coap_cache_set_app_data(cache_entry, data_so_far, cache_free_app_data);
                }
                if (offset + size == total)  {
                    /* All the data is now in */
                    data_so_far = coap_cache_get_app_data(cache_entry);
                    coap_cache_set_app_data(cache_entry, NULL, NULL);
                } else {
                    /* Give us the next block response */
                    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTINUE);
                    return;
                }
            } else {
                /* single body of data received */
                data_so_far = coap_new_binary(size);
                if (data_so_far)
                {
                memcpy(data_so_far->s, data, size);
                }
            }

            if (example_data_value)  {
                /* pre-existed response */
                coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);
                /* Need to de-reference as value may be in use elsewhere */
                release_resource_data(session, example_data_value);
            } else
                /* just generated response */
                coap_pdu_set_code(response, COAP_RESPONSE_CODE_CREATED);

            example_data_value = alloc_resource_data(data_so_far);
            if (!example_data_value)  {
                coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
                return;
            }
            if ((option = coap_check_option(request, COAP_OPTION_CONTENT_FORMAT, &opt_iter)) != NULL) {
                example_data_media_type = coap_decode_var_bytes(coap_opt_value(option), coap_opt_length(option));
            }
            else  {
                example_data_media_type = COAP_MEDIATYPE_TEXT_PLAIN;
            }
            coap_resource_notify_observers(resource, NULL);
            if (echo_back) {
                coap_binary_t body;
                body = reference_resource_data(example_data_value);
                coap_add_data_large_response(resource, session, request, response,
                                            query, example_data_media_type, -1, 0,
                                            body.length,
                                            body.s,
                                            release_resource_data, example_data_value);
            }
        }
    </code>
    </pre>
</div>

<hr/>
<h2>Client 流程</h2>

<div id="client"><hr/>
<pre>
  void coap_register_response_handler(coap_context_t * 	context, coap_response_handler_t handler);
  解释：注册一个新的消息处理程序，每当收到一个响应时就会被调用。
  参数：
    context 要注册处理程序的上下文。
    handler 要注册的响应处理程序。

  typedef coap_response_t (*coap_response_handler_t)(coap_session_t *session,
                                                   const coap_pdu_t *sent,
                                                   const coap_pdu_t *received,
                                                   const coap_mid_t mid);
    回调函数例子：
    coap_response_t message_handler(coap_session_t *session COAP_UNUSED,
                const coap_pdu_t *sent,
                const coap_pdu_t *received,
                const coap_mid_t id COAP_UNUSED) 
    {
    coap_opt_t *block_opt;
    coap_opt_iterator_t opt_iter;
    size_t len;
    const uint8_t *databuf;
    size_t offset;
    size_t total;
    // 获取接收的code： 402..502等
    coap_pdu_code_t rcv_code = coap_pdu_get_code(received);
    // 获取接收的类型：CON, NON, ACK, RST四个
    coap_pdu_type_t rcv_type = coap_pdu_get_type(received);
    // 获取token: TODO: 客户端需要吗？
    coap_bin_const_t token = coap_pdu_get_token(received);

    /*
        COAP_RESPONSE_CLASS(rcv_code)：
        0 空报文
        2 正确响应
        4 客服端错误
        5 服务器错误

        rcv_code & 0x1F:
        1 GET
        2 POST
        3 PUT
        4 DELETE
    */ 
    coap_log_debug("** process incoming %d.%02d response:\n",
            COAP_RESPONSE_CLASS(rcv_code), rcv_code & 0x1F);
    // if (coap_get_log_level() < COAP_LOG_DEBUG)
    //   coap_show_pdu(COAP_LOG_INFO, received);

    /* check if this is a response to our original request */
    // if (!track_check_token(&token)) {
    //   /* drop if this was just some message, or send RST in case of notification */
    //   if (!sent && (rcv_type == COAP_MESSAGE_CON ||
    //                 rcv_type == COAP_MESSAGE_NON)) {
    //     /* Cause a CoAP RST to be sent */
    //     return COAP_RESPONSE_FAIL;
    //   }
    //   return COAP_RESPONSE_OK;
    // }

    // 表示服务器返回了一个RST
    if (rcv_type == COAP_MESSAGE_RST) {
        coap_log_info("got RST\n");
        return COAP_RESPONSE_OK;
    }

    /* output the received data, if any */
    if (COAP_RESPONSE_CLASS(rcv_code) == 2) {

        /*  
        **********************************************************************
            这段代码是处理CoAP协议中观测（Observe）的部分。
            在这段代码中，如果客户端向服务器订阅了某个资源并且服务器成功地建立了观测关系，那么就会启动一个计时器，
            计时器的时间为 obs_seconds 秒。
            当计时器超时时，就会向服务器重新发送一次请求，以便获取资源最新的状态信息。
            同时，obs_ms_reset 被设置为1，用于表示计时器被重置了。
            这个标志位的作用是，当客户端收到服务器返回的响应时，可以检查这个标志位，
            如果为1则说明计时器已经被重置过了，可以不再启动新的计时器。
            如果为0则说明计时器没有被重置，需要重新启动计时器。 
        **********************************************************************
        */
        if (doing_observe && !obs_started &&
            coap_check_option(received, COAP_OPTION_OBSERVE, &opt_iter)) {
        coap_log_debug(
                "observation relationship established, set timeout to %d\n",
                obs_seconds);
        obs_started = 1; // 观察已经开始
        obs_ms = obs_seconds * 1000;
        obs_ms_reset = 1; // 计时器被重置了
        }

        // do something..
        if (coap_get_data_large(received, &len, &databuf, &offset, &total)) {
        append_to_output(databuf, len);
        if ((len + offset == total) && add_nl)
            append_to_output((const uint8_t*)"\n", 1);
        }

        // /* Check if Block2 option is set */
        // block_opt = coap_check_option(received, COAP_OPTION_BLOCK2, &opt_iter);
        // if (!single_block_requested && block_opt) { /* handle Block2 */

        //   /* TODO: check if we are looking at the correct block number */
        //   if (coap_opt_block_num(block_opt) == 0) {
        //     /* See if observe is set in first response */
        //     ready = doing_observe ? coap_check_option(received,
        //                               COAP_OPTION_OBSERVE, &opt_iter) == NULL : 1;
        //   }
        //   if(COAP_OPT_BLOCK_MORE(block_opt)) {
        //     doing_getting_block = 1;
        //   } else {
        //     doing_getting_block = 0;
        //     if (!is_mcast)
        //       track_flush_token(&token);
        //   }
        //   return COAP_RESPONSE_OK;
        // }
    } else {      /* no 2.05 */
        /* check if an error was signaled and output payload if so */
        if (COAP_RESPONSE_CLASS(rcv_code) >= 4) {
        fprintf(stderr, "%d.%02d", COAP_RESPONSE_CLASS(rcv_code),
                rcv_code & 0x1F);
        if (coap_get_data_large(received, &len, &databuf, &offset, &total)) {
            fprintf(stderr, " ");
            while(len--) {
            fprintf(stderr, "%c", isprint(*databuf) ? *databuf : '.');
            databuf++;
            }
        }
        fprintf(stderr, "\n");
        }

    }
    if (!is_mcast)
        track_flush_token(&token);

    /* our job is done, we can exit at any time */
    ready = doing_observe ? coap_check_option(received,
                                    COAP_OPTION_OBSERVE, &opt_iter) == NULL : 1;
    return COAP_RESPONSE_OK;
    }
  coap_register_event_handler(ctx, event_handler);
  coap_register_nack_handler(ctx, nack_handler);
</pre>
</div>
