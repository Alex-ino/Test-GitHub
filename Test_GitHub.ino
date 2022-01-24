#include <ESP8266WiFi.h>
#include <FS.h>

#ifndef STASSID
#define STASSID "xxxxxxxxx"
#define STAPSK "xxxxxxxxx"
#endif

// A single, global CertStore which can be used by all
// connections.  Needs to stay live the entire time any of
// the WiFiClientBearSSLs are present.
#include <CertStoreBearSSL.h>
BearSSL::CertStore certStore;

/* Set up values for your repository and binary names */
#define GHOTA_USER "Alex-ino"
#define GHOTA_REPO "Test-GitHub"
#define GHOTA_CURRENT_TAG "0.0.2"
#define GHOTA_BIN_FILE "Test_GitHub.ino.nodemcu.bin"
#define GHOTA_ACCEPT_PRERELEASE 0
#define HARDWARE_LED 2  /// GPIO02 /// D4 /// change according where your ESP8266 board LED is connected to


#include <ESP_OTA_GitHub.h>
void handle_upgade() {
  // Initialise Update Code
  //We do this locally so that the memory used is freed when the function exists.
  ESPOTAGitHub ESPOTAGitHub(&certStore, GHOTA_USER, GHOTA_REPO, GHOTA_CURRENT_TAG, GHOTA_BIN_FILE, GHOTA_ACCEPT_PRERELEASE);

  Serial.println("Checking for update...");
  if (ESPOTAGitHub.checkUpgrade()) {
    Serial.print("Upgrade found at: ");
    Serial.println(ESPOTAGitHub.getUpgradeURL());
    if (ESPOTAGitHub.doUpgrade()) {
      Serial.println("Upgrade complete."); //This should never be seen as the device should restart on successful upgrade.
    } else {
      Serial.print("Unable to upgrade: ");
      Serial.println(ESPOTAGitHub.getLastError());
    }
  } else {
    Serial.print("Not proceeding to upgrade: ");
    Serial.println(ESPOTAGitHub.getLastError());
  }
}

void setup() {
  // Start serial for debugging (not used by library, just this sketch).
  Serial.begin(115200);

  // Start SPIFFS and retrieve certificates.
  SPIFFS.begin();
  int numCerts = certStore.initCertStore(SPIFFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.print(F("Number of CA certs read: "));
  Serial.println(numCerts);
  if (numCerts == 0) {
    Serial.println(F("No certs found. Did you run certs-from-mozill.py and upload the SPIFFS directory before running?"));
    return; // Can't connect to anything w/o certs!
  }

  // Connect to WiFi
  Serial.print("Connecting to WiFi... ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.print("... ");
  }
  Serial.println();

  /* This is the actual code to check and upgrade */
  handle_upgade();
  /* End of check and upgrade code */

  // Your setup code goes here
  pinMode(HARDWARE_LED, OUTPUT);
}

void loop () {
  digitalWrite(HARDWARE_LED, HIGH);
  delay(1000);
  digitalWrite(HARDWARE_LED, LOW);
  delay(1000);

}
