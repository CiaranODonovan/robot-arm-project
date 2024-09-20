#include <Servo.h>
#include <ESP8266WiFi.h>
// Enter Wifi SSID
const char* ssid = "";
// Enter Wifi password
const char* password = "";

// Web server on port 80 (http)
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Servo objects
Servo baseServo;
Servo lowerArmServo;
Servo upperArmServo;
Servo wristServo;
Servo fingerServo;
Servo forearmServo;  

// Current servo positions
int basePos = 90;
int lowerArmPos = 36; // Initialize efficient starting position
int upperArmPos = 45; // Initialize efficient starting position
int wristPos = 90;
int fingerPos = 90;
int forearmPos = 90;  

// Target positions for each servo
int baseTarget = 90;
int lowerArmTarget = 36;
int upperArmTarget = 45;
int wristTarget = 90;
int fingerTarget = 90;
int forearmTarget = 90;  

// Speed for each servo in degrees per second 
float baseSpeed = 75.0;
float lowerArmSpeed = 75.0;
float upperArmSpeed = 75.0;
float wristSpeed = 75.0;
float fingerSpeed = 75.0;
float forearmSpeed = 75.0;  

// Number of servos
const int numServos = 6;  

// Servo objects
Servo servos[numServos] = {baseServo, lowerArmServo, upperArmServo, wristServo, fingerServo, forearmServo};  

// Current positions for each servo
int currentPos[numServos] = {basePos, lowerArmPos, upperArmPos, wristPos, fingerPos, forearmPos};  

// Target positions for each servo
int targetPos[numServos] = {baseTarget, lowerArmTarget, upperArmTarget, wristTarget, fingerTarget, forearmTarget};  
// Speeds for each servo in degrees per second
float servoSpeed[numServos] = {baseSpeed, lowerArmSpeed, upperArmSpeed, wristSpeed, fingerSpeed, forearmSpeed}; 

// Timing variables for each servo
unsigned long lastUpdateMillis[numServos] = {0, 0, 0, 0, 0, 0};  
const unsigned long interval = 20; // Update interval in milliseconds

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Serial Feedback
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

   // Attach servos to their respective GPIO pins (ESP8266)
  servos[0].attach(4, 500, 2400);   // GPIO4 (D2)
  servos[1].attach(5, 500, 2400);   // GPIO5 (D1)
  servos[2].attach(12, 500, 2400);  // GPIO12 (D6)
  servos[3].attach(13, 500, 2400);  // GPIO13 (D7)
  servos[4].attach(14, 500, 2400);  // GPIO14 (D5)
  servos[5].attach(0, 500, 2400);   // GPIO0 (D3) 

  // Set initial positions for servos
  for (int i = 0; i < numServos; i++) {
    servos[i].write(currentPos[i]);
  }
}

void loop() {
  // Handle web client connections
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String header = "";
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send the HTTP response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // HTML code for the web interface
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; }");
            client.println(".slider { width: 300px; }</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
            client.println("</head><body><h1>Servo Control</h1>");

            // Sliders for each servo with current position display
            client.println("<p>Base Servo: <span id=\"baseValue\">" + String(currentPos[0]) + "</span>&#176;</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"" + String(targetPos[0]) + "\" class=\"slider\" id=\"baseSlider\" onchange=\"updateServo(this.id, this.value)\">");

            client.println("<p>Lower Arm Servo: <span id=\"lowerArmValue\">" + String(currentPos[1]) + "</span>&#176;</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"" + String(targetPos[1]) + "\" class=\"slider\" id=\"lowerArmSlider\" onchange=\"updateServo(this.id, this.value)\">");

            client.println("<p>Upper Arm Servo: <span id=\"upperArmValue\">" + String(currentPos[2]) + "</span>&#176;</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"" + String(targetPos[2]) + "\" class=\"slider\" id=\"upperArmSlider\" onchange=\"updateServo(this.id, this.value)\">");

            client.println("<p>Wrist Servo: <span id=\"wristValue\">" + String(currentPos[3]) + "</span>&#176;</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"" + String(targetPos[3]) + "\" class=\"slider\" id=\"wristSlider\" onchange=\"updateServo(this.id, this.value)\">");

            client.println("<p>Finger Servo: <span id=\"fingerValue\">" + String(currentPos[4]) + "</span>&#176;</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"" + String(targetPos[4]) + "\" class=\"slider\" id=\"fingerSlider\" onchange=\"updateServo(this.id, this.value)\">");

            client.println("<p>Forearm Servo: <span id=\"forearmValue\">" + String(currentPos[5]) + "</span>&#176;</p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" value=\"" + String(targetPos[5]) + "\" class=\"slider\" id=\"forearmSlider\" onchange=\"updateServo(this.id, this.value)\">");

            client.println("<script>");
            client.println("function updateServo(id, value) {");
            client.println("document.getElementById(id.replace('Slider', 'Value')).innerHTML = value;");
            client.println("$.get(\"/?\" + id + \"=\" + value);");
            client.println("}");
            client.println("</script>");

            client.println("</body></html>");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    // Handle the HTTP request and update servo target positions
    if (header.indexOf("baseSlider=") >= 0) {
      int pos = header.indexOf("baseSlider=") + strlen("baseSlider=");
      int end = header.indexOf("&", pos);
      targetPos[0] = header.substring(pos, end).toInt();
      Serial.println("Base Servo Target: " + String(targetPos[0]));
    }
    if (header.indexOf("lowerArmSlider=") >= 0) {
      int pos = header.indexOf("lowerArmSlider=") + strlen("lowerArmSlider=");
      int end = header.indexOf("&", pos);
      targetPos[1] = header.substring(pos, end).toInt();
      Serial.println("Lower Arm Servo Target: " + String(targetPos[1]));
    }
    if (header.indexOf("upperArmSlider=") >= 0) {
      int pos = header.indexOf("upperArmSlider=") + strlen("upperArmSlider=");
      int end = header.indexOf("&", pos);
      targetPos[2] = header.substring(pos, end).toInt();
      Serial.println("Upper Arm Servo Target: " + String(targetPos[2]));
    }
    if (header.indexOf("wristSlider=") >= 0) {
      int pos = header.indexOf("wristSlider=") + strlen("wristSlider=");
      int end = header.indexOf("&", pos);
      targetPos[3] = header.substring(pos, end).toInt();
      Serial.println("Wrist Servo Target: " + String(targetPos[3]));
    }
    if (header.indexOf("fingerSlider=") >= 0) {
      int pos = header.indexOf("fingerSlider=") + strlen("fingerSlider=");
      int end = header.indexOf("&", pos);
      targetPos[4] = header.substring(pos, end).toInt();
      Serial.println("Finger Servo Target: " + String(targetPos[4]));
    }
    if (header.indexOf("forearmSlider=") >= 0) {
      int pos = header.indexOf("forearmSlider=") + strlen("forearmSlider=");
      int end = header.indexOf("&", pos);
      targetPos[5] = header.substring(pos, end).toInt();
      Serial.println("Forearm Servo Target: " + String(targetPos[5]));
    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
  }

  // Update each servo position incrementally
  unsigned long currentMillis = millis();
  for (int i = 0; i < numServos; i++) {
    if (currentMillis - lastUpdateMillis[i] >= interval) {
      updateServo(servos[i], currentPos[i], targetPos[i], servoSpeed[i], interval);
    }
  }
}






// Function to smoothly move a servo to a target position
void updateServo(Servo &servo, int &currentPos, int targetPos, float speed, unsigned long interval) {
  unsigned long currentMillis = millis();
  static unsigned long lastUpdateMillis[numServos] = {0}; // One for each servo

  if (currentMillis - lastUpdateMillis[&servo - &servos[0]] >= interval) {
    lastUpdateMillis[&servo - &servos[0]] = currentMillis;

    // Calculate the degrees to move per update cycle
    float moveStep = speed * (interval / 1000.0); // degrees per interval

    // Accumulate movement
    static float moveStepAccumulator[numServos] = {0}; // One for each servo
    int servoIndex = &servo - &servos[0]; // Determine which servo we're updating
    moveStepAccumulator[servoIndex] += moveStep;

    // Increment or decrement the servo position
    while (moveStepAccumulator[servoIndex] >= 1.0) { // Move by at least 1 degree
      if (abs(targetPos - currentPos) < 1) {
        currentPos = targetPos; // Directly set to target if within one degree
        moveStepAccumulator[servoIndex] = 0; // Reset accumulator
        break;
      } else if (targetPos > currentPos) {
        currentPos++;
      } else {
        currentPos--;
      }

      // Clamp the current position to be within valid bounds
      currentPos = constrain(currentPos, 0, 180);

      // Write to servo
      servo.write(currentPos);

      // Decrease the accumulator by the step increment
      moveStepAccumulator[servoIndex] -= 1.0; // We incremented by 1 degree, so decrease by 1
    }
  }
}
