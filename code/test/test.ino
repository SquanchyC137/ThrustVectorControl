
unsigned long time1, time2, time3, time4 = 0;

void setup() {
  Serial.begin(9600);


}

void loop() {
  time3 = time2 - time1;
  time4 = time4 + time3;
  if(time4 >= 3900){
    time4 = 0;
    Serial.print("hurra\n");
  }
  time1 = millis();

  Serial.print(time4);
  Serial.print("\n");
  delay(1000);
  time2 = millis();
}
