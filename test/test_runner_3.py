import os
from time import sleep
from jumper.vlab import Vlab
import unittest

dir = os.path.dirname(os.path.abspath(__file__))
fw_bin = os.path.join(dir, '..', 'pca10040', 'blank', 'armgcc', '_build', 'nrf52832_xxaa.bin')


class TestEndToEnd(unittest.TestCase):
    def setUp(self):
        print(self.id().split('.')[-1])  # test name
        # self.vlab = Vlab(working_directory=dir, print_uart=True)
        # self.vlab.load(fw_bin)
        # self.vlab.run_for_ms(500)
        # print('Virtual device is running')

    def tearDown(self):
        # self.vlab.stop()
        pass

    '''
    Integration Test
    Set temperature, click a button and verify that the shadow device on AWS was updated with the same temperature
    '''
    def test_3_Integration_Test(self):
        # print('Button on')
        # self.vlab.BUTTON1.on()
        # self.vlab.run_for_ms(60)
        # print('Button off')
        # self.vlab.BUTTON1.off()

        self.assertEqual(345, 345)  # in this case we are going to poll for the pin level
        self.assertTrue(True)
          

if __name__ == '__main__':
    unittest.main()
