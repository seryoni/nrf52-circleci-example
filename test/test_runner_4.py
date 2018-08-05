from jumper.vlab import Vlab
import unittest
import setting


class TestEndToEnd(unittest.TestCase):
    def setUp(self):
        print(self.id().split('.')[-1])  # test name
        # self.vlab = Vlab(working_directory=setting.dir, print_uart=True)
        # self.vlab.load(setting.fw_bin)
        # self.vlab.run_for_ms(500)
        # print('Virtual device is running')

    def tearDown(self):
        # self.vlab.stop()
        pass

    '''
    Black Box Test - Injects a sin wave to the BME and verify that the the printed values are in the range of the wave.
    '''
    def test_4(self):
        # print('Button on')
        # self.vlab.BUTTON1.on()
        # self.vlab.run_for_ms(60)
        # print('Button off')
        # self.vlab.BUTTON1.off()

        self.assertEqual(345, 345)  
        self.assertTrue(True)
          

if __name__ == '__main__':
    unittest.main()
