#ifndef CUSTOM_PSYCHIC_WEBSOCKET_HANDLER_H
#define CUSTOM_PSYCHIC_WEBSOCKET_HANDLER_H

#include "PsychicWebSocket.h"
#include <Arduino.h>
#include <Debuger.h>

class CustomPsychicWebSocketHandler : public PsychicWebSocketHandler
{
public:
    CustomPsychicWebSocketHandler();
    ~CustomPsychicWebSocketHandler();

    // Override the handleRequest method
    esp_err_t handleRequest(PsychicRequest *request) override;
};

#endif // CUSTOM_PSYCHIC_WEBSOCKET_HANDLER_H

