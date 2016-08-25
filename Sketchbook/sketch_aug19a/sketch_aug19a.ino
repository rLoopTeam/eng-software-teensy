void setup() {
  // put your setup code here, to run once:
pinMode(13,OUTPUT);


}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13,1);
  delay(1000);
  digitalWrite(13,0);
  delay(1000);
  Serial.println("Hello");

  Serial.print("OSC_CR: ");
  uint8_t value = *((uint8_t*)0x40065000);
  Serial.println(value);

  Serial.print("SIM_CLDIV1: ");
  uint32_t value2 = *((uint32_t*)0x40048044);
  Serial.println(value);

  Serial.print("SIM_CLDIV2: ");
  value2 = *((uint32_t*)0x40048048);
  Serial.println(value);

  Serial.print("MCG_C1: ");
  value = *((uint8_t*)0x40064000);
  Serial.println(value);
  Serial.print("MCG_C2: ");
  value = *((uint8_t*)0x40064001);
  Serial.println(value);
  Serial.print("MCG_C3: ");
  value = *((uint8_t*)0x40064002);
  Serial.println(value);
  Serial.print("MCG_C4: ");
  value = *((uint8_t*)0x40064003);
  Serial.println(value);
  Serial.print("MCG_C5: ");
  value = *((uint8_t*)0x40064004);
  Serial.println(value);
  Serial.print("MCG_C6: ");
  value = *((uint8_t*)0x40064005);
  Serial.println(value);
  Serial.print("MCG_C7: ");
  value = *((uint8_t*)0x4006400C);
  Serial.println(value);
  Serial.print("MCG_C8: ");
  value = *((uint8_t*)0x4006400D);
  Serial.println(value);
}
