#include <CustomPsychicWebSocketHandler.h>

// Implementation of CustomPsychicWebSocketHandler
CustomPsychicWebSocketHandler::CustomPsychicWebSocketHandler()
{
    // Constructor implementation
}

CustomPsychicWebSocketHandler::~CustomPsychicWebSocketHandler()
{
    // Destructor implementation
}

esp_err_t CustomPsychicWebSocketHandler::handleRequest(PsychicRequest *request)
{
    Serial.println("CustomPsychicWebSocketHandler::handleRequest");
    PsychicClient *client = checkForNewClient(request->client());

    // if no token reject this client
    // beginning of the ws URI handler and our onConnect hook
    if (request->method() == HTTP_GET)
    {  
        // if (request->queryString())
        // {
        //     Serial.println("Invalid Token");
        //     return ESP_FAIL;
        // }
        // else
        // {
        //     Serial.println("Valid Token");
        //     Serial.println(request->header("Sec-WebSocket-Protocol"));
        // }
        // if(request->url() != "/ws?token=dXNlcjoxMjM0NTY=") {
        //     Serial.println("Invalid Token");
        //     return ESP_FAIL;
        // }
        if (client->isNew)
            openCallback(client);

        return ESP_OK;
    }

    // prep our request
    PsychicWebSocketRequest wsRequest(request);

    // init our memory for storing the packet
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    uint8_t *buf = NULL;

    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(wsRequest.request(), &ws_pkt, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(PH_TAG, "httpd_ws_recv_frame failed to get frame len with %s", esp_err_to_name(ret));
        return ret;
    }

    // okay, now try to load the packet
    // ESP_LOGD(PH_TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len)
    {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = (uint8_t *)calloc(1, ws_pkt.len + 1);
        if (buf == NULL)
        {
            ESP_LOGE(PH_TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(wsRequest.request(), &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            ESP_LOGE(PH_TAG, "httpd_ws_recv_frame failed with %s", esp_err_to_name(ret));
            free(buf);
            return ret;
        }
        // ESP_LOGD(PH_TAG, "Got packet with message: %s", ws_pkt.payload);
    }

    // Text messages are our payload.
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT || ws_pkt.type == HTTPD_WS_TYPE_BINARY)
    {
        if (this->_onFrame != NULL)
            ret = this->_onFrame(&wsRequest, &ws_pkt);
    }

    // logging housekeeping
    if (ret != ESP_OK)
        ESP_LOGE(PH_TAG, "httpd_ws_send_frame failed with %s", esp_err_to_name(ret));
    // ESP_LOGD(PH_TAG, "ws_handler: httpd_handle_t=%p, sockfd=%d, client_info:%d",
    //   request->server(),
    //   httpd_req_to_sockfd(request->request()),
    //   httpd_ws_get_fd_info(request->server()->server, httpd_req_to_sockfd(request->request())));

    // dont forget to release our buffer memory
    free(buf);

    return ret;
}