import RPi.GPIO as GPIO
import serial, pigpio, time

# Define a serial port for UART communication
ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)

PWM_GPIO = 12 # PWM pin to which ESP32 is connected

# Using pigpio library for generating 10Hz 50% duty cycle PWM signal
pi = pigpio.pi()
pi.set_mode(PWM_GPIO, pigpio.OUTPUT)

pi.set_PWM_frequency(PWM_GPIO,10)
pi.set_PWM_dutycycle(PWM_GPIO, 128)

# Reading the first line from Serial port
while True:
    try:
        intr_t_init = ser.readline()
        break
    except ValueError:
        continue

# Reading from the Serial port and logging it to a text file
with open("Data\Exp1_02_4A.txt", "w") as f:
#with open("Data\Exp1_10_66.txt", "w") as f:
    while True:
        intr_t = int(str(ser.readline())[2:-5])
        f.write(str(intr_t) + "\n")
