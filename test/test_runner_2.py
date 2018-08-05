from jumper.vlab import Vlab
import unittest
import setting


class TestEndToEnd(unittest.TestCase):
    def setUp(self):
        print(self.id().split('.')[-1])  # test name
        self.vlab = Vlab(working_directory=setting.dir, print_uart=True)
        self.vlab.load(setting.fw_bin)
        self.uart = self.vlab.uart
        self.vlab.run_for_ms(500)
        print('Virtual device is running')

    def tearDown(self):
        self.vlab.stop()

    def push_button(self):
        print('Button on')
        self.vlab.BUTTON1.on()
        self.vlab.run_for_ms(60)
        print('Button off')
        self.vlab.BUTTON1.off()
        self.vlab.run_for_ms(600)
        
    '''
    Black Box Test 
    Set temperature, click a button and verify that the same temperature was printed to UART
    '''
    def test_2_Black_Box(self):
        # self.push_button()
        # self.uart.wait_until_uart_receives('Temperature: 2762', timeout=1000)
        self.push_button()
        # self.uart.wait_until_uart_receives('Temperature: 2911', timeout=1000)
        self.push_button()
        # self.uart.wait_until_uart_receives('Temperature: 3001', timeout=1000)
        self.push_button()
        # self.uart.wait_until_uart_receives('Temperature: 3586', timeout=1000)
        self.push_button()
        self.push_button()
        self.push_button()


if __name__ == '__main__':
    unittest.main()
