A component that turns application events into sound cues and a bounded history.

Semantic feedback is for one-shot moments that matter to the user, such as a
navigation to a new location, a committed option or a finished or failed
operation, not for ordinary hover, focus, press or progress. Each `emit`
records the event with a sequence number and a monotonic timestamp, maps it
to a `HostSoundCue`, asks the native host to play that cue, and raises
`emitted`. The window's presentation settings decide whether sound is
enabled, and the host decides whether it can play the cue. Without a host the
records are still kept, which makes the feedback traceable in tests.

Parameters
----------
window
    The window whose host plays the cues and whose presentation settings
    decide whether sound is enabled. The window must outlive the component.
clock
    A function returning the current time in nanoseconds, used for the record
    timestamps. When empty, the steady clock is used.

Properties
----------
records
    The recorded events, oldest first.
dropped_record_count : default ``0``
    How many old records have been discarded because the history was full.
maximum_records : default ``1024``
    The size of the history. When it is full, `emit` discards the oldest
    record. It must be from 1 to 65,536, or `std::invalid_argument` is
    thrown; lowering it discards the oldest records at once and counts them
    as dropped.
trace
    The history as text, one line per record, such as
    `feedback=option_committed sequence=2 timestamp=11 cue=success sound=off accepted=false presented=false`.

Events
------
emitted
    Raised by `emit` after the record has been added to the history. It
    carries the new `SemanticFeedbackRecord`.

Methods
-------
emit
    `emit(SemanticFeedbackKind)` records one event and returns its
    `SemanticFeedbackRecord`. The record holds the next sequence number,
    starting at 1; a timestamp from the clock, raised if needed so that
    timestamps always increase; the cue from `cue_for`; whether sound was
    enabled; whether the host accepted the request; and `sound_presented`,
    which is true when both hold. When sound is disabled the host is still
    asked, at zero gain. Throws `std::logic_error` after disposal and when
    called from another thread.
clear
    Empties the history and resets `dropped_record_count`. Sequence numbers
    continue from where they were.
cue_for
    `cue_for(SemanticFeedbackKind)` gives the cue for a kind:
    `location_changed` and `operation_started` play `notification`,
    `pane_changed` and `option_committed` play `success`,
    `operation_completed` plays `operation_complete`, `operation_failed`
    plays `error`, and `conflict` plays `warning`.

See Also
--------
HostServices : The native host's services, which play the cues.

Notes
-----
Sound is enabled when the window's `PresentationSettings::sound_enabled` is
true, which is the default. A record's `host_accepted` and `sound_presented`
are false when the window has no host. The host may merge a cue repeated
within its coalescing window, `HostServices::sound_cue_coalescing_window`,
into one sound.

`emit`, `set_maximum_records` and `clear` must be called on the window's UI
thread. The component keeps a plain pointer to its window: destroy or dispose
it before the window.

Examples
--------
Record the completion of an operation. Without a native host, nothing is
played, but the record is kept:

.. code-block:: cpp

   std::shared_ptr<gf::Control> root = gf::make_control<gf::Control>(gf::StableId("app"));
   gf::Window window(root, gf::Size{200.0, 100.0});
   gf::SemanticFeedback feedback(window);

   gf::SemanticFeedbackRecord done =
       feedback.emit(gf::SemanticFeedbackKind::operation_completed);
   // done.sequence is 1, done.cue is HostSoundCue::operation_complete,
   // and done.host_accepted is false
   std::string history = feedback.trace();

Count failures with an owned handler:

.. code-block:: cpp

   struct FailureCounter final : gf::Component {
       int failures = 0;
       void on_feedback(const gf::SemanticFeedbackRecord& record) {
           if (record.kind == gf::SemanticFeedbackKind::operation_failed) ++failures;
       }
   };

   void example() {
       std::shared_ptr<gf::Control> root = gf::make_control<gf::Control>(gf::StableId("app"));
       gf::Window window(root, gf::Size{200.0, 100.0});
       gf::SemanticFeedback feedback(window);
       FailureCounter counter;
       gf::on(feedback.emitted(), counter, &FailureCounter::on_feedback);
       static_cast<void>(feedback.emit(gf::SemanticFeedbackKind::operation_failed));
       // counter.failures is 1
   }
