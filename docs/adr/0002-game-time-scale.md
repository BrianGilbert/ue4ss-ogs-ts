# 2. Movement is timed with the game's time dilation

Date: 2026-11-26. Status: accepted, confirmed in Hot Wheels Unleashed. In one
popup run from positions only and one with engine velocity, surge stayed
within 8 m/s² while the game slowed, held and sped up again. The same
positions timed by the wall clock give peaks of 134 to 195 m/s².

## Context

The mod timed movement with the wall clock. Hot Wheels Unleashed shows a
keyboard hint by lowering `TimeDilation` in `WorldSettings` from 1 to 0.0001
over about 0.6 s, and raising it again afterwards. A logged run matches the
pawn's per-frame movement, which shrinks and grows linearly over the same
spans. Position-only dynamics read this as braking and then accelerating, at
about 150 m/s².

## Decision

Read `TimeDilation`, `MatineeTimeDilation` and `DemoPlayTimeDilation` from the
`WorldSettings` that shares the player controller's level, and time movement
for the velocity check and the dynamics with their product. The value read with
one sample times the movement seen at the next. Below 0.02 nothing is derived
and the last values are held. The stream clock, send rate and timestamps stay
on the wall clock.

On a recorded popup, recalculated from positions with the shared C# class, peak
surge was 172 m/s² on the wall clock and 6 with this rule. One frame of delay
was needed: no delay, two frames or the opposite direction gave peaks from 149
to over 200,000 m/s².

A detector for a suddenly still pose was rejected. The pose doesn't stop
suddenly, so it would act only after the braking had been sent, and it could
swallow a real impact.

## Consequences

- A game that slows or holds its simulation some other way isn't covered.
- A sample that spans several game frames pairs them all with one value.
- An actor's own `CustomTimeDilation` isn't applied. It stayed at 1 in the
  logged run.
- The game isn't reported as paused during a popup.
- Finding `WorldSettings` walks the object array once for each new player
  controller.
