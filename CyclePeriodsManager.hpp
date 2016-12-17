
#ifndef __NANO_BERT_CYCLE_TIMER_H__
#define __NANO_BERT_CYCLE_TIMER_H__


template<class TimeEnum>
class CycleTimesManager
{
  public:

    uint16_t cycle_period_ms;
    uint16_t curr_clock_cycle;

    uint16_t periods[(uint8_t) TimeEnum::cycle_names_final];
    uint16_t last_cycle_clock[(uint8_t) TimeEnum::cycle_names_final];

    CycleTimesManager(uint16_t base_period_ms) {
      this->cycle_period_ms = base_period_ms;

      uint16_t curr_cycle = tock();
      for (size_t idx = 0; idx < (uint8_t) TimeEnum::cycle_names_final; ++idx) {
        last_cycle_clock[idx] = curr_cycle;
        periods[idx] = 0;
      }
    }

    uint16_t tock() {
      return millis() / this->cycle_period_ms;
    }

    void set_period_ms(TimeEnum cycle_name, uint16_t time_ms) {
      this->periods[cycle_name] = time_ms / this->cycle_period_ms;
    }

    bool is_period_up_for(TimeEnum cycle_name) {
      uint16_t clocks_names_last_cycle = this->last_cycle_clock[cycle_name];
      uint16_t clocks_names_period = this->periods[cycle_name];

      if (clocks_names_period == 0)
        return true; // Always return true if cycle_names period is unset

      if (clocks_names_last_cycle + clocks_names_period < tock())
      {
        return true;
      } else {
        return false;
      }
    }
};


#endif
