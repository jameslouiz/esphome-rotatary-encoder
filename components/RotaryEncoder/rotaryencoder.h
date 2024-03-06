#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace esphome
{
  namespace rotaryencoder
  {

    class RotaryEncoder : public i2c::I2CDevice, public Component
    {
    public:
      void setup() override;
      void loop() override;

      float get_setup_priority() const override;

      void readButton();
      void readEncoder();

      void setupPin(uint8_t buf);

      void setupButton()
      {
        setupPin(GPIO_DIRCLR_BULK);
        setupPin(GPIO_PULLENSET);
        setupPin(GPIO_BULK_SET);
      }

      void set_encoder(sensor::Sensor *encoder) { this->encoder_value_ = encoder; }

      void set_button(binary_sensor::BinarySensor *button)
      {
        this->button_ = button;
      }

      void add_on_clockwise_callback(std::function<void()> callback)
      {
        this->on_clockwise_callback_.add(std::move(callback));
      }

      void add_on_anticlockwise_callback(std::function<void()> callback)
      {
        this->on_anticlockwise_callback_.add(std::move(callback));
      }

    protected:
      uint8_t number_{0};
      int32_t value_{0};
      int encoder_filter_ = 1;
      int32_t min_value_{INT32_MIN};
      int32_t max_value_{INT32_MAX};
      long long lastUpdated = 8;
      static const uint8_t GPIO_BASE = 0x01;
      static const uint8_t GPIO_DIRCLR_BULK = 0x03;
      static const uint8_t GPIO_PULLENSET = 0x0B;
      static const uint8_t GPIO_BULK_SET = 0x05;
      static const uint8_t ENCODER_BASE = 0x11;
      CallbackManager<void()> on_clockwise_callback_;
      CallbackManager<void()> on_anticlockwise_callback_;

      sensor::Sensor *encoder_value_{nullptr};
      sensor::Sensor *increment_value_{nullptr};
      binary_sensor::BinarySensor *button_{nullptr};

      void setEncoderValue(int32_t value);
    };

    class RotaryEncoderClockwiseTrigger : public Trigger<>
    {
    public:
      explicit RotaryEncoderClockwiseTrigger(RotaryEncoder *parent)
      {
        parent->add_on_clockwise_callback([this]()
                                          { this->trigger(); });
      }
    };

    class RotaryEncoderAntiClockwiseTrigger : public Trigger<>
    {
    public:
      explicit RotaryEncoderAntiClockwiseTrigger(RotaryEncoder *parent)
      {
        parent->add_on_anticlockwise_callback([this]()
                                              { this->trigger(); });
      }
    };

  } // namespace rotaryencoder
} // namespace esphome
