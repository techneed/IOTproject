import RPi.GPIO as GPIO
from lib_nrf24 import NRF24
import time
import datetime
import spidev
import MySQLdb

GPIO.setmode(GPIO.BCM)

pipes = [[0xE8, 0xE8, 0xF0, 0xF0, 0xE1], [0xF0, 0xF0, 0xF0, 0xF0, 0xE1]]

radio = NRF24(GPIO, spidev.SpiDev())
radio.begin(0, 19)

radio.setPayloadSize(32)
radio.setChannel(0x76)
radio.setDataRate(NRF24.BR_250KBPS)
radio.setPALevel(NRF24.PA_MIN)

radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.enableAckPayload()

radio.openWritingPipe(pipes[0])
radio.openReadingPipe(1, pipes[1])
radio.printDetails()
# radio.startListening()

db =MySQLdb.connect("localhost","root","thusara","parking")

while(1):
    start = time.time()
    radio.startListening()

    while not radio.available(0):
        time.sleep(1 / 100)
        if time.time() - start > 2:
           # print("Timed out.")
            break

    receivedMessage = []
    radio.read(receivedMessage, radio.getDynamicPayloadSize())
    # print("Received: {}".format(receivedMessage))

    string = ""
    for n in receivedMessage:
        # Decode into standard unicode set
        if (n >= 32 and n <= 126):
            string += chr(n)
    print("Out received message decodes to: {}".format(string))
    slot,stat = string.split("||")
    print ("Slot number:{}".format(slot))
    print ("Slot status Y-occupied N-Free: {}".format(stat))
    cur=db.cursor()
    with db:

        cur.execute("""SELECT * FROM slot WHERE id = %s""", (slot,))
        result = cur.fetchone()
        if result:
            cur.execute('UPDATE slot SET status=%s where id=%s' , ((stat),(slot)))
        if not result:
            cur.execute('insert into slot(id,status)values(%s,%s)',((slot),(stat)))

    radio.stopListening()
    time.sleep(1)
    cur.close()
db.close()
print slot
print stat
