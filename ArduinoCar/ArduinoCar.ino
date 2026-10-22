
// go and get someone else's code
#include <WiFiS3.h>

// WiFi
char ssid[] = "ArduinoCar";
char password[] = "12345678";
WiFiServer server(80);

// constants (nicknames)
const int ENA = 5, IN1 = 6, IN2 = 7;
const int IN3 = 8, IN4 = 9, ENB = 10;
const int SPEED = 255;

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  drive(0, 0);

  WiFi.beginAP(ssid, password);
  server.begin();

  Serial.print("Open: http://");
  Serial.println(WiFi.localIP());
}

void drive(int left, int right) {
  // set direction of IN1, IN2, IN3, IN4 pins
  digitalWrite(IN1, left > 0);
  digitalWrite(IN2, left < 0);
  digitalWrite(IN3, right > 0);
  digitalWrite(IN4, right < 0);

  // set speed
  analogWrite(ENA, abs(left));
  analogWrite(ENB, abs(right));
}

void loop() {
  // start website
  WiFiClient client = server.available();

  if (client) { // if the website is working
    client.setTimeout(50);
    String request = client.readStringUntil('\n');

    // check if a key is pressed
    if (request.indexOf("GET /W") >= 0)
      drive(SPEED, SPEED);

    else if (request.indexOf("GET /S") >= 0)
      drive(-SPEED, -SPEED);

    else if (request.indexOf("GET /A") >= 0)
      drive(-SPEED, SPEED);

    else if (request.indexOf("GET /D") >= 0)
      drive(SPEED, -SPEED);

    else if (request.indexOf("GET /X") >= 0)
      drive(0, 0);

    // website stuff
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    // html for what the website should look like
    client.println(R"rawliteral(
<html>
<body style="text-align:center;font-family:Arial">
<h1>Arduino Car</h1>
<h2>Use WASD to drive</h2>

<script>
let key = '';

document.onkeydown = function(e) {
  let k = e.key.toUpperCase();
  if ('WASD'.includes(k) && k.length === 1) {
    e.preventDefault();
    if (key !== k) {
      key = k;
      fetch('/' + k);
    }
  }
};

document.onkeyup = function(e) {
  if (e.key.toUpperCase() === key) {
    key = '';
    fetch('/X');
  }
};

window.onblur = function() {
  key = '';
  fetch('/X');
};
</script>
</body>
</html>
)rawliteral");

    client.stop();
  }
}
