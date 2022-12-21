#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define audioSensor A5 // ADC pin to which Audio sensor is connected

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

BaseType_t pxReadTaskWoken;
TaskHandle_t httpRead;

hw_timer_t* timer0 = NULL;
int sensorValue = 0;
std::string timeStamp; 
std::string txValue;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

// Function to be trigerred on software interrupt
void IRAM_ATTR onTimer() {   
    pxReadTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(httpRead, &pxReadTaskWoken);
    if (pxReadTaskWoken == pdTRUE) {
      portYIELD_FROM_ISR();
    }
}

// Task for sending Audio sensor value and ESP32 timestamp over BLE
void httpReadTask(void * parameter) {
    while(true) {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      sensorValue = analogRead(audioSensor);
      timeStamp = std::to_string(esp_timer_get_time());
      pTxCharacteristic->setValue(std::to_string(sensorValue) + ", " + timeStamp);
      pTxCharacteristic->notify();
      portYIELD();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(audioSensor, INPUT); 
 
  BLEDevice::init("UART Service");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic ( CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );               
  pTxCharacteristic->addDescriptor(new BLE2902());
  pTxCharacteristic->setValue(txValue);
  pService->start();  // Start the service
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  //functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read.");

  timer0 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer0, &onTimer, true); // Linking software interrupt and Interrupt function
  timerAlarmWrite(timer0, 1000000, true); // Start the timer to trigger software interrupt every 1000000μs
  timerAlarmEnable(timer0);

  xTaskCreatePinnedToCore(  httpReadTask, // function to implement task
                            "httpRead",   // Task name
                            4096,         // stack size in words
                            NULL,         // arguments
                            2,            // priority
                            &httpRead,    // task handle
                            0 );
}

void loop() {
    while (true);
}
