

#include <SoftwareSerial.h>

#include "WiFiEsp.h"
#include <Servo.h>




int servoPin = 3;
int pirPinSignal = 4;
int pirValue;
int moveCounter = 0;
Servo servo1;
// Create WiFi module object on GPIO pin 6 (RX) and 7 (TX)
SoftwareSerial Serial1(7, 6);

// Declare and initialise global arrays for WiFi settings
char ssid[] = "STI Student";
char pass[] = "STI1924stu";

// Declare and initialise variable for radio status 
int status = WL_IDLE_STATUS;
WiFiEspServer server(80);
// the setup function runs once when you press reset or power the board
void setup() {
	// Initialize serial for debugging
	Serial.begin(115200);

	// Initialize serial for ESP module
	Serial1.begin(9600);

	// Initialize ESP module
	WiFi.init(&Serial1);
	
	// Check for the presence of the shield
	if (WiFi.status() == WL_NO_SHIELD) {
		Serial.println("WiFi shield not present");

		// Don't continue
		while (true);
	}

	// Attempt to connect to WiFi network
	while (status != WL_CONNECTED) {
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);

		// Connect to WPA/WPA2 network
		status = WiFi.begin(ssid, pass);
	}

	Serial.println("You're connected to the network");
	printWifiStatus();
	server.begin();
	//Declares the pir as INPUT at pin 4
	pinMode(pirPinSignal, INPUT);
	//Declares the servo as output at pwm pin 3
	servo1.attach(servoPin);
	
}

// the loop function runs over and over again until power down or reset
void loop() {
	pirValue = digitalRead(pirPinSignal);
	//Serial.println(pirValue);
	if (pirValue == 0) {
		servo1.write(0);
		delay(1000);
		servo1.write(45);
		delay(1000);
		servo1.write(0);
		moveCounter++;
		Serial.println(moveCounter);
	}
	
	

	// Listen for incoming clients
	WiFiEspClient client = server.available();

	if (client) {
		Serial.println("New client");

		// An HTTP request ends with a blank line
		boolean currentLineIsBlank = true;

		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				Serial.write(c);

				// If you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank) {
					Serial.println("Sending response");

					// Send a standard HTTP response header
					client.print(
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html\r\n"
						"Connection: close\r\n"
						"\r\n");
					client.print("<!DOCTYPE HTML>\r\n");
					client.print("<html>\r\n");
					client.print("<head>\r\n");
					client.print("<meta charset=\"utf-8\">");
					client.print("<title>My Arduino</title>\r\n");
					client.print("<link rel=\"stylesheet\"  href=\"styles/MoveItInStyle.css\">");
					client.print("</head>\r\n");
					client.print("<body>\r\n");
					client.print("<h1>Hello World!</h1>\r\n");
					client.print("<h1>Welcome to the Movementfactory!</h1>\r\n");
					client.print("<h4>Number of people that has moved today, so far!</h4>\r\n");
					client.print(moveCounter);
					client.print("</body>\r\n");
					client.print("</html>\r\n");
					break;
				}
				if (c == '\n') {
					// you're starting a new line
					currentLineIsBlank = true;
				}
				else if (c != '\r') {
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}

		// Give the web browser time to receive the data
		delay(10);

		// Close the connection:
		client.stop();
		Serial.println("Client disconnected");
	}


}
void printWifiStatus() {

	// Print the SSID of the network
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// Print the IP address
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
}
