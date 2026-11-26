# 1. Body dynamics use the engine's velocity

Date: 2026-11-26. Status: accepted, confirmed in Hot Wheels Unleashed. In a
recording of 1,143 moving frames, no surge, sway or heave value changed by
more than 100 m/s² between frames. Working the same positions out the old way
gives 225 such surge changes. The pose froze once, the log shows two checks
skipped, and the source stayed on the engine. A later recording held two
keyboard hints for 8.9 s and 11.1 s with the still pose never counted: the
source stayed on the engine, and surge stayed under 1 m/s² during both.

## Context

The mod worked out velocity from position changes divided by wall-clock time.
The game moves the pawn in its own update steps, which don't line up with that
clock. A Hot Wheels Unleashed recording shows the car at a steady 68 m/s
reading 135 m/s for one sample and 36 m/s for another. Differentiating again
turned this into surge spikes of several hundred m/s², above 3 g in 38% of
frames. Running the same calculation on the recorded poses reproduced the sent
values, and 99% of the large jumps were at samples where time and distance
disagreed.

## Decision

Read `GetVelocity` from the pawn through `ProcessEvent` and pass it to the
dynamics calculation. A timing mismatch then distorts only a velocity change, so
the error scales with real acceleration instead of speed.

Custom vehicle physics can leave `GetVelocity` at zero or in another frame.
Every 0.5 s the publisher compares the distance the pawn moved with the
distance the reported velocity explains. A check fails when more than half the
path, plus 0.5 m, is unexplained. Each change of source restarts the
derivation.

**Three failures in a row** are needed before position deltas take over, and
one passing check brings the engine reading back. One failure was enough at first.
In a Hot Wheels Unleashed recording the source switched to position three times,
and 22 of the 24 surge jumps over 100 m/s² while moving fell in two short bursts of the
position calculation. A reading that's really broken keeps failing, so it still loses,
about a second later. Until then its dynamics are wrong.

**A frozen pose isn't evidence** against a velocity. A failed check in which
the pawn didn't move at all is never counted, however long that lasts: a game
can hold its simulation without reporting a pause, as Hot Wheels Unleashed
does while it shows a keyboard hint, and the velocity stays as it was. A
failed check isn't counted either when the pawn moved less than a tenth of the
distance the velocity explains. In a Hot Wheels Unleashed recording the pose stopped for about
1.5 s while `GetVelocity` still read 83 m/s. The mod switched to position,
and when the pose moved again it read that as accelerating from rest, with
surge up to 877 m/s². Replaying the check on that recording with every window
alignment, the rule without this exception switched in all 30 and with it in
none. The replay held the velocity at the logged reading. At most six of these slightly moving checks in a row
are skipped. Without that limit a velocity more than ten times too big would
never be rejected.

**Only movement restores trust.** A passing check counts when the pawn moved
more than 1 m in it, the distance at which a zero velocity fails. A parked
pawn passes any velocity that reads zero, so without this a reading stuck at
zero was trusted again at every stop.

**A failed read changes the source too.** The calculation restarts whenever
velocity switches between the engine and position, including for a single
missed read. Differencing a velocity from one source against the other gave a
680 m/s² spike in a test with uneven physics steps.

The time step stays wall-clock. Game time wouldn't help a game that steps
physics apart from its frames, and changing one thing at a time shows what the
velocity reading fixes. A new in-game recording decides whether it needs to
change.

## Consequences

- A game whose `GetVelocity` fails the comparison keeps the noisy position
  path. `ogs_telemetry_debug.log` shows which source is in use.
- `uevr-ogs-ts` already passes engine velocity and doesn't have this
  comparison.
