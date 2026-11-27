# 4. A motion frame may go up to a quarter of an interval early

Date: 2026-11-27. Status: accepted, confirmed in Hot Wheels Unleashed. With both
changes below, a drive had no gaps of 28 to 45 ms and held 59.95 Hz outside
longer stalls. The recording before had 69 such gaps.

## Decision

A motion frame is sent on a tick that arrives up to a quarter of an interval
before its due time. Without this, a host running close to the motion rate
skipped frames and sent less than the declared rate. The reasons, measurements
and consequences are in `ogs-ts-dotnet`'s
`docs/adr/0003-motion-frames-may-go-early.md`.

The smoke test sends over loopback at frame rates from 59 to 144 with uneven
frame times and counts the motion frames received.

## The mod's sampling gate follows the same schedule

The publisher's change alone didn't help in Hot Wheels: the drive after it still
had 109 gaps of about 32 ms. The mod's gate was the cause there.

The mod samples the pawn from the `ProcessEvent` hook, which fires many times a
frame. It used its own gate: one full interval since the last sample. In a Hot
Wheels Unleashed timing log the calls came in short bursts, with about 15.3 ms
of silence between them at 60 frames a second. Each sample then fell a little
later in the burst, until a whole burst came too soon and the next sample waited
for the one after, about 32 ms after the last. The recording had about three such
gaps a second.

The mod now asks the publisher whether a sample is due, so there is one
schedule. It still waits at least three quarters of an interval between
samples, which limits the object search to 80 a second when there is no pawn.

The smoke test calls the publisher the way the hook does, in a 1.4 ms burst
each frame, at 60 to 144 frames a second. A model of the same bursts gives
54 Hz at 90 frames a second and 50 Hz at 100.
