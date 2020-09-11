#ifndef VALIANT_TIME_HPP
#define VALIANT_TIME_HPP

namespace valiant {
class Time {
   public:
    const double &delta_time;

    explicit Time(double new_delta_time = 0.)
        : delta_time(delta_time_), delta_time_(new_delta_time) {}

    void set(double new_delta_time) { delta_time_ = new_delta_time; }

   private:
    double delta_time_;
};
}  // namespace valiant

#endif
