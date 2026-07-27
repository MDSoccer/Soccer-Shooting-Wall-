#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

Adafruit_NeoPixel strip(NUM_ZONES * LEDS_PER_ZONE, PIN_LED_DATA, NEO_GRB + NEO_KHZ800);

unsigned long lockoutUntil[NUM_ZONES] = {0};
unsigned long ledOffAt[NUM_ZONES] = {0};

bool windowOpen = false;
unsigned long windowStart = 0;
int windowWinnerZone = -1;
int windowWinnerVal = 0;

void setMuxChannel(uint8_t channel) {
  digitalWrite(PIN_MUX_S0, channel & 0x01);
  digitalWrite(PIN_MUX_S1, (channel >> 1) & 0x01);
  digitalWrite(PIN_MUX_S2, (channel >> 2) & 0x01);
  digitalWrite(PIN_MUX_S3, (channel >> 3) & 0x01);
}

int readZone(uint8_t zone) {
  setMuxChannel(zone);
  delayMicroseconds(5); // mux propagation delay before the signal settles
  return analogRead(PIN_MUX_SIG);
}

void setZoneColor(uint8_t zone, uint32_t color) {
  for (uint8_t i = 0; i < LEDS_PER_ZONE; i++) {
    strip.setPixelColor(zone * LEDS_PER_ZONE + i, color);
  }
}

void registerHit(uint8_t zone, int adcValue) {
  unsigned long now = millis();
  lockoutUntil[zone] = now + LOCKOUT_MS;
  ledOffAt[zone] = now + LED_FLASH_MS;
  setZoneColor(zone, strip.Color(255, 255, 255));
  strip.show();
  Serial.printf("HIT zone=%u adc=%d\n", zone, adcValue);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_MUX_S0, OUTPUT);
  pinMode(PIN_MUX_S1, OUTPUT);
  pinMode(PIN_MUX_S2, OUTPUT);
  pinMode(PIN_MUX_S3, OUTPUT);

  analogReadResolution(ADC_RESOLUTION_BITS);

  strip.begin();
  strip.setBrightness(80);
  strip.show();
}

void loop() {
  unsigned long now = millis();

  for (uint8_t zone = 0; zone < NUM_ZONES; zone++) {
    if (lockoutUntil[zone] > now) continue;

    int value = readZone(zone);
    if (value < ADC_HIT_THRESHOLD) continue;

    if (!windowOpen) {
      windowOpen = true;
      windowStart = now;
      windowWinnerZone = zone;
      windowWinnerVal = value;
    } else if (value > windowWinnerVal) {
      windowWinnerZone = zone;
      windowWinnerVal = value;
    }
  }

  // Keep the comparison window open briefly so simultaneous/adjacent panel
  // strikes resolve to a single winner instead of double-registering.
  if (windowOpen && (now - windowStart >= SCAN_WINDOW_MS)) {
    registerHit((uint8_t)windowWinnerZone, windowWinnerVal);
    windowOpen = false;
    windowWinnerZone = -1;
    windowWinnerVal = 0;
  }

  bool needsShow = false;
  for (uint8_t zone = 0; zone < NUM_ZONES; zone++) {
    if (ledOffAt[zone] != 0 && now >= ledOffAt[zone]) {
      setZoneColor(zone, strip.Color(0, 0, 0));
      ledOffAt[zone] = 0;
      needsShow = true;
    }
  }
  if (needsShow) strip.show();
}
