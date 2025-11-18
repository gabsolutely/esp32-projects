#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <map>
#include <vector>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

//put your wifi name and password
const char* ssid = "yourWifiName";
const char* password = "yourPassword";

//put your button/buzzer pin
const int buttonPin = 15;
const int buzzerPin = 14;

const unsigned long dotThreshold = 300;
const unsigned long charPause = 600;
const unsigned long wordPause = 1400;
const unsigned long linePause = 3000;
const unsigned long debounceTime = 50;

unsigned long pressStart = 0;
unsigned long releaseStart = 0;
unsigned long lastPress = 0;
bool pressed = false;

String currentMorse = "";
String decodedMessage = "";
std::vector<String> historyLines;

std::map<String, char> morseCode = {
    {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'},
    {".", 'E'}, {"..-.", 'F'}, {"--.", 'G'}, {"....", 'H'},
    {"..", 'I'}, {".---", 'J'}, {"-.-", 'K'}, {".-..", 'L'},
    {"--", 'M'}, {"-.", 'N'}, {"---", 'O'}, {".--.", 'P'},
    {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'},
    {"..-", 'U'}, {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'},
    {"-.--", 'Y'}, {"--..", 'Z'},
    {"-----", '0'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'},
    {"....-", '4'}, {".....", '5'}, {"-....", '6'}, {"--...", '7'},
    {"---..", '8'}, {"----.", '9'}
};

void initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
    }
}

void saveLineToSPIFFS(const String &line) {
    File f = SPIFFS.open("/messages.txt", FILE_APPEND);
    if (!f) return;
    f.println(line);
    f.close();
}

std::vector<String> readAllLinesFromSPIFFS() {
    std::vector<String> out;
    if (!SPIFFS.exists("/messages.txt")) return out;
    File f = SPIFFS.open("/messages.txt", "r");
    if (!f) return out;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        line.trim();
        if (line.length()) out.push_back(line);
    }
    f.close();
    return out;
}

void displayMessageOLED() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Morse Messenger:");
    display.println("----------------");

    int lineHeight = 8;
    int maxLines = SCREEN_HEIGHT / lineHeight - 2;

    int start = max(0, (int)historyLines.size() - maxLines);
    for (int i = start; i < historyLines.size(); i++) {
        display.println(historyLines[i]);
    }

    String liveMsg = decodedMessage;
    while (liveMsg.length() > 0 && (historyLines.size() - start + 1) > maxLines) {
        liveMsg = liveMsg.substring(1);
    }

    display.println(liveMsg);
    display.display();
}

void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
        delay(200);
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected: " + WiFi.localIP().toString());
    } else {
        Serial.println("\nWiFi failed, starting AP mode...");
        WiFi.softAP("ESP32_Logger", "12345678");
        Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
    }
}

String escapeForJSON(const String &s) {
    String out = s;
    out.replace("\\", "\\\\");
    out.replace("\"", "\\\"");
    out.replace("\n", "\\n");
    return out;
}

void handleRoot() {
    String html = R"rawliteral(
<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>Morse Dashboard</title>
<style>
body{font-family:Arial;background:#111;color:#fff;padding:12px}
pre{background:#222;padding:12px;border-radius:6px;white-space:pre-wrap}
input,button{margin:4px;padding:6px}
</style>
</head>
<body>
<h2>Morse Dashboard</h2>
<pre id="messages">loading...</pre>
<form id="sendForm">
<input id="msg" placeholder="Send message"/><button>Send</button>
</form>
<button id="clearBtn">Clear</button>
<button id="downloadBtn">Download</button>
<script>
async function fetchData(){
    const r = await fetch('/data');
    const j = await r.json();
    let out = j.messages.join('\\n');
    if(j.current) out += '\\n' + j.current;
    out = out.replace(/\\n/g,'\n');
    document.getElementById('messages').innerText = out;
}
setInterval(fetchData,500);
fetchData();

document.getElementById('sendForm').addEventListener('submit', async ev => {
    ev.preventDefault();
    const v = document.getElementById('msg').value || '';
    await fetch('/send',{
        method:'POST',
        headers:{'Content-Type':'application/x-www-form-urlencoded'},
        body:'msg='+encodeURIComponent(v)
    });
    document.getElementById('msg').value='';
    fetchData();
});

document.getElementById('clearBtn').addEventListener('click', async () => {
    await fetch('/clear',{method:'POST'});
    fetchData();
});

document.getElementById('downloadBtn').addEventListener('click', () => {
    location.href='/download';
});
</script>
</body>
</html>
)rawliteral";
    server.send(200, "text/html", html);
}

void handleData() {
    String json = "{\"messages\":[";
    for (size_t i = 0; i < historyLines.size(); i++) {
        json += "\"" + escapeForJSON(historyLines[i]) + "\"";
        if (i + 1 < historyLines.size()) json += ",";
    }
    json += "],\"current\":\"" + escapeForJSON(decodedMessage) + "\"}";
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", json);
}

void handleSend() {
    if (server.hasArg("msg")) {
        String msg = server.arg("msg");
        msg.replace("\\n", "\n");
        msg.trim();
        if (msg.length()) {
            int start = 0;
            while (start < msg.length()) {
                int idx = msg.indexOf('\n', start);
                String line = (idx == -1) ? msg.substring(start) : msg.substring(start, idx);
                line.trim();
                if (line.length()) {
                    historyLines.push_back(line);
                    saveLineToSPIFFS(line);
                }
                if (idx == -1) break;
                start = idx + 1;
            }
            decodedMessage = "";
            displayMessageOLED();
        }
    }
    server.send(200,"text/plain","OK");
}

void handleClear() {
    SPIFFS.remove("/messages.txt");
    historyLines.clear();
    decodedMessage = "";
    displayMessageOLED();
    server.send(200,"text/plain","CLEARED");
}

void handleDownload() {
    String content = "";
    for (auto &line : readAllLinesFromSPIFFS()) content += line + "\n";
    server.sendHeader("Content-Disposition","attachment; filename=messages.txt");
    server.send(200,"text/plain",content);
}

void decodeMorseLetter() {
    if (currentMorse.length() == 0) return;
    char ch = '?';
    auto it = morseCode.find(currentMorse);
    if (it != morseCode.end()) ch = it->second;
    decodedMessage += ch;
    currentMorse = "";
    displayMessageOLED();
}

void setup() {
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);

    initSPIFFS();
    historyLines = readAllLinesFromSPIFFS();
    connectWiFi();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED init failed");
        while(true) delay(1000);
    }
    display.clearDisplay();
    displayMessageOLED();

    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.on("/send", HTTP_POST, handleSend);
    server.on("/clear", handleClear);
    server.on("/download", handleDownload);
    server.begin();

    Serial.println("Server started at: " + WiFi.localIP().toString());
}

void loop() {
    server.handleClient();

    int state = digitalRead(buttonPin);
    unsigned long now = millis();

    if (state == LOW && !pressed && now - lastPress > debounceTime) {
        pressStart = now;
        pressed = true;
        digitalWrite(buzzerPin, HIGH);
        lastPress = now;
    }

    if (state == HIGH && pressed) {
        pressed = false;
        digitalWrite(buzzerPin, LOW);
        unsigned long pressTime = now - pressStart;
        currentMorse += (pressTime < dotThreshold) ? "." : "-";
        releaseStart = now;
        lastPress = now;
    }

    if (!pressed) {
        unsigned long idle = now - releaseStart;

        if (currentMorse.length() > 0 && idle >= charPause) {
            decodeMorseLetter();
        }

        if (idle >= wordPause && idle < linePause) {
            if (decodedMessage.length() > 0 && decodedMessage.charAt(decodedMessage.length()-1) != ' ') {
                decodedMessage += ' ';
                displayMessageOLED();
            }
        }

        if (idle >= linePause && decodedMessage.length() > 0) {
            historyLines.push_back(decodedMessage);
            saveLineToSPIFFS(decodedMessage);
            decodedMessage = "";
            currentMorse = "";
            displayMessageOLED();
        }
    }
}
