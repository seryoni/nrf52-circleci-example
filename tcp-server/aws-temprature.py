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
certificatePath = 'my_temp_sensor.cert.pem'
privateKeyPath = 'my_temp_sensor.private.key'
port = 8883
useWebsocket = False
thingName = 'my_temp_sensor'
clientId = 'arn:aws:iot:us-east-1:617413614608:thing/my_temp_sensor'
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
    TCP_IP = '0.0.0.0'
    TCP_PORT = 5005

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)    
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)
  #  s.settimeout(10)

    try:
        while True:
            print "Waiting for client"
            conn, addr = s.accept()
            print "Client connected!"
            connected = True

            while connected:
                try:
                    data=b''
                    while (len(data) < 2):
                        new_data = conn.recv(2 - len(data))
                        if new_data == b'':
                            print("Client disconnected")
                            connected = False
                            break
                        data += new_data

                    if (len(data) >= 2):
                        print "got 2 bytes"
                        val = str(data.strip('\0'))
                        print "received temprature value from device:", str(val)
                        JSONPayload = json.dumps(dict(
                            state=dict(
                                desired=dict(temprature=str(val))
                                )
                            ))
                        # JSONPayload = '{"state":{"desired":{"temprature":"' + str(val) + '"}}}'.decode('utf-8')
                        print(JSONPayload)
                        deviceShadowHandler.shadowUpdate(JSONPayload, customShadowCallback_Update, 5)
                        print "Data was sent to AWS"
                    sleep(1)
                except Exception as e:
                    print("Received exception:")
                    print(e)
    except KeyboardInterrupt:
        pass
    
    s.close()

if __name__ == '__main__':
    main()
