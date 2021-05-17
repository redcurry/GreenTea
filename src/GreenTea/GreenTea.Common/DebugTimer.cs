using System.Collections.Generic;
using System.Diagnostics;

namespace GreenTea.Common
{
    // Being static allows it to Start/Stop the same timer from anywhere
    public static class DebugTimer
    {
        private static readonly Dictionary<string, Stopwatch> Timers =
            new Dictionary<string, Stopwatch>();

        public static void Start(string id)
        {
            Timers[id] = Stopwatch.StartNew();
        }

        public static void Stop(string id)
        {
            if (Timers.TryGetValue(id, out var timer))
            {
                timer.Stop();
                var elapsed = timer.ElapsedMilliseconds;
                Debug.WriteLine($"{elapsed} ms\t{id}");
            }
        }

        public static void Pause(string id)
        {
            if (Timers.TryGetValue(id, out var timer))
            {
                timer.Stop();
            }
        }

        public static void Continue(string id)
        {
            if (Timers.TryGetValue(id, out var timer))
            {
                timer.Start();
            }
        }
    }
}