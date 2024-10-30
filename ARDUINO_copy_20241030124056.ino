#include <WiFi.h>
#include <ESP_Mail_Client.h>


#define WIFI_SSID "OnePlus Nord2 5G"
#define WIFI_PASSWORD "v8g8c4qr"


#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465


#define sender_email "abc625507@gmail.com"
#define sender_password "pigszxqgzpmodoar"


#define Recipient_email "vashishth.patel2021@gmail.com"
#define Recipient_name ""


// Sound sensor pin
#define SOUND_SENSOR_PIN 34


// Create an SMTP session object
SMTPSession smtp;


void setup() {
    Serial.begin(9600);
    Serial.println();


    // Configure the sound sensor pin as input
    pinMode(SOUND_SENSOR_PIN, INPUT);


    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
    }
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());


    // Set up time synchronization with NTP
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("Waiting for time synchronization...");
    while (!time(nullptr)) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nTime synchronized.");


    // Enable debug mode for the SMTP client
    smtp.debug(1);
}


void loop() {
    // Read the value from the sound sensor
    int sensorValue = analogRead(SOUND_SENSOR_PIN);
    Serial.print("Sound Sensor Value: ");
    Serial.println(sensorValue);


    // Check if the sensor value is above the threshold
    if (sensorValue > 2800) {
        // Configure the SMTP session
        ESP_Mail_Session session;
        session.server.host_name = SMTP_server;
        session.server.port = SMTP_Port;
        session.login.email = sender_email;
        session.login.password = sender_password;
        session.login.user_domain = "";


        // Declare the message class
        SMTP_Message message;
        message.sender.name = "ESP32";
        message.sender.email = sender_email;
        message.subject = "Alert: Sound Detected";
        message.addRecipient(Recipient_name, Recipient_email);


        // Set the message content
        String textMsg = "Alert! The sound sensor detected a noise level above the threshold.";
        message.text.content = textMsg.c_str();
        message.text.charSet = "us-ascii";
        message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;


        // Try connecting to the SMTP server
        if (!smtp.connect(&session)) {
            Serial.println("Failed to connect to the SMTP server.");
            return;
        }


        // Send the email
        if (!MailClient.sendMail(&smtp, &message)) {
            Serial.println("Error sending Email: " + smtp.errorReason());
        } else {
            Serial.println("Alert email sent successfully.");
        }


        // Terminate the SMTP session
        smtp.closeSession();


        // Wait for a while before sending another alert
        delay(60000); // 1 minute delay to avoid spamming emails
    }


    // Small delay before the next reading
    delay(1000);
}

