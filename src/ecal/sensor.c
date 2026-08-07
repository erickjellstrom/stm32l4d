
#include "stm32l4xx.h"
#include "i2c_sens.h"
#include "timer.h"


extern float temperature_celsius;
extern int16_t raw_x;
extern int16_t raw_y;
extern int16_t raw_z;
extern int16_t raw_t;

extern uint8_t accel_buffer[];
extern uint8_t temp_buffer[];


// Divide by 8.0f as specified by the HTS221 factory data datasheet
static float T0_degC;
static float T1_degC;

// Reconstruct the true signed 16-bit internal raw calibration counts
static int16_t T0_OUT;
static int16_t T1_OUT;

void sensor_temp_read_bm(void)
{
    i2c_sens_read_reg(0x5f, 0xAA, temp_buffer, 2);


    // Assemble the signed 16-bit internal counter value from the low and high byte components
    raw_t = (int16_t)((temp_buffer[1] << 8) | temp_buffer[0]);

    // Prevent division-by-zero crashes using our verified factory values
    if (T1_OUT != T0_OUT)
    {
        // Apply the direct linear interpolation formula mapping raw counts to Celsius
        temperature_celsius = (((T1_degC - T0_degC) * (float)(raw_t - T0_OUT)) / (float)(T1_OUT - T0_OUT)) + T0_degC;
    }

}
void sensor_temp_init_bm(void)
{
    uint8_t hts221_id = 0;

    // Read HTS221 Identity (Register 0x0F, Expected response: 0xBC)
    i2c_sens_read_reg(0x5f, 0x0F, &hts221_id, 1);

    uint8_t t0_degC_x8 = 0;
    uint8_t t1_degC_x8 = 0;
    uint8_t t_msb = 0;

    // Read raw factory calibration registers for temperature targets
    i2c_sens_read_reg(0x5f, 0x32, &t0_degC_x8, 1);
    i2c_sens_read_reg(0x5f, 0x33, &t1_degC_x8, 1);
    i2c_sens_read_reg(0x5f, 0x35, &t_msb, 1);

    uint8_t t0_out_l = 0;
    uint8_t t0_out_h = 0;
    uint8_t t1_out_l = 0;
    uint8_t t1_out_h = 0;

    // Read internal factory ADC counts for T0 and T1
    i2c_sens_read_reg(0x5f, 0x3C, &t0_out_l, 1);
    i2c_sens_read_reg(0x5f, 0x3D, &t0_out_h, 1);
    i2c_sens_read_reg(0x5f, 0x3E, &t1_out_l, 1);
    i2c_sens_read_reg(0x5f, 0x3F, &t1_out_h, 1);

    // Unpack the 2-bit MSB chunks for T0 and T1 target temperatures
    uint16_t T0_degC_int = ((uint16_t)(t_msb & 0x03) << 8) | t0_degC_x8;
    uint16_t T1_degC_int = ((uint16_t)(t_msb & 0x0C) << 6) | t1_degC_x8;

    // Divide by 8.0f as specified by the HTS221 factory data datasheet
    T0_degC = (float)T0_degC_int / 8.0f;
    T1_degC = (float)T1_degC_int / 8.0f;

    // Reconstruct the true signed 16-bit internal raw calibration counts
    T0_OUT = (int16_t)((t0_out_h << 8) | t0_out_l);
    T1_OUT = (int16_t)((t1_out_h << 8) | t1_out_l);


    // Activate HTS221: AV_CONF (0x10) = 0x1B (Averaging: T=16, H=32)
    uint8_t hts221_av_cfg = 0x1B;
    i2c_sens_write_reg(0x5f, 0x10, 1, &hts221_av_cfg, 1);

    // Activate HTS221: CTRL_REG1 (0x20) = 0x81 (PD=1 [Active], ODR=1Hz, BDU=1 [Block Data Update])
    uint8_t hts221_ctrl1_cfg = 0x81;
    i2c_sens_write_reg(0x5f, 0x20, 1, &hts221_ctrl1_cfg, 1);

//    HAL_Delay(20);
    tim2_delay_ms(20);
}

void sensor_scanner_bm(void)
{
    uint8_t scan_result;
    uint8_t rx_val = 0;

    scan_result = i2c_is_device_ready(I2C2, 0x5f, 1);

    // Validate HTS221 (Humidity) -> Expects 0xBC at register 0x0F
    i2c_sens_read_reg(0x5F, 0x0F, &rx_val, 1);
    if (rx_val == 0xBC) { /* HTS221 is present and working! */ }

    scan_result = i2c_is_device_ready(I2C2, 0x6A, 1);

    // Validate LSM6DSL (Acc/Gyro) -> Expects 0x6A at register 0x0F
    i2c_sens_read_reg(0x6A, 0x0F, &rx_val, 1);
    if (rx_val == 0x6A) { /* LSM6DSL is present and working! */ }

    scan_result = i2c_is_device_ready(I2C2, 0x5D, 1);
    
    // Validate LPS22HB (Pressure) -> Expects 0xB1 at register 0x0F
    i2c_sens_read_reg(0x5D, 0x0F, &rx_val, 1);
    if (rx_val == 0xB1) { /* LPS22HB is present and working! */ }

    scan_result = i2c_is_device_ready(I2C2, 0x1E, 1);

    // Validate LIS3MDL (Magnetometer) -> Expects 0x3D at register 0x0F
    i2c_sens_read_reg(0x1E, 0x0F, &rx_val, 1);
    if (rx_val == 0x3D) { /* LIS3MDL is present and working! */ }

}


void sensor_imu_init_bm(void)
{
    // Activate LSM6DSL: CTRL3_C (0x12) = 0x44 (BDU=1, IF_INC=1)
    uint8_t lsm6dsl_ctrl3_cfg = 0x44;
    i2c_sens_write_reg(0x6A, 0x12, 1, &lsm6dsl_ctrl3_cfg, 1);
    //HAL_StatusTypeDef act_status3 = HAL_I2C_Mem_Write(&hi2c2, 0xD4, 0x12, I2C_MEMADD_SIZE_8BIT, &lsm6dsl_ctrl3_cfg, 1, 100);
    HAL_Delay(10);

    // Activate LSM6DSL: CTRL1_XL (0x10) = 0x40 (104Hz Output Data Rate, +/-2g range)
    uint8_t lsm6dsl_ctrl1_cfg = 0x40;
    i2c_sens_write_reg(0x6A, 0x10, 1, &lsm6dsl_ctrl1_cfg, 1);
    //HAL_StatusTypeDef act_status4 = HAL_I2C_Mem_Write(&hi2c2, 0xD4, 0x10, I2C_MEMADD_SIZE_8BIT, &lsm6dsl_ctrl1_cfg, 1, 100);
    HAL_Delay(20);

}

void sensor_imu_read_bm(void)
{
    i2c_sens_read_reg(0x6A, 0x28, accel_buffer, 6);

    // Map every independent array buffer index to its exact 16-bit physical axis
    raw_x = (int16_t)((accel_buffer[1] << 8) | accel_buffer[0]);
    raw_y = (int16_t)((accel_buffer[3] << 8) | accel_buffer[2]);
    raw_z = (int16_t)((accel_buffer[5] << 8) | accel_buffer[4]);

    // Convert the raw integers into physical milli-g units using the +/-2g sensitivity constant
    float accel_x_mg = raw_x * 0.061f;
    float accel_y_mg = raw_y * 0.061f;
    float accel_z_mg = raw_z * 0.061f;
}
