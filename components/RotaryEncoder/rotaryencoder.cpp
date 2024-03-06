#include "rotaryencoder.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace rotaryencoder
  {

    static const char *const TAG = "ENCODER";

    float RotaryEncoder::get_setup_priority() const
    {
      return setup_priority::IO;
    }

    void RotaryEncoder::setup()
    {
      setupButton();

      LOG_I2C_DEVICE(this);
    }

    void RotaryEncoder::setEncoderValue(int32_t value)
    {
      uint8_t data[4];

      data[0] = 0x36;
      data[1] = 0x23;
      data[2] = 0x55;
      data[3] = 0x66;

      u_int8_t result = this->write_register(ENCODER_BASE, data, 4);

      if (result != i2c::ERROR_OK)
      {
        ESP_LOGE(TAG, "RotaryEncoder encoder zero setup failed");
        this->mark_failed();
      }
    }

    void RotaryEncoder::readButton()
    {
      uint8_t buffer[2] = {0x01, 0x04};
      this->write(buffer, 2);
      uint8_t read_buffer[4];

      this->read(read_buffer, 4);

      int32_t button_state = (read_buffer[0] << 24) |
                             (read_buffer[1] << 16) |
                             (read_buffer[2] << 8) |
                             (read_buffer[3] << 0);

      bool is_pressed = !(1 & (button_state >> 24));

      if (this->button_ != nullptr)
      {
        if ((this->button_->state != is_pressed))
        {
          this->button_->publish_state(is_pressed);
        }
      }
    }

    void RotaryEncoder::readEncoder()
    {
      uint8_t buffer[2] = {0x11, 0x30};
      this->write(buffer, 2);
      uint8_t read_buffer[4];

      delay(8);

      this->read(read_buffer, 4);

      int32_t value = (read_buffer[0] << 24) |
                      (read_buffer[1] << 16) |
                      (read_buffer[2] << 8) |
                      (read_buffer[3] << 0);

      if (this->encoder_value_ != nullptr)
      {
        if (value_ != value)
        {
          // this->encoder_value_->publish_state(value);

          if (value_ > value)
          {
            ESP_LOGI(TAG, "ROTATING CLOCKWISE");
            this->encoder_value_->publish_state(++number_);
          }
          else
          {
            ESP_LOGI(TAG, "ROTATING ANTI-CLOCKWISE");
            this->encoder_value_->publish_state(--number_);
          }

          value_ = value;
        }
      }
    }

    void RotaryEncoder::loop()
    {
      this->readEncoder();
      this->readButton();
    }

  } // namespace rotaryencoder
} // namespace esphome
