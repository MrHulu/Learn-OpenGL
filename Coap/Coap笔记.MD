Coap

Server流程

1.开始与结束
    必须有coap_startup(), 没什么意义，就是要有
    RAII: 有coap_startup()后, 结束必须要有coap_cleanup(); 


2.context创建释放上下文
    coap_context_t *coap_new_context(const coap_address_t *listen_addr);
    例子: auto cxt = coap_new_context(nullptr); // 通过判断cxt是否为null来确定是否正常, 如果是空, 问题严重
    
    void coap_free_context(coap_context_t *context);
    解释: 此函数清除接收队列和发送队列中的所有条目，并删除已注册的资源，并释放连接的终结点。
    RAII: 有coap_new_context()就必须要有coap_free_context()

3.网络
    coap_addr_info_t *coap_resolve_address_info(const coap_str_const_t *server, uint16_t port, uint16_t secure_port, int ai_hints_flags, int scheme_hint_bits);
    解释: 将指定的服务器解析为一组coap_address_t, 可用于bind()或connect()。
    参数:
        server - 要解析的地址。
        port - 要使用的不安全的协议端口。默认是5683
        secure_port - 要使用的安全协议端口。一般是port+1, 默认是5684
        ai_hints_flags - 用于内部getaddrinfo()的AI_*提示标志。
        scheme_hint_bits - 要返回哪些方案的信息。一个或多个COAP_URI_SCHEME_*_BIT或一起。
    返回： 一个或多个coap_addr_info_t的链接集, 如果出错则为NULL。
    例子: auto info_list = coap_resolve_address_info(, 5683, 5684, AI_PASSIVE | AI_NUMERICHOST, COAP_URI_SCHEME_ALL_COAP_BITS);

    void coap_free_address_info(coap_addr_info_t *info_list)
    解释: Free off the one or more linked sets of coap_addr_info_t returned from coap_resolve_address_info().
    参数: info_list - The set of coap_addr_info_t to free off.
    RAII: coap_resolve_address_info()就必须要有coap_free_address_info()

4.endpoint端点
    coap_endpoint_t *coap_new_endpoint(coap_context_t *context, const coap_address_t *listen_addr, coap_proto_t proto);
    解释：创建一个新的端点，用于与对等体进行通信。
    参数：
        context 将拥有新端点的coap上下文。
        listen_addr 该端点将侦听传入的请求或发起传出的请求的地址。使用NULL来指定不接受任何传入的请求，并使用一个随机的端点。
        proto 该端点使用的协议
    返回: 一个端点
    例子: auto endpoint = coap_new_endpoint(ctx, &info->addr, COAP_PROTO_UDP); //通过判断endpoint是否为null来确定是否正常, 如果是空, 问题严重

5.resource资源初始化
    coap_resource_t *coap_resource_init(coap_str_const_t *uri_path, int flags)
    解释: 创建一个新的资源对象，并将链接字段初始化为字符串uri_path。这个函数返回新的coap_resource_t对象。
    如果在flags中设置了COAP_RESOURCE_FLAGS_RELEASE_URI时，字符串将被coap_delete_resource()释放掉，
    那么coap_str_const_t的's'变量必须指向常量文本，或者指向分配的coap_str_const_t参数中的数据。
    参数:
        uri_path - 新资源的字符串URI路径。通常不需要前面的'/'，例如，只需要 "full/path/for/resource"。
        flags - 用于内存管理、观察处理和组播支持的标志，由零个或多个COAP_RESOURCE_FLAGS_*组成，放在一起。
                如果flags没有设置COAP_RESOURCE_FLAGS_NOTIFY_CON，那么就假定COAP_RESOURCE_FLAGS_NOTIFY_NON，
                观察通知将以不可确认的方式发送，但每5个通知将是可确认的。如果设置了COAP_RESOURCE_FLAGS_NOTIFY_NON_ALWAYS，观察通知将总是以非确认方式发送。
    返回: 一个指向新对象的指针，如果出错则为NULL。

6.resource资源添加属性
    coap_attr_t *coap_add_attr(coap_resource_t *resource, coap_str_const_t *name, coap_str_const_t *value, int flags)
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

7.resource资源注册回调函数
    void coap_register_request_handler(coap_resource_t *resource, coap_request_t method, coap_method_handler_t handler)
    解释：将指定的处理程序注册为请求类型方法的消息处理程序。
    参数:
        resource - 处理程序应被注册的资源。
        method - 要处理的CoAP请求方法。COAP_REQUEST_GET, COAP_REQUEST_POST, COAP_REQUEST_PUT, COAP_REQUEST_DELETE这四种足以
        handler - 要在资源上注册的处理程序。

8.resource Get 回调函数
    typedef void(* coap_method_handler_t) (coap_resource_t *, coap_session_t *, const coap_pdu_t *, const coap_string_t *, coap_pdu_t *)
    解释: 回调函数需要自己实现
    例子1:
        // libcoap coap-server.cc
        static void hnd_get_index(coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, 
                                    const coap_string_t *query COAP_UNUSED, coap_pdu_t *response)
        {
            // TODO: 应该如此修改
            auto code = coap_pdu_get_code(request);
            size_t size; const uint8_t *data;
            (void)coap_get_data(request, &size, &data);
            // if(code == COAP_REQUEST_CODE_GET) //判断是否get
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
            coap_add_data_large_response(resource, session, request, response,
                                        query, COAP_MEDIATYPE_TEXT_PLAIN,
                                        0x2ffff, 0, strlen(INDEX),
                                        (const uint8_t *)INDEX, NULL, NULL);
        }
    例子2:
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


