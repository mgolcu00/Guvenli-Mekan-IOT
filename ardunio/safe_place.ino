#include <ESP8266WiFi.h>
#include <time.h>
#include <FirebaseArduino.h>

#define WIFI_SSID "TurkTelekom_T52F8"
#define WIFI_PASSWORD "mert7591"
#define FIREBASE_HOST "safe-place-gm-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "92tC7movxLoHxMGxJwwE4km1unnf4vxJNGUtMrip"

//defines pins
const int echo_pin = 12;//D6
const int trig_pin = 13;//D7
const int ir_pin = 5;//D1

int c_control = 0;

//about time
int timezone = 3;
int dst = 0;
#define TIME_BUF 256
int first_enter_ir = 0;

int t=0;
char t_buf [TIME_BUF];

void setup()
{
  Serial.begin(9600);

  pinMode(ir_pin, INPUT); //kizilotesi sensor girisi
  pinMode(trig_pin, OUTPUT); // Sets the trig_pin as an Output
  pinMode(echo_pin, INPUT); // Sets the echo_pin as an Input

  Serial.println("Start");

  wifiConnect();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  delay(10);
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void loop()
{


  time_t now;
  struct tm *tminfo;
  time(&now);
  tminfo = localtime(&now);
  //  Serial.println(ctime(&now));

  strftime(t_buf, TIME_BUF, "%d-%m-%Y", tminfo);



  int dis = distance();
  Serial.print("dis : ");
  Serial.println(dis);
  int ir_val = ir_sensor();
  Serial.print("ir : ");
  Serial.println(ir_val);
  //  if (control_dis(dis) && !ir_val)
  //    first_enter_ir = 2;
  //  else if (!control_dis(dis) && ir_val)
  //    first_enter_ir = 1;
  //error handler

  //  if (dis > 400 || dis < 0) {
  //    Firebase.setInt("errors/dis_sensor", 1);
  //  } else {
  //    Firebase.setInt("errors/dis_sensor", 0);
  //  }
  //  if (ir_val < 0 || ir_val > 2) {
  //    Firebase.setInt("errors/ir_sensor", 1);
  //  } else {
  //    Firebase.setInt("errors/ir_sensor", 0);
  //  }


  //error handler


  Firebase.setInt("distance", dis);
  //Hata ayiklama
  if (Firebase.failed()) {
    Serial.print("setting /distance failed:");
    Serial.println(Firebase.error());
  }
  //  delay(10);
  Firebase.setInt("ir", ir_val);
  //Hata ayiklama
  if (Firebase.failed()) {
    Serial.print("setting /ir failed:");
    Serial.println(Firebase.error());
  }


  int c = control(ir_val, dis);

  Serial.println(first_enter_ir);
  if (c == 1 && c_control == 1) {

    c_control = 0;
    int temp = 0;
    if (t==0 ) {
      temp = Firebase.getInt("live/giris");
      temp += c;
      Firebase.setInt("live/giris", temp);
      t=1;
    }
    else if (t==1) {
      temp = Firebase.getInt("live/cikis");
      temp += c;
      Firebase.setInt("live/cikis", temp);
      t=0;
    } else {
      temp = Firebase.getInt("live/giris");
      temp += c;
      Firebase.setInt("live/giris", temp);
    }



    //-------------------------------------------------------
    char  data_path[TIME_BUF];
    strncat(data_path, "data/", 6);
    strncat(data_path, t_buf, strlen(t_buf));
    strncat(data_path, "/kisi_sayisi", 13);
    Firebase.setInt(data_path, temp);
    //-------------------------------------------------------


    if (Firebase.failed()) {
      Serial.print("setting /giris failed:");
      Serial.println(Firebase.error());
    }

  } else {
    if (c == 0) {
      c_control = 1;
    }
  }
  // control





  // calculate kisi sayisi

  //  int g = Firebase.getInt("live/giris");
  //  int ck =  Firebase.getInt("live/cikis");
  //  g -= ck;
  //  Firebase.setInt("live/kisi_sayisi", g);


  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Tekrar bağlantı deneniyor");
    wifiConnect();
  }

}

int control_dis(int dis) {
  return dis < 50 && dis > 0;
}
int control(int ir, int dis) {
  if (dis > 0 && dis < 45 ) {
    first_enter_ir = 2;
    return 1;
  }
  if (ir == 1) {
    first_enter_ir = 1;
    return 1;
  }
  return 0;
}
int distance() {
  long duration = 0;
  int distance = 0;
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);

  // Sets the trig_pin on HIGH state for 10 micro seconds
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  // Reads the echo_pin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo_pin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  //  Serial.print("Distance: ");
  //  Serial.println(distance);
  return distance;
}

int ir_sensor() {
  int val = 0;
  val = digitalRead(ir_pin); // read input value
  return val;
}

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++teller); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}
