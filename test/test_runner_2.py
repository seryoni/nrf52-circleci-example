import os
from jumper.vlab import Vlab
import unittest

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'src', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


class TestEndToEnd(unittest.TestCase):
    def setUp(self):
        print(self.id().split('.')[-1])  # test name
        self.vlab = Vlab(working_directory=dir, print_uart=True)
        self.vlab.load(fw_bin)
        self.uart = self.vlab.uart
        self.vlab.run_for_ms(500)
        print('Virtual device is running')

    def tearDown(self):
        self.vlab.stop()

    '''
    Black Box Test 
    Set temperature, click a button and verify that the same temperature was printed to UART
    '''
    def test_2_Black_Box(self):
        print('Button on')
        self.vlab.BUTTON1.on()
        self.vlab.run_for_ms(60)
        print('Button off')
        self.vlab.BUTTON1.off()
        self.vlab.run_for_ms(600)
        self.assertEqual(self.uart.read(), 'result: 0\r\nTemperature, Pressure, Humidity\r\nTemperature: 2762\r\n')


if __name__ == '__main__':
    unittest.main()
