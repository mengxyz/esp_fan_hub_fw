#include <Arduino.h>

#define S_CMD_RESET "C:RS"
#define S_CMD_RESTART "C:RST"
#define S_CMD_SET_WIFI_SSID "C:S:W_SSID"
#define S_CMD_SET_WIFI_PASSWORD "C:S:W_PASS"
#define S_CMD_SET_WIFI_MDSN "C:S:W_MDSN"
#define S_CMD_SET_USER_NAME "C:S:U_NAME"
#define S_CMD_SET_USER_PASSWORD "C:S:U_PASS"
#define S_NOTHING ""

class SerialTool
{
private:
    Stream &serial;
    bool waitingForAnswer = false;
    String answer = "";
    uint8_t answerTimeout = 3000;
    uint32_t lastAnswerTime = 0;
    String lastCommand = S_NOTHING;
public:
    String answerString() { return answer; }
    void clear() {
        answer = "";
        lastCommand = S_NOTHING;
        waitingForAnswer = false;
    }
    void hasCommand(String *cmd, string *ans) {
        if (serial.available()) {
            String command = serial.readStringUntil('\n');
            command.trim();
            if (waitingForAnswer) {
                if (millis() - lastAnswerTime > answerTimeout) {
                    waitingForAnswer = false;
                    answer = "";
                    lastCommand = S_NOTHING;
                }
                if (lastCommand != command) {
                    lastAnswerTime = millis();
                    ans = command;
                    cmd = lastCommand;
                    waitingForAnswer = false;
                }
            }
            cmd = S_NOTHING;
            if (command == S_CMD_RESET) {
                cmd =  S_CMD_RESET;
            } else if (command == S_CMD_RESTART) {
                cmd = S_CMD_RESTART;
            } else if (command == S_CMD_SET_WIFI_SSID) {
                waitingForAnswer = true;
            } else if (command == S_CMD_SET_WIFI_PASSWORD) {
                waitingForAnswer = true;
            } else if (command == S_CMD_SET_WIFI_MDSN) {
                waitingForAnswer = true;
            } else if (command == S_CMD_SET_USER_NAME) {
                waitingForAnswer = true;
            } else if (command == S_CMD_SET_USER_PASSWORD) {
                waitingForAnswer = true;
            }
        }
        cmd = S_NOTHING;
    }
};