#include <CAN.h>

#define TX_GPIO_NUM   17  // Connects to CTX
#define RX_GPIO_NUM   16  // Connects to CRX

// Define CAN ID for the message you expect to receive
const uint32_t CAN_ID = 0x11;

struct CanMessage {
  uint8_t driveMode;
  uint8_t throttleValue;
  uint8_t steeringAngle;
  uint8_t driverSignalRecieved;
};


struct CanRecieveMessage {
  bool extended;
  bool rtr;
  uint32_t id;
  uint8_t length;
  uint8_t data[8];
  uint8_t driverReady;
  uint8_t throttleValue;
};

int driverReady = 0;
int flag = 0;


void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(1000);

  Serial.println("CAN Sender");

  // Set the pins for CAN communication
  CAN.setPins(RX_GPIO_NUM, TX_GPIO_NUM);

  // Start the CAN bus at 1 Mbps (adjust as needed)
  if (!CAN.begin(1E6)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  else {
    Serial.println("CAN Initialized");
  }
}

int canReciever(){
  int packetSize = CAN.parsePacket();

  if (packetSize) {

    CanRecieveMessage recvMsg;


    if (CAN.packetExtended()) {
      // recieve packet
      recvMsg.extended = CAN.packetExtended();
      recvMsg.rtr = CAN.packetRtr();
      recvMsg.id = CAN.packetId();
      recvMsg.length = CAN.packetDlc();

      // read packet data into the struct
      for (int i = 0; i < packetSize; i++){
        recvMsg.data[i] = CAN.read();
      }

      recvMsg.driverReader = recvMsg.data[0]
      recvMsg.driverReader = recvMsg.data[1]
      return recvMsg;

    }else {
      driverReady = CAN.read();
      return driverReady;
    }
  }
}

void canSender(uint8_t driveMode, float throttleValue, float steeringAngle, uint8_t driverSignalRecieved) {
  CanMessage msg;
  msg.driveMode = driveMode;
  msg.throttleValue = throttleValue;
  msg.steeringAngle = steeringAngle;
  msg.driverSignalRecieved = driverSignalRecieved;

  CAN.beginPacket(0x12);  // Set CAN ID (adjust as needed)
  CAN.write((uint8_t*)&msg, sizeof(msg));  // Write message data
  CAN.endPacket();  // Send the packet

  Serial.println("Sent CAN message");
}

void loop() {
  // Example: Send drive mode 2, throttle value 0.5, and steering angle -0.2
  Serial.println("waiting for driver");
  while(driverReady != 1 && flag == 0){
    driverReady = canReciever();
    if (driverReady == 1){ flag = 1; canSender(2, 160, 60,1);}
  }
  Serial.println("driver ready!");
  
  delay(10000);
  canSender(2, 160, 60,1);
  delay(5000);  // Adjust delay as needed between sending messages
  canSender(2, 150, 120,1);
  delay(5000);
  canSender(2, 120, 90,1);
  delay(10000);
  canSender(1, 120, 90,1);
  //huhu
}
