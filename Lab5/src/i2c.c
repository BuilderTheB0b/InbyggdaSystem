#include "i2c.h"
#include <driver/i2c.h>



#define ACCEL_XOUT_H        0x3B
#define ACCEL_XOUT_L        0x3C
#define ACCEL_YOUT_H        0x3D
#define ACCEL_YOUT_L        0x3E
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40

#define ACCEL_SENSITIVITY   16384.0 // for ±2g range
/*
From the Datasheet, page 33:
The slave address of the MPU-60X0 is b110100X which is 7 bits long.
The LSB bit of the 7 bit address is determined by the logic level on pin AD0.
This allows two MPU-60X0s to be connected to the same I2C bus.
*/
// If AD0 is LOW, the I2C address of the board will be 0x68. Otherwise, the address will be 0x69.
#define MPU6050_ADDR 0x68

/*
From Datasheet, page 40:
Register (Hex): 6B
Bit7 DEVICE_RESET
Bit6 SLEEP
Bit5 CYCLE
Bit4 -
Bit3 TEMP_DIS
Bit2, Bit1, Bit0 CLKSEL[2:0]
*/
#define MPU6050_PWR_MGMT_1 0x6B

/*
Register (Hex) 19
Bit7, Bit6, Bit5, Bit4, Bit3, Bit2, Bit1, Bit0: SMPLRT_DIV[7:0]
*/
#define MPU6050_SMPLRT_DIV 0x19

/*
From Datasheet, page 45:
Register (Hex): 75
Bit7 -
Bit6, Bit5, Bit4, Bit3, Bit2, Bit1: WHO_AM_I[6:1]
Bit0: -
*/
#define MPU6050_WHO_AM_I 0x75

/*
From Datasheet, page 30:
Register (Hex) 41
Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0: TEMP_OUT[15:8]
Register (Hex) 42
Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0: TEMP_OUT[7:0]
*/
#define MPU6050_TEMP_OUT_H 0x41
#define MPU6050_TEMP_OUT_L 0x42

void initI2C(int sdapin, int sclpin){
    
    i2c_config_t conf = {0};
    esp_err_t res;
    conf.mode = I2C_MODE_MASTER;             // ESP32 acts as master
    conf.sda_io_num = sdapin;                    // pin used for SDA
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE; // The SDA and SCL lines are active low, so they should be pulled up with resistors
    conf.scl_io_num = sclpin;                    // SCL pin number
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE; // enables pullup on SDA
    conf.master.clk_speed = 100000;          // Standard mode (100 Kbit/s)
    // configure I2C controller 0
    res = i2c_param_config(I2C_NUM_0, &conf);
    ESP_ERROR_CHECK(res);
    // install driver, no buffers needed in master mode nor special interrupts config
    res = i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    ESP_ERROR_CHECK(res);

    


    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // start command
    res = i2c_master_start(cmd);
    ESP_ERROR_CHECK(res);
    // set address + write and check for ack
    res = i2c_master_write_byte(cmd, MPU6050_ADDR << 1 | I2C_MASTER_WRITE, 1);
    ESP_ERROR_CHECK(res);
    // write the register address and check for ack
    res = i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, 1);
    ESP_ERROR_CHECK(res);
    // write value of the regiter: 0, and check for ack
    res = i2c_master_write_byte(cmd, 0x00, 1);
    ESP_ERROR_CHECK(res);
    // end of command
    res = i2c_master_stop(cmd);
    ESP_ERROR_CHECK(res);
    // send the command, 1 second timeout
    res = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(res);
    // delete command now that it's not needed
    i2c_cmd_link_delete(cmd);

    // set the sampling frequency
    // the sampling freq is gyro sampling freq / (1 + divider)
    // setting divider to 250 leads to sampling freq. of 32 Hz
    cmd = i2c_cmd_link_create();
    res = i2c_master_start(cmd);
    ESP_ERROR_CHECK(res);
    res = i2c_master_write_byte(cmd, MPU6050_ADDR << 1 | I2C_MASTER_WRITE, 1); // WRITE bit set!
    ESP_ERROR_CHECK(res);
    res = i2c_master_write_byte(cmd, MPU6050_SMPLRT_DIV, 1); // write to SMPLRT_DIV
    ESP_ERROR_CHECK(res);
    res = i2c_master_write_byte(cmd, 0x07, 1); // set SMPLRT_DIV to 250
    ESP_ERROR_CHECK(res);
    res = i2c_master_stop(cmd);
    ESP_ERROR_CHECK(res);
    res = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    // ESP_ERROR_CHECK(res);
    i2c_cmd_link_delete(cmd);
  
}

void writeI2C(uint8_t address, uint8_t reg, uint8_t data){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Start condition
    i2c_master_start(cmd);
    // Send device address with WRITE bit
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    // Send the register address
    i2c_master_write_byte(cmd, reg, true);
    // Send the data byte
    i2c_master_write_byte(cmd, data, true);
    // Stop condition
    i2c_master_stop(cmd);

    // Execute the command with a timeout
    esp_err_t res = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    if (res != ESP_OK) {
        printf("I2C write error: %d\n", res);
    }

    i2c_cmd_link_delete(cmd);
}

void readI2C(uint8_t address, uint8_t reg, uint8_t *buffer, int len){
    i2c_cmd_handle_t cmd;
    esp_err_t res;

    cmd = i2c_cmd_link_create();
    res = i2c_master_start(cmd);
    ESP_ERROR_CHECK(res);
    res = i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_WRITE, 1); // WRITE bit set!
    ESP_ERROR_CHECK(res);
    res = i2c_master_write_byte(cmd, reg, 1); // read high
    ESP_ERROR_CHECK(res);
    res = i2c_master_start(cmd);
    ESP_ERROR_CHECK(res);
    res = i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_READ, 1); // READ bit set!
    ESP_ERROR_CHECK(res);
    res = i2c_master_read(cmd, buffer, 2, I2C_MASTER_NACK);
    ESP_ERROR_CHECK(res);
    res = i2c_master_stop(cmd);
    ESP_ERROR_CHECK(res);
    res = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(res);
    i2c_cmd_link_delete(cmd);
}


float readAccelX() {
    uint8_t buffer[2];
    int16_t raw;
    readI2C(MPU6050_ADDR, ACCEL_XOUT_H, buffer, 2);
    raw = ((int16_t)buffer[0] << 8) | buffer[1];
    return raw / ACCEL_SENSITIVITY;
}

float readAccelY() {
    uint8_t buffer[2];
    int16_t raw;
    readI2C(MPU6050_ADDR, ACCEL_YOUT_H, buffer, 2);
    raw = ((int16_t)buffer[0] << 8) | buffer[1];
    return raw / ACCEL_SENSITIVITY;
}

float readAccelZ() {
    uint8_t buffer[2];
    int16_t raw;
    readI2C(MPU6050_ADDR, ACCEL_ZOUT_H, buffer, 2);
    raw = ((int16_t)buffer[0] << 8) | buffer[1];
    return raw / ACCEL_SENSITIVITY;
}

