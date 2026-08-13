const int LDR_PIN     = 0;
const int LED_PIN     = 19;
const int BUZZER_PIN  = 18;

const int TRIG_PIN    = 4;
const int ECHO_PIN    = 5;

// Adjust this after calibration
int threshold = 2000;

//----------------------------
// Measure Distance of the object from the laser 5
//----------------------------
float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
    return -1;

  float distance = duration * 0.0343 / 2.0;

  return distance;
}

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("--------------------------------");
  Serial.println(" LASER SECURITY SYSTEM STARTED ");
  Serial.println("--------------------------------");
}

void loop()
{
  int ldrValue = analogRead(LDR_PIN);

  Serial.print("LDR Value : ");
  Serial.println(ldrValue);

  // Change < to > if your readings are reversed
  if (ldrValue < threshold)
  {
    float distance = getDistance();

    Serial.println("***************");
    Serial.println("INTRUDER DETECTED");

    if (distance > 0)
    {
      Serial.print("Distance : ");
      Serial.print(distance);
      Serial.println(" cm");
    }
    else
    {
      Serial.println("Distance : Out of Range");
    }

    Serial.println("***************");

    // LED ON
    digitalWrite(LED_PIN, HIGH);

    // Buzzer ON
    digitalWrite(BUZZER_PIN, HIGH);

    delay(250);

    // LED OFF
    digitalWrite(LED_PIN, LOW);

    // Buzzer OFF
    digitalWrite(BUZZER_PIN, LOW);

    delay(250);
  }
  else
  {
    float distance = getDistance();

    Serial.println("SYSTEM SAFE");

    if (distance > 0)
    {
      Serial.print("Current Distance : ");
      Serial.print(distance);
      Serial.println(" cm");
    }

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    delay(200);
  }
}