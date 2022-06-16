#include <TinyGPS.h>
#include <LiquidCrystal.h>

int x, y, z, px = 0, py = 0, pz = 0, fire = 0, smoke = 0, accident = 0, rfid = 0;
double x_g_value, y_g_value, z_g_value, roll, pitch;
TinyGPS gps;
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  lcd.begin(20, 4);
  lcd.setCursor(0,1);
  lcd.print("Initializing System");
}

void loop() {
  // put your main code here, to run repeatedly:
  fire = digitalRead(A0);
  smoke = digitalRead(A1);
  x = analogRead(A2);  
  y = analogRead(A3);
  z = analogRead(A4); 

  x_g_value = ( ( ( (double)(x * 3.3)/1024) - 1.65 ) / 0.55 ); /* Acceleration in x-direction in g units */ 
  y_g_value = ( ( ( (double)(y * 3.3)/1024) - 1.65 ) / 0.55 ); /* Acceleration in y-direction in g units */ 
  z_g_value = ( ( ( (double)(z * 3.3)/1024) - 1.65 ) / 0.55 ); /* Acceleration in z-direction in g units */ 

  roll = ( ( (atan2(y_g_value,z_g_value) * 180) / 3.14 ) + 180 ); /* Formula for roll */
  pitch = ( ( (atan2(z_g_value,x_g_value) * 180) / 3.14 ) + 180 ); /* Formula for pitch */
  
  if(px+py+pz > 0 && (abs(x-px)>50 || abs(y-py)>50 || abs(z-pz)>50)) {
    accident = 1;
  }
  
  px = x; py = y; pz = z;
  
  rfid = digitalRead(2);

  bool newData = false;

  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      if (gps.encode(c)) 
        newData = true;  
    }
  }
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Fire:");
  lcd.print(fire?"Yes":"No");
  lcd.print(" Smoke:");
  lcd.print(smoke?"Yes":"No");
  lcd.setCursor(0,1);
  lcd.print("X:");
  lcd.print(x);
  lcd.print(" Y:");
  lcd.print(y);
  lcd.print(" Z:");
  lcd.print(z);

//  lcd.setCursor(0,2);
//  lcd.print("Roll:");
//  lcd.print((int)roll);
//  lcd.print(" Pitch:");
//  lcd.print((int)pitch);

  if(rfid){
    Serial.println("RFID");
    lcd.setCursor(1,3);
    lcd.print("Location via RFID");
  }
  
  if (newData && (fire || smoke || accident))      //If newData is true
  {
    if(accident) {
      lcd.setCursor(0,2);
      lcd.print("  Accident Detected");
    }
    digitalWrite(3, HIGH);
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age); 
    Serial.print("Fire:");
    Serial.print(fire?"Yes":"No");
    Serial.print(", Smoke:");
    Serial.print(smoke?"Yes":"No");
    Serial.print(", Accident:");
    Serial.print(accident?"Yes":"No");
    Serial.print(";");  
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(";");
    Serial.println(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    lcd.setCursor(0,4);
    lcd.print("Lat:");
    lcd.print(abs(flat),1);
    lcd.print(flat>=0?"N":"S");
    lcd.print(" Lon:");
    lcd.print(abs(flon),1);
    lcd.print(flon>=0?"E":"W");
  }
  else {
    digitalWrite(3, LOW);
  }
}
