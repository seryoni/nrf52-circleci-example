from time import sleep
import sys

from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTShadowClient
import logging
import time
import json
import socket

# Custom Shadow callback
def customShadowCallback_Update(payload, responseStatus, token):
    # payload is a JSON string ready to be parsed using json.loads(...)
    # in both Py2.x and Py3.x
    if responseStatus == "timeout":
        print("Update request " + token + " time out!")
    if responseStatus == "accepted":
        print("Update request with token: " + token + " accepted!")
    if responseStatus == "rejected":
        print("Update request " + token + " rejected!")

host = 'a1rxr3meyz7der.iot.us-east-1.amazonaws.com'
rootCAPath = 'root-CA.crt'
certificatePath = 'TemperatureSensor.cert.pem'
privateKeyPath = 'TemperatureSensor.private.key'
port = 8883
useWebsocket = False
thingName = 'TemperatureSensor'
clientId = 'arn:aws:iot:us-east-1:617413614608:thing/TemperatureSensor'
data = B''
# Init AWSIoTMQTTShadowClient
myAWSIoTMQTTShadowClient = AWSIoTMQTTShadowClient(clientId)
myAWSIoTMQTTShadowClient.configureEndpoint(host, port)
myAWSIoTMQTTShadowClient.configureCredentials(rootCAPath, privateKeyPath, certificatePath)

# AWSIoTMQTTShadowClient configuration
myAWSIoTMQTTShadowClient.configureAutoReconnectBackoffTime(1, 32, 20)
myAWSIoTMQTTShadowClient.configureConnectDisconnectTimeout(10)  # 10 sec
myAWSIoTMQTTShadowClient.configureMQTTOperationTimeout(5)  # 5 sec
MQTTClient = myAWSIoTMQTTShadowClient.getMQTTConnection()
MQTTClient.configureOfflinePublishQueueing(10)
# Connect to AWS IoT
myAWSIoTMQTTShadowClient.connect()

# Create a deviceShadow with persistent subscription
deviceShadowHandler = myAWSIoTMQTTShadowClient.createShadowHandlerWithName(thingName, True)

def main():
    # open tcp server socket
    TCP_IP = '127.0.0.1'
    TCP_PORT = 5005
    BUFFER_SIZE = 8192 

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)
  #  s.settimeout(10)
    print "Waiting for client"
    conn, addr = s.accept()
    print "Client connected!"
    data=''

    try:
        while True:
            data = conn.recv(4)
            if (data):
                val = str(data.strip('\0'))
                print "received temprature value from device:", str(val)
                JSONPayload = '{"state":{"desired":{"temprature":"' + str(val) + '"}}}'.decode('utf-8')
                print JSONPayload
                deviceShadowHandler.shadowUpdate(JSONPayload, customShadowCallback_Update, 5)
                print "Data was sent to AWS"
            else:
                print "got null"
            sleep(1)
    except KeyboardInterrupt:
        pass
    finally:
        print('Disconnecting')

if __name__ == '__main__':
    main()
