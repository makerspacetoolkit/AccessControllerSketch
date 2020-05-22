// AccessControllerSketch  © 2015 Peter Hartmann 
// Part of Maker Space Tool Kit.  Arduino code for Leonardo preferably.
// Arduino has a relay shield, proto shield and is connected to a local 
// server running mstk_doord. Relays are connected to mag-locks and a 
// power supply. Request to exit buttons are connected to analog pins 
// and ground.   

// Long presses turn off for 4 hours AKA Meetup mode. 
// Another long press reengages the lock out of Meetup mode.


int ledPin = 13;
int door1pin = 4;
int door2pin = 5;
int button1pin = A0;
int button2pin = A1;
int THRESHOLD = 40;
unsigned long timer; // the timer
unsigned long INTERVAL = 1000; // the repeat interval
long door1disable = 0;
long door2disable = 0;
int door1entry = 0;
int door2entry = 0;
long serialprevious = 0;
long serailinterval = 4;  // serailinterval at which to blink (econds)
int incomingByte = 0;

int button1state;
int button2state;
byte button1previous = HIGH;
byte button2previous = HIGH;
unsigned long door1firstTime;   // how long since the button was first pressed
unsigned long door2firstTime;
int door1holdcount;   // How long the button was held (secs)
int door2holdcount;


void setup() {
  pinMode(ledPin, OUTPUT); // initialize LED output
  pinMode(button1pin, INPUT_PULLUP);
  pinMode(button2pin, INPUT_PULLUP);
  Serial.begin(9600);
  timer = millis(); // start timer
  pinMode(door1pin, OUTPUT);
  pinMode(door2pin, OUTPUT);
  digitalWrite(door1pin, HIGH);
  delay(500);
  digitalWrite(door1pin, LOW);
  delay(500);
  digitalWrite(door1pin, HIGH);
  delay(500);
  digitalWrite(door1pin, LOW);
}

void loop() {

  if ((millis() - timer) > INTERVAL) {
    // timed out
    timer += INTERVAL;// reset timer by moving it along to the next interval
    //    toggle led

    if ( door1disable > 0 ) {
      Serial.println(door1disable);
      door1disable--;
    }
    else if (door1entry > 0 ) {
      Serial.println(door1entry);
      door1entry--;
    } else {

      digitalWrite(door1pin, LOW);
    }

    if ( door2disable > 0 ) {
      Serial.println(door2disable);
      door2disable--;
    }
    else if (door2entry > 0 ) {
      Serial.println(door2entry);
      door2entry--;
    } else {

      digitalWrite(door2pin, LOW);
    }

  }   // end 1 secc interval events

  unsigned long serialcurrent = millis();

  // send data only when you receive data:
  if (serialcurrent - serialprevious > serailinterval) {

    serialprevious = serialcurrent;

    if (Serial.available() > 0) {

      // read the incoming byte:
      incomingByte = Serial.read();

      if (incomingByte == '1') {
        digitalWrite(door1pin, HIGH);
        door1entry = 7;

      }
      else if (incomingByte == '2') {
        digitalWrite(door2pin, HIGH);
        door2entry = 7;
      }


    }
  }


  button1state = (analogRead(button1pin) < THRESHOLD) ? LOW : HIGH;
  if (button1state == LOW && button1previous == HIGH && millis() - door1firstTime > 100 ) { //  && door1entry > 0
    Serial.println("short press 1");
    digitalWrite(door1pin, HIGH);
    door1entry = 7;
    door1firstTime = millis();    // if the buttons becomes press remember the time
  }

  if (button1state == LOW && ((millis() - door1firstTime) % 1000) < 20 && millis() - door1firstTime > 500 && door1holdcount <= 4) {
    ledblink(1, 50, ledPin); // Each second the button is held blink the indicator led and
    door1holdcount++;                // and 1 to the door1holdcounter
    Serial.println(door1holdcount);
  }
  if (button1state == HIGH && button1previous == LOW &&  door1holdcount <= 4) {

    door1holdcount = 0; // When the button is released if the door1holdcounter is between the
  }                          // two numbers (3-6 blinks or secs) run the program


  if (  door1holdcount >= 4 ) {
    ledblink(10, 200, ledPin);
    Serial.println("long press 1");

    if ( door1disable > 0 ) {
      // door liock wqs already disabled.  re-locking low.
      Serial.println("re-locking");
      door1disable = 0;

      digitalWrite(door1pin, LOW);
      delay(350);
      digitalWrite(door1pin, HIGH);
      delay(350);
      digitalWrite(door1pin, LOW);

    } else {
      // meetup mode
      Serial.print("meetup mode!");
      digitalWrite(door1pin, HIGH);
      delay(350);
      digitalWrite(door1pin, LOW);
      delay(350);
      digitalWrite(door1pin, HIGH);
      delay(350);
      digitalWrite(door1pin, LOW);
      delay(350);
      digitalWrite(door1pin, HIGH);
      door1disable = 14400;
    }
    door1holdcount = 0;
  }

  if (button1state == HIGH) { // reset the counter if the button is not pressed
    door1holdcount = 0;
  }

  button1previous = button1state;

  button2state = (analogRead(button2pin) < THRESHOLD) ? LOW : HIGH;
  if (button2state == LOW && button2previous == HIGH && millis() - door2firstTime > 100 ) { //  && door2entry > 0
    Serial.println("short press 2");
    digitalWrite(door2pin, HIGH);
    door2entry = 7;
    door2firstTime = millis();    // if the buttons becomes press remember the time
  }

  if (button2state == LOW && ((millis() - door2firstTime) % 1000) < 20 && millis() - door2firstTime > 500 && door2holdcount <= 4) {
    ledblink(1, 50, ledPin); // Each second the button is held blink the indicator led and
    door2holdcount++;                // and 1 to the door2holdcounter
    Serial.println(door2holdcount);
  }
  if (button2state == HIGH && button2previous == LOW &&  door2holdcount <= 4) {

    door2holdcount = 0; // When the button is released if the door2holdcounter is between the
  }                          // two numbers (3-6 blinks or secs) run the program


  if (  door2holdcount >= 4 ) {
    ledblink(10, 200, ledPin);
    Serial.println("long press 2");
    if ( door2disable > 0 ) {
      door2disable = 0;

      digitalWrite(door2pin, LOW);
      delay(350);
      digitalWrite(door2pin, HIGH);
      delay(350);
      digitalWrite(door2pin, LOW);


    } else {
      digitalWrite(door2pin, HIGH);
      delay(350);
      digitalWrite(door2pin, LOW);
      delay(350);
      digitalWrite(door2pin, HIGH);
      delay(350);
      digitalWrite(door2pin, LOW);
      delay(350);
      digitalWrite(door2pin, HIGH);
      door2disable = 14400;
    }
    door2holdcount = 0;
  }

  if (button2state == HIGH) { // reset the counter if the button is not pressed
    door2holdcount = 0;
  }

  button2previous = button2state;

}


void ledblink(int times, int lengthms, int pinnum) {
  for (int x = 0; x < times; x++) {
    digitalWrite(pinnum, HIGH);
    delay (lengthms);
    digitalWrite(pinnum, LOW);
    delay(lengthms);
  }
}

