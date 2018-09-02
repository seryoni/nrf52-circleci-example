# NRF52 + CircleCI Sample Project

This is a sample project that shown a Nordic nrf52832 MCU, connected to the following peripherals:
1. ATWINC1500 Wi-Fi controller (SPI)
2. Button (GPIO)
3. Bosch BME280 environmental sensor (I2C)

When the button is pressed, the device sends the data to a gateway which sends the data to an AWS IoT shadow device.

This sample project demonstrates tests written in Jumper Virtual Lab and how they can be used for continuous integration with CircleCI.

## Setup Instructions
### Prerequisites
- Arm Embedded Toolchain (arm-none-eabi-gcc)
- Python + pip
- AWS account
- AWS Command Line Interface
- CircleCI account

### AWS Setup
Note: Use the "US East (N. Virginia)" ("us-east-1") region for AWS services
- Create a thing on AWS IoT and name it "my_temp_sensor": Follow this [Getting Started](https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html) guide to register a device, activate it and download certificates for the device (No need to attach policies or configure your device).
- Save the certificates files under nrf52-circleci-example/tcp-server/my_temp_sensor.cert.pem and nrf52-circleci-example/tcp-server/my_temp_sensor.private.key
- In nrf52-circleci-example/tcp-server/aws-temprature.py, change the `clientId` variable to the client "thing ARN" from your thing's Detail section an the AWS console.
- Create a policy with "Action" set to "iot.*", "Resouce ARN" set to "*" and mark the "Allow" tick.
- Attach the newly created policy to your certificates.

### Local setup
- Create a GitHub fork of this repository and clone it to your local environment.
- Go to the the local repo and run: `pip install -r requirements.txt`

### Connect to CircleCI
- Create a context in CircleCI called "atwinc-aws-demo".
- Set the following environmental variables for the context:

  VLAB_USER_CONFIG: Contents of ~/.jumper/config.json

  AWS_ACCESS_KEY_ID: Value from ~/.aws/credentials
  
  AWS_SECRET_ACCESS_KEY: Value from ~/.aws/credentials
  
  TEMP_SENSOR_CERT: Value of `cat tcp-server/my_temp_sensor.cert.pem | base64`
  
  TEMP_SENSOR_KEY: Value of `cat tcp-server/my_temp_sensor.private.key | base64`

- Add the project to CircleCI

## Running the Tests
Execute this command to run the tests: `nosetests --rednose -sxv test/`
