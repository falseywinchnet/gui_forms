A one-way flag that asks background work to stop.

A cancellation flag starts clear. Any thread may `request` cancellation, and
the work polls `requested` at points where it can stop cleanly. A `Worker`
owns one flag and passes it to its entry function; you can also use a flag
of your own, for example with `AudioClip::load_ogg`. Cancellation is
cooperative: nothing is interrupted, the work decides when to look.

Properties
----------
requested
    Whether cancellation has been requested. Safe to read from any thread.
    Once it returns true, every write the requesting thread made before
    calling `request` is visible to the reader.

Methods
-------
request
    Requests cancellation. The request is permanent; calling it again does
    nothing. Safe to call from any thread, and never throws.

See Also
--------
Worker : Runs one function on a background thread and hands it a flag.
AtomicThreadPool : Runs batches of short tasks across threads.

Notes
-----
A request only sets the flag. It does not interrupt blocking I/O, unlock a
mutex or wake a thread waiting on a condition variable, so work that blocks
needs its own wake-up or timeout as well. Check the flag at bounded steps,
for example once per loop iteration or per decoded block.

A flag cannot be copied or moved, and there is no way to clear it: start new
work with a new flag.

Examples
--------
A flag on its own:

.. code-block:: cpp

   gf::CancellationFlag flag;
   bool before = flag.requested();   // false
   flag.request();
   flag.request();                   // repeating a request is harmless
   bool after = flag.requested();    // true
   static_cast<void>(before);
   static_cast<void>(after);

Work that checks the flag between steps and stops early when asked:

.. code-block:: cpp

   struct Scan final {
       int visited = 0;   // read by the owner only after join
   };

   void scan_items(const gf::CancellationFlag& cancel, void* context) {
       Scan& scan = *static_cast<Scan*>(context);
       for (int item = 0; item < 1000000 && !cancel.requested(); ++item) {
           ++scan.visited;
       }
   }

   void example() {
       Scan scan;
       gf::Worker worker(scan_items, &scan);
       worker.request_cancel();   // the loop stops at its next check
       worker.join();             // scan.visited is between 0 and 1000000
   }
