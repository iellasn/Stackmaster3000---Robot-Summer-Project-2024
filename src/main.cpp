
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <vector>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define REFLECTANCE_0 PA_2 
#define REFLECTANCE_1 PA_3



/**
 * @brief Object of an IR sensor
 * 
 */
class IrSensor {


  private:
  PinName sensorPin; //pin sensor is attached to
  double lastCorrelationVal = -1; //result of the most recent correlation

  public:

  /**
   * @brief Construct a new Ir Sensor object
   * 
   * @param pin the pin which the ir detector is connected to
   */
  IrSensor(PinName pin) : sensorPin(pin) {}

  /**
   * @brief Get the most recent result of this->crossCorrelation().
   * 
   * @return double most recent result of crossCorrelation.
   * returns -1 if crossCorrelation() has not been called yet
   */
  double getLastCorrelationVal(){

    return this->lastCorrelationVal;

  }


  /**
   * @brief measures a signal and cross correlates it with 
   * a 1kHz sine wave. 
   * 
   * @return 'amount' of 1kHz IR signal detected
   */
  double crossCorrelation() {

    std::vector<double> IRsignal;
    int numSamples = 0;
    unsigned long finishTime = 0;
    unsigned long startTime = millis(); 

    /*  Read Values from sensor  */
    while (millis() - startTime < 10){
      IRsignal.push_back(analogRead(this->sensorPin));
      numSamples++;
      finishTime = millis();
    }

    double oneK[2* numSamples] = {0}; //computed sine wave
    double oneKCorr[numSamples] = {0}; //array for convolved result

    /*  calculate period of sine wave  */
    int dt = ( finishTime - startTime );
    double oneKT = (double) numSamples / ( (double) dt );

    /*  Create sine wave  */
    for(int i = 0; i < 2 * numSamples;  i++) {
      oneK[i] = sin(i * TWO_PI / oneKT);
    }

    /*  Convolve measured IR signal with created sin wave  */
    for (int k = 0; k < numSamples; k++){
      oneKCorr[k] = 0;
      for (int i = 0; i < numSamples; i++){      
        oneKCorr[k] += IRsignal.at(i) * oneK[k+i];
      }
    }

    /*  Find max in array  */
    double max = oneKCorr[0];
    for (int i=0; i< numSamples; i++) {

      if (oneKCorr[i]>max){
        max = oneKCorr[i];
      }
    }

    this->lastCorrelationVal = max;
    return max;

  }


};



void setup() {
  display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 

  Serial.begin(115200);

  // Displays Adafruit logo by default. call clearDisplay immediately if you don't want this.
  display_handler.display();
  delay(2000);

  // Displays "Hello world!" on the screen
  display_handler.clearDisplay();
  display_handler.setTextSize(1);
  display_handler.setTextColor(SSD1306_WHITE);
  display_handler.setCursor(0,0);
  display_handler.println("Setup");
  display_handler.display();

  
  // Set up reflectance inputs
  pinMode(REFLECTANCE_0, INPUT);
  pinMode(REFLECTANCE_1, INPUT);


}

void loop() {


	int reflectanceReading0 = analogRead(REFLECTANCE_0);
	int reflectanceReading1 = analogRead(REFLECTANCE_1);


	// Displays "Hello world!" on the screen
	display_handler.clearDisplay();
	display_handler.setTextSize(1);
	display_handler.setTextColor(SSD1306_WHITE);
	display_handler.setCursor(0,0);
	display_handler.println("reflectance 0: ");
	display_handler.println(reflectanceReading0);
	display_handler.println("reflectance 1: ");
	display_handler.println(reflectanceReading1);
	display_handler.display();

	delay(100);



}

