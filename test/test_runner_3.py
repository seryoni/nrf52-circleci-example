from jumper.vlab import Vlab
import unittest
import setting
import boto3
import json
import os
import re
from time import sleep
import subprocess

here = os.path.dirname(os.path.abspath(__file__))
client = boto3.client('iot-data', region_name='us-east-1')

class TestEndToEnd(unittest.TestCase):
    def setUp(self):
        print(self.id().split('.')[-1])  # test name
        tcp_server_dir = os.path.join(here, '..', 'tcp-server')
        self.gateway_process = subprocess.Popen(['python', 'aws-temprature.py'], cwd=tcp_server_dir)
        sleep(2)
        self.vlab = Vlab(working_directory=setting.dir, print_uart=True)
        self.vlab.load(setting.fw_bin_aws)
        self.uart = self.vlab.uart
        self.vlab.run_for_ms(500)
        print('Virtual device is running')

    def tearDown(self):
        self.vlab.stop()
        self.gateway_process.terminate()
        self.gateway_process.wait()

    def push_button(self):
        print('Button on')
        self.vlab.BUTTON1.on()
        self.vlab.run_for_ms(60)
        print('Button off')
        self.vlab.BUTTON1.off()
        self.vlab.run_for_ms(1000)

    def read_from_aws(self):
        response = client.get_thing_shadow(thingName='my_temp_sensor')
        streaming_body = response["payload"]
        raw_data_bytes = streaming_body.read()
        raw_data_string = raw_data_bytes.decode('utf-8')  # Python 3.x specific
        json_state = json.loads(raw_data_string)
        return json_state['state']['desired']['temprature']

    '''
    Integration Test
    Set temperature, click a button and verify that the shadow device on AWS was updated with the same temperature
    '''
    def test_3_Integration_Test(self):
        for i in range(3):
            self.push_button()
            match = re.search('(Temperature: )(\d{2})', self.uart.read())
            temp_uart = int(match.group(2))
            success = False
            for j in range(60):
                try:
                    temp_aws = int(self.read_from_aws())
                    print('Temperature from aws: ' + str(temp_aws))
                    success = temp_uart == temp_aws
                    if success:
                        break
                except Exception as e:
                    pass
                sleep(1)
            self.assertTrue(20 <= temp_aws <= 40)
            self.assertTrue(success)


if __name__ == '__main__':
    unittest.main()
