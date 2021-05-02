namespace Purl {
  namespace Events {
    class TimesTrigger {
      public:
        TimesTrigger(int times, int resetMillis);
        typedef void (*TimesOutEvent)();
        TimesOutEvent onTimesOut;
        void count();
      private:
        int _times;
        int _resetMillis;
        // state
        int _count              = 0;
        unsigned long _lastTime = 0;
    };
  }
}
