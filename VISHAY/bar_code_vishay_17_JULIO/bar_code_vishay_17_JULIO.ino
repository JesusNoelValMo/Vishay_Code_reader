#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Timer.h"
LiquidCrystal_I2C lcd(0x27, 20, 4);
Timer t;
Timer t2;
int cuenta;
String val;
String a;
String placa;
String placa_lcd;
int LED_OK = 19;
int LED_BAD = 18;
int LED_BAD2 = 17;
//int resetcount= 7;
String c(50);
String c_lcd;
String c_lcd2;
String codigoSub;
bool isFinished = false;
bool ledblinkk = false;
bool toggle= false;

bool greenledblinkk = false;
bool togglegreen= false;
class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
  // Serial.println((char)len);
  if (buf[2] == 1 || buf[2] == 0) return;

  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(i > 2, buf, buf[i]);
    }

    return;
  }
}

uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters
  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    if (upper) return (key - 4 + 'A');
    else return (key - 4 + 'a');
  }

  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
  }

  return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  uint8_t ascii = KeyToAscii(upper, mod, key);
  Serial.println(ascii);
  // char a =  (char)ascii;
  //String b =
  //Serial.print((char)ascii); 
  
  if (((ascii>=48  )&&(ascii<=57)) || ((ascii>=65 )
  && (ascii<=90 ))|| ((ascii>=97 )&&(ascii<=122 ))) {
 val +=  (char)ascii;

  }
  else {
   

  }


}

void MyParser::OnScanFinished() {
  String e = (String)val;
  //  Serial.print(val.substring(0,4));
  isFinished = true;
  Serial.println(val);
  Serial.println(val.length());
  //val= "";
  //  c = val;
  Serial.println("-Finished");

}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;

void setup() {

 
  t.oscillate(13, 1000, LOW);
  t.every(1000, timeoutcount); 
  t2.oscillate(13, 1000, LOW);
  t2.every(1000, ledblink);
  lcd.init();
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_BAD, OUTPUT);
  pinMode(LED_BAD2, OUTPUT);

  //APAGA LEDS Y PRENDE RELE DE BOTON STOP
   digitalWrite(LED_BAD, HIGH);
  digitalWrite(LED_BAD2, HIGH);
  digitalWrite(LED_OK, HIGH);

  //PIN PARA RESET DE TIMEOUT
  pinMode(7, INPUT);
  Serial.begin( 115200 );
  Serial.println("Start");
  cuenta = 0;
  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }

  delay( 200 );

  Hid.SetReportParser(0, &Parser);
 lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ESCANEE TARJETA");
      greenledblinkk = true;
}

void loop() {


  String s = "wp00r033";
t2.update();

  //  a = a + "1";
  // Serial.println(a);
  if (isFinished == true) {
    isFinished = false;

    if ((val.startsWith("w"))&&(val.length() == 7)||(val.startsWith("w"))&&(val.length() == 8)||(val.startsWith("w"))&&(val.length() == 9)){
      ledblinkk=false;
greenledblinkk = false;
      //quita boton de stop
  digitalWrite(LED_BAD2, LOW);

//LED BAD OFF
   digitalWrite(LED_BAD, HIGH);

   //APAGA VERDE
    digitalWrite(LED_OK, HIGH);
      // String valtrim = "";
      val.replace(" ", "");
      // Serial.print(val);
      c = val;
      c.toUpperCase();
      c_lcd = c.substring(0, 4);
      c_lcd2 = c.substring(4, val.length());

      c.toLowerCase();



      //Serial.print("asd");
      Serial.println(val);
      Serial.println(c);
      val = "";
    
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print(c_lcd);
      lcd.setCursor(6, 0);
      lcd.print(c_lcd2);
      cuenta=0;
      while (1) {
if (digitalRead(7)==HIGH){
  cuenta=0;
}

        
        t.update();
        if (cuenta > 60) {
          ledblinkk=true;
          
           ledblink();
         //SI SE CUMPLW TIMEOUT SE PARA LA MAQUINA   
         digitalWrite(LED_BAD2, HIGH);
          digitalWrite(LED_OK, HIGH);   // turn the LED on (HIGH is the voltage level)
          lcd.backlight();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("ESCANEE TARJETA");
          lcd.setCursor(3, 1);
          lcd.print("OTRA VEZ");
          
          break;
        }
        if (isFinished == true) {
          isFinished = false;
          if ((val.length() == 5) || (val.length() == 4) || (val.length() == 3)) {
            cuenta=0;
            placa = val;
            val;
            placa.toUpperCase();
            placa_lcd = placa;
            placa.toLowerCase();

            lcd.backlight();
            lcd.setCursor(1, 1);
            lcd.print(placa_lcd);
            cuenta = 0;
            Serial.print("asd2");

            codigoSub =  c.substring(4);

            if (placa == codigoSub) {
              Serial.println("PZ Buena");
              codigoSub = "";
              val = "";

              lcd.backlight();
              lcd.setCursor(13, 0);
              lcd.print("OK");

              digitalWrite(LED_BAD, HIGH);
              digitalWrite(LED_BAD2, LOW);
              digitalWrite(LED_OK, LOW);   // turn the LED on (HIGH is the voltage level)


            }
            else {
              Serial.print("Codigo Dif");

              Serial.println(placa);
              Serial.println(codigoSub);
              val = "";
              codigoSub = "";
              c = "";
              lcd.backlight();
              lcd.clear();
              lcd.setCursor(2, 0);
              lcd.print("ADVERTENCIA!!");
              lcd.setCursor(1, 1);
              lcd.print("PLACA ERRONEA");

              digitalWrite(LED_BAD, LOW);
                 digitalWrite(LED_OK, HIGH);
              digitalWrite(LED_BAD2, HIGH);
              //codigo = "";
              break;
            }

          }

          else if ((val.startsWith("w"))&&(val.length() == 7)||(val.startsWith("w"))&&(val.length() == 8)||(val.startsWith("w"))&&(val.length() == 9)) {
            cuenta=0;
            digitalWrite(LED_BAD, HIGH);
            //Etiqueta de cierre.
            if (val == c) {
              Serial.println("FIN");
              val = "";
              c = "";
              placa = "";
              codigoSub = "";
              lcd.backlight();
              lcd.clear();
              lcd.setCursor(1, 0);
              lcd.print("ORDEN COMPLETA");
               digitalWrite(LED_BAD2, HIGH);
             
               delay(2000);
                 lcd.backlight();
              lcd.clear();
              lcd.setCursor(1, 0);
                greenledblinkk = true;
                 lcd.print("ESCANEE TARJETA");
              break;

            }
            else {
              Serial.println("TARJETA EQUIVOCADA");
              val = "";
              lcd.clear();
              lcd.setCursor(1, 0);
              lcd.print("TARJETA EQUIV.");
            //  break;

              codigoSub = "";
            }

          }
          else {
         
//            Serial.print("Tamanio dif");
//            Serial.print(placa);
//            Serial.print(codigoSub);
//            codigoSub = "";
//
//            //codigo = "";
           val = "";
//            break;
          }


        }
        //Serial.print("1");
        Usb.Task();
      }
      //codigo = "";
    }
    else {
      // val = "";
      val = "";
      placa = "";
      c = "";
      //  codigo = "";

    }
    //  if (placa == codigo){
    //
    //    Serial.println(codigo);
    //    Serial.println("OK");
    //    Serial.println(val);
    //    Serial.println(val.length());
    //    val = "";
    //   // delay(300);
    //    isFinished = false;
    //}
    //
    //
    //else {
    //   Serial.println(val);
    //   Serial.println("BAD");
    //   val = "";
    // //  delay(300);
    //   isFinished = false;
    //}
  }
  Usb.Task();
}
void timeoutcount()
{
  cuenta++;
}
void ledblink()
{
 
if (ledblinkk == true){
  toggle= !toggle;
   digitalWrite(LED_BAD, toggle);
        
       
    
  
  }
  if (greenledblinkk == true){
  togglegreen= !togglegreen;
   digitalWrite(LED_OK, togglegreen);
        
       
    
  
  }
           

}


