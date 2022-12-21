from bluepy import btle
import pigpio, time

PWM_GPIO=12 # PWM pin to which ESP32 is connected

# Using pigpio library for generating 10Hz 50% duty cycle PWM signal
pi = pigpio.pi()
pi.set_mode(PWM_GPIO, pigpio.OUTPUT)

pi.set_PWM_frequency(PWM_GPIO,10)
pi.set_PWM_dutycycle(PWM_GPIO, 128)

with open("Data\Exp2_02_4A.txt", "w") as f:
#with open("Data\Exp2_10_66.txt", "w") as f:
    while True:
        class MyDelegate(btle.DefaultDelegate):
            def __init__(self):
                btle.DefaultDelegate.__init__(self)

            def handleNotification(self, cHandle, data):
                # Get the Raspberry Pi time immediately after receiving the data and log it to a text file along with the data read over BLE
                time_RPi = time.time()
                f.write(str(data)[2:-1] + ", " + str(time_RPi) + "\n")

        # Waits till the Raspberry Pi connects to ESP32 over BLE
        while True:
            try:
                p = btle.Peripheral("FC:F5:C4:00:02:4A") # Replace it with the MAC address of the ESP32 to be connected over BLE
                #p = btle.Peripheral("FC:F5:C4:00:10:66")
                print("Connected")
                break
            except btle.BTLEDisconnectError:
                print("Connecting to ESP32 BLE...")
                continue 

        # Reading the data over BLE
        svc = p.getServiceByUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
        ch_Rx = svc.getCharacteristics("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")[0]

        p.setDelegate(MyDelegate())
        setup_data = b"\x01\00"
        p.writeCharacteristic(ch_Rx.valHandle+1, setup_data)

        while True:
            try:
                ch_Rx.read()
            except btle.BTLEException:
                print("btle.BTLEException")
                break
