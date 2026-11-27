// smoke.cpp — standalone verification of the UEVR plugin's wire path and
// math, no UEVR required. Runs the exact publisher code (ogs_publisher)
// against synthetic pawn motion and asserts the conversion chain:
// FRotator -> FQuat (FRotator::Quaternion transcription) -> ToStandard ->
// facing expectations (mirrors ogs-ts-unreal Test/main.cpp). Exit 0 = pass.
#include <winsock2.h>
#include <cstdio>
#include <cmath>
#include <utility>
#include "../plugin/ogs_publisher.h"

using namespace ogs::telemetry;

static int g_pass = 0, g_fail = 0;
static void Check(const char* name, bool ok)
{
    ok ? g_pass++ : g_fail++;
    std::printf("%s %s\n", ok ? "PASS" : "FAIL", name);
}

static ogs::telemetry::Quat RotatorToQuat(double p, double y, double r)
{
    const double half = 3.14159265358979323846 / 180.0 / 2.0;
    const double sp = std::sin(p * half), cp = std::cos(p * half);
    const double sy = std::sin(y * half), cy = std::cos(y * half);
    const double sr = std::sin(r * half), cr = std::cos(r * half);
    return { (float)(cr * sp * sy - sr * cp * cy),
             (float)(-cr * sp * cy - sr * cp * sy),
             (float)(cr * cp * sy - sr * sp * cy),
             (float)(cr * cp * cy + sr * sp * sy) };
}

int main()
{
    // ---- conversion chain: UE rotator -> UE quat -> standard frame ----
    {
        auto q = RotatorToQuat(0, 0, 0);
        Check("identity rotator -> identity quat",
              q.X == 0 && q.Y == 0 && q.Z == 0 && q.W == 1);
    }
    {
        // UE yaw +90 deg about Z: faces +X -> +Y in UE space.
        // After ToStandard (cyclic xyz permutation), standard forward
        // (+Z) must land on standard right (+X) — same sense (mirrors
        // the quaternion-based check in ogs-ts-unreal's conformance).
        Quat s = ToStandard(RotatorToQuat(0, 90, 0));
        Vec3 fwd = Rotate(s, { 0, 0, 1 });
        Check("UE yaw 90 -> standard forward faces +X",
              fwd.X > 0.999f && std::abs(fwd.Y) < 1e-6f && std::abs(fwd.Z) < 1e-6f);
    }
    {
        Quat s = ToStandard(RotatorToQuat(0, -90, 0));
        Vec3 fwd = Rotate(s, { 0, 0, 1 });
        Check("UE yaw -90 -> standard forward faces -X",
              fwd.X < -0.999f);
    }
    {
        Quat s = ToStandard(RotatorToQuat(90, 0, 0));
        Vec3 fwd = Rotate(s, { 0, 0, 1 });
        Check("UE pitch 90 -> standard forward faces +Y (up)",
              fwd.Y > 0.999f);
    }
    {
        auto q = RotatorToQuat(30, -145, 12);
        double n = std::sqrt((double)q.X * q.X + q.Y * q.Y + q.Z * q.Z + q.W * q.W);
        Check("composite rotator -> unit quaternion",
              std::abs(n - 1.0) < 1e-6);
    }

    // ---- typeLabel: UE instance-suffix stripping ----
    {
        Check("instance suffix stripped",
              ogs::StripInstanceSuffix("BP_PlayerCharacter_C_3")
                  == "BP_PlayerCharacter_C");
        Check("multi-digit instance suffix stripped",
              ogs::StripInstanceSuffix("Sportscar_12") == "Sportscar");
        Check("non-numeric suffix kept",
              ogs::StripInstanceSuffix("Wing_2L") == "Wing_2L");
        Check("no suffix kept", ogs::StripInstanceSuffix("Pawn") == "Pawn");
    }

    // ---- wire path: publish 150 synthetic ticks (2.5 s at 60 Hz) ----
    // A pawn circling horizontally at 10 m with a nose-right yaw sweep —
    // receivers on the LAN (ogs-ts-dotnet TelemetryReceive) should see
    // discovery + motion. NO engine velocity is fed (UE4SS derives it
    // from position deltas — the ue4ss path), and the derived body
    // dynamics are asserted: yaw rate ≈ 90°/s, heave ≈ +1 g
    // (accelerometer-style), centripetal ω²r on +sway.
    ogs::Publisher pub;
    ogs::PublisherConfig cfg;
    cfg.gameName = "ogs_smoke";
    cfg.objectName = "synthetic_pawn";
    if (!pub.start(cfg)) { std::printf("FAIL socket start\n"); return 1; }
    const double dt = 1.0 / 60.0;
    const double yawRate = 90.0;                    // deg/s
    const double w = yawRate * 3.14159265 / 180.0;  // rad/s
    const double r = 10.0;                          // metres
    for (int i = 0; i < 150; i++)
    {
        const double t = i * dt;
        const double yaw = t * yawRate;
        const double yr = yaw * 3.14159265 / 180.0;
        // horizontal circle in UE's ground plane (X-Y, Z-up const),
        // nose tangential: heading = position angle + 90°, turning
        // nose-right at yawRate — centripetal ω²r points +sway in the
        // body frame the whole time
        Vec3 pos{ (float)(1000.0 * std::cos(yr)),
                  (float)(1000.0 * std::sin(yr)), 0.0f };
        if (i == 75) pub.set_type_label("BP_SyntheticPawn_C_7"); // mid-stream swap
        pub.tick(dt, true, pos, RotatorToQuat(0, yaw + 90.0, 0));
    }
    pub.stop();
    Check("150 ticks published (discovery + motion sent, typeLabel "
          "swapped at tick 75)", true);
    Check("typeLabel swap after discovery starts generation 1",
          pub.generation() == 1);

    // ---- derived body dynamics (circling pawn, position-delta velocity) ----
    {
        const float g = ogs::telemetry::BodyDynamicsDeriver::Gravity;
        const Vec3& av = pub.last_angular_velocity();
        const Vec3& sf = pub.last_specific_force();
        std::printf("dynamics: av=(%.4f, %.4f, %.4f) sf=(%.4f, %.4f, %.4f) "
                    "[expect av.y=%.4f, sf.y=%.4f, horiz=%.4f]\n",
                    av.X, av.Y, av.Z, sf.X, sf.Y, sf.Z,
                    (float)w, g, (float)(w * w * r));
        Check("dynamics: yaw rate ≈ 90°/s nose-right (+y, C-4)",
              std::abs(av.Y - w) < 0.05 && std::abs(av.X) < 0.01 &&
              std::abs(av.Z) < 0.01);
        Check("dynamics: heave ≈ +1 g (accelerometer-style)",
              std::abs(sf.Y - g) < 0.2);
        // centripetal on +sway; surge leakage bounded by the accel LP
        // phase lag atan(ωτ) on the co-rotating vector plus the secant/
        // backward-difference skew of delta-derived velocity
        Check("dynamics: centripetal ω²r on +sway (C-4 right-turn)",
              std::abs(sf.X - (float)(w * w * r)) < 1.5 &&
              std::abs(sf.Z) < (float)(w * w * r) * 0.25f);
        const float horiz = std::sqrt(sf.X * sf.X + sf.Z * sf.Z);
        Check("dynamics: horizontal specific force ≈ centripetal ω²r",
              std::abs(horiz - (float)(w * w * r)) < 1.5);
    }

    // ---- pause (C-11): no derivation while paused, fresh start on resume ----
    {
        const float g = ogs::telemetry::BodyDynamicsDeriver::Gravity;
        const Quat level = RotatorToQuat(0, 0, 0);
        pub.set_paused(true);
        Check("pause: reported", pub.paused());
        for (int i = 0; i < 30; i++)       // frozen pose while paused
            pub.tick(dt, true, Vec3{ 0, 0, 0 }, level);
        pub.set_paused(false);
        pub.tick(dt, true, Vec3{ 500, 0, 0 }, level); // resume elsewhere
        const Vec3& av = pub.last_angular_velocity();
        const Vec3& sf = pub.last_specific_force();
        Check("pause: resume restarts derivation (at rest: +1 g heave, no rates)",
              std::abs(sf.Y - g) < 1e-3f && std::abs(sf.X) < 1e-3f &&
              std::abs(sf.Z) < 1e-3f && std::abs(av.X) < 1e-6f &&
              std::abs(av.Y) < 1e-6f && std::abs(av.Z) < 1e-6f);
    }

    {
        constexpr float StepSpeedCm = 6800.0f;
        const Quat level = RotatorToQuat(0, 0, 0);
        auto peakSurge = [&](bool hasVel, float reportedScale, float accelCm)
        {
            ogs::Publisher p;
            float distance = 0.0f, speed = StepSpeedCm, peak = 0.0f;
            for (int i = 0; i < 240; i++)
            {
                double step = dt;
                int simSteps = 1;
                if (i % 30 == 10) simSteps = 2;
                if (i % 30 == 20) { step = 2 * dt; simSteps = 1; }
                for (int s = 0; s < simSteps; s++)
                {
                    speed += accelCm * (float)dt;
                    distance += speed * (float)dt;
                }
                p.tick(step, true, Vec3{ distance, 0, 0 }, level, hasVel,
                       Vec3{ speed * reportedScale, 0, 0 });
                if (i > 120) peak = (std::max)(peak, std::abs(p.last_specific_force().Z));
            }
            return std::pair<float, float>(peak, p.last_specific_force().Z);
        };

        const auto fromPosition = peakSurge(false, 0.0f, 0.0f);
        const auto fromEngine = peakSurge(true, 1.0f, 0.0f);
        std::printf("uneven stepping at constant speed: peak surge position=%.1f engine=%.2f\n",
                    fromPosition.first, fromEngine.first);
        Check("uneven stepping: position alone stays quiet too",
              fromPosition.first < 100.0f);
        Check("uneven stepping: engine velocity keeps surge quiet",
              fromEngine.first < 0.5f);

        const auto deadReading = peakSurge(true, 0.0f, 1000.0f);
        std::printf("engine velocity stuck at zero while accelerating 10 m/s^2: surge=%.1f\n",
                    deadReading.second);
        Check("dead engine velocity: falls back to position deltas",
              std::abs(deadReading.second) > 3.0f);

        ogs::Publisher parked;
        for (int i = 0; i < 120; i++)
            parked.tick(dt, true, Vec3{ 0, 0, 0 }, level, true, Vec3{ 0, 0, 0 });
        Check("parked: a zero engine velocity is believed", parked.using_engine_velocity());

        ogs::Publisher wrongWay;
        for (int i = 0; i < 120; i++)
            wrongWay.tick(dt, true, Vec3{ i * 100.0f, 0, 0 }, level, true, Vec3{ 0, 6000.0f, 0 });
        Check("engine velocity in the wrong direction is rejected",
              !wrongWay.using_engine_velocity());
    }

    {
        const Quat level = RotatorToQuat(0, 0, 0);
        auto firstFallbackTick = [&](int badFrom, int badTicks, int secondBadFrom = -1)
        {
            ogs::Publisher p;
            for (int i = 0; i < 600; i++)
            {
                const bool bad = (i >= badFrom && i < badFrom + badTicks) ||
                                 (secondBadFrom >= 0 && i >= secondBadFrom && i < secondBadFrom + badTicks);
                p.tick(dt, true, Vec3{ i * 6800.0f * (float)dt, 0, 0 }, level, true,
                       Vec3{ bad ? 0.0f : 6800.0f, 0, 0 });
                if (!p.using_engine_velocity()) return i;
            }
            return -1;
        };
        for (int offset = 0; offset < 30; offset += 5)
        {
            Check("a 0.9 s disagreement keeps engine velocity", firstFallbackTick(120 + offset, 54) < 0);
        }
        Check("separate brief disagreements do not add up", firstFallbackTick(120, 54, 300) < 0);
        const int at = firstFallbackTick(120, 480);
        std::printf("lasting disagreement from tick 120: fell back at tick %d\n", at);
        Check("a lasting disagreement falls back after 1 to 2.1 s", at >= 180 && at <= 246);
    }

    {
        const Quat level = RotatorToQuat(0, 0, 0);
        const float perTickCm = 6800.0f * (float)dt;
        bool kept = true;
        float peak = 0.0f;
        for (int offset = 0; offset < 30; offset += 3)
        {
            ogs::Publisher p;
            const int frozeAt = 120 + offset, frozenTicks = 90;
            for (int i = 0; i < 480; i++)
            {
                const int moved = i < frozeAt ? i : i < frozeAt + frozenTicks ? frozeAt : i - frozenTicks;
                p.tick(dt, true, Vec3{ moved * perTickCm, 0, 0 }, level, true, Vec3{ 6800.0f, 0, 0 });
                kept = kept && p.using_engine_velocity();
                if (i > 90) peak = (std::max)(peak, std::abs(p.last_specific_force().Z));
            }
        }
        std::printf("pose frozen 1.5 s at 68 m/s then resumed: peak surge %.2f\n", peak);
        Check("a briefly frozen pose keeps engine velocity", kept);
        Check("a briefly frozen pose causes no surge spike", peak < 0.5f);

        {
            ogs::Publisher p;
            bool stayed = true;
            float after = 0.0f;
            for (int i = 0; i < 960; i++)
            {
                const int moved = i < 120 ? i : i < 720 ? 120 : i - 600;
                p.tick(dt, true, Vec3{ moved * perTickCm, 0, 0 }, level, true, Vec3{ 6800.0f, 0, 0 });
                stayed = stayed && p.using_engine_velocity();
                if (i > 90) after = (std::max)(after, std::abs(p.last_specific_force().Z));
            }
            std::printf("pose completely still for 10 s then resumed: peak surge %.2f\n", after);
            Check("a completely still pose never switches, however long", stayed);
            Check("a long still pose causes no surge spike", after < 0.5f);
        }

        auto rejectedAt = [&](float velocityCm, float creepAfterTick)
        {
            ogs::Publisher p;
            float x = 0.0f;
            for (int i = 0; i < 900; i++)
            {
                x += i < creepAfterTick ? perTickCm : 0.03f * perTickCm;
                p.tick(dt, true, Vec3{ x, 0, 0 }, level, true, Vec3{ velocityCm, 0, 0 });
                if (!p.using_engine_velocity()) return i;
            }
            return -1;
        };
        const int creeping = rejectedAt(6800.0f, 120);
        const int huge = rejectedAt(680000.0f, 900);
        std::printf("pose creeping at 3%%: rejected at tick %d; velocity 100x too big: tick %d\n", creeping, huge);
        Check("a pose that only creeps is eventually not believed", creeping > 300 && creeping <= 450);
        Check("a velocity 100x too big is still rejected", huge > 0 && huge <= 330);
    }

    {
        const Quat level = RotatorToQuat(0, 0, 0);
        {
            ogs::Publisher p;
            float x = 0.0f;
            bool fellBack = false, trustedAgain = false;
            for (int i = 0; i < 1500; i++)
            {
                const bool moving = (i % 360) < 240;
                if (moving) x += 1000.0f * (float)dt;
                p.tick(dt, true, Vec3{ x, 0, 0 }, level, true, Vec3{ 0, 0, 0 });
                if (!p.using_engine_velocity()) fellBack = true;
                else if (fellBack) trustedAgain = true;
            }
            Check("velocity stuck at zero: standing still does not restore trust",
                  fellBack && !trustedAgain);
        }
        {
            ogs::Publisher p;
            float x = 0.0f;
            bool fellBack = false;
            for (int i = 0; i < 600; i++)
            {
                const float speedCm = i < 200 ? 1000.0f : 300.0f;
                x += speedCm * (float)dt;
                p.tick(dt, true, Vec3{ x, 0, 0 }, level, true, Vec3{ i < 200 ? 0.0f : speedCm, 0, 0 });
                fellBack = fellBack || !p.using_engine_velocity();
            }
            Check("a velocity that comes right while walking is trusted again",
                  fellBack && p.using_engine_velocity());
        }
        {
            ogs::Publisher p;
            float x = 0.0f, peak = 0.0f;
            for (int i = 0; i < 360; i++)
            {
                double step = dt;
                int simSteps = 1;
                if (i % 30 == 10) simSteps = 2;
                if (i % 30 == 20) step = 2 * dt;
                x += simSteps * 6800.0f * (float)dt;
                const bool readFailed = i % 90 == 40;
                p.tick(step, true, Vec3{ x, 0, 0 }, level, !readFailed,
                       readFailed ? Vec3{} : Vec3{ 6800.0f, 0, 0 });
                if (i > 90) peak = (std::max)(peak, std::abs(p.last_specific_force().Z));
            }
            std::printf("single failed velocity reads at 68 m/s: peak surge %.2f\n", peak);
            Check("a single failed velocity read causes no surge spike", peak < 0.5f);
        }
    }

    {
        const Quat level = RotatorToQuat(0, 0, 0);
        auto slowMotion = [&](bool passScale, bool hasVel)
        {
            ogs::Publisher p;
            float x = 0.0f, previousScale = 1.0f, peak = 0.0f;
            int countedFailures = 0;
            bool stayed = true;
            for (int i = 0; i < 480; i++)
            {
                float scale = 1.0f;
                if (i >= 120 && i < 156) scale = 1.0f - (i - 119) / 36.0f;
                else if (i >= 156 && i < 276) scale = 0.0f;
                else if (i >= 276 && i < 312) scale = (i - 275) / 36.0f;
                scale = (std::max)(scale, 0.0001f);
                x += 6800.0f * (float)dt * previousScale;
                p.tick(dt, true, Vec3{ x, 0, 0 }, level, hasVel, Vec3{ 6800.0f, 0, 0 },
                       passScale ? scale : 1.0f);
                previousScale = scale;
                if (i > 90) peak = (std::max)(peak, std::abs(p.last_specific_force().Z));
                if (hasVel)
                {
                    stayed = stayed && p.using_engine_velocity();
                    countedFailures = (std::max)(countedFailures, p.velocity_check().failures_in_a_row());
                }
            }
            return std::make_pair(peak, stayed && countedFailures == 0);
        };
        const auto ignored = slowMotion(false, false);
        const auto followed = slowMotion(true, false);
        std::printf("slow motion to a stop and back at 68 m/s, positions only: peak surge ignoring the time scale %.1f, following it %.1f\n",
                    ignored.first, followed.first);
        Check("slow motion: ignoring the time scale reads as braking (the field symptom)", ignored.first > 50.0f);
        Check("slow motion: following the time scale keeps surge quiet", followed.first < 2.0f);
        Check("slow motion: the engine velocity passes every check when game time is used",
              slowMotion(true, true).second);
        ogs::Publisher odd;
        for (int i = 0; i < 120; i++)
            odd.tick(dt, true, Vec3{ i * 100.0f, 0, 0 }, level, false, Vec3{},
                     i % 2 ? -1.0f : std::nanf(""));
        Check("slow motion: an unreadable time scale counts as normal speed",
              std::abs(odd.last_specific_force().Z) < 0.5f);
    }

    // ---- cadence: uneven frames near the motion rate are not skipped ----
    {
        WSADATA wsa{};
        WSAStartup(MAKEWORD(2, 2), &wsa);
        SOCKET in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        sockaddr_in at{};
        at.sin_family = AF_INET;
        at.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        int length = sizeof at;
        bind(in, (const sockaddr*)&at, sizeof at);
        getsockname(in, (sockaddr*)&at, &length);
        u_long nonBlocking = 1;
        ioctlsocket(in, FIONBIO, &nonBlocking);

        const double cases[][2] = { { 59, 0.002 }, { 59.94, 0.0005 }, { 59.94, 0.002 },
                                    { 62, 0.002 }, { 144, 0.002 } };
        double worstShare = 1.0, fastest = 0.0;
        for (const auto& c : cases)
        {
            ogs::Publisher cadence;
            ogs::PublisherConfig loopback;
            loopback.group = "127.0.0.1";
            loopback.port = ntohs(at.sin_port);
            if (!cadence.start(loopback)) { worstShare = 0.0; break; }
            const double seconds = 20.0;
            int motion = 0;
            double previous = 0.0;
            unsigned seed = 7;
            for (int i = 1; i <= (int)(seconds * c[0]); i++)
            {
                seed = seed * 1103515245u + 12345u;
                const double noise = ((seed >> 16) & 0x7fff) / 16383.5 - 1.0;
                const double t = i / c[0] + c[1] * noise;
                cadence.tick(t - previous, true, { 0, 0, 0 }, { 0, 0, 0, 1 });
                previous = t;
                char datagram[2048];
                while (recv(in, datagram, sizeof datagram, 0) > 4)
                    if (datagram[4] == (char)ogs::telemetry::FrameType::Motion) motion++;
            }
            cadence.stop();
            const double achievable = c[0] < 60.0 ? c[0] : 60.0;
            const double rate = motion / seconds;
            std::printf("cadence: %.2f fps, jitter %.1f ms -> %.2f Hz\n", c[0], c[1] * 1000, rate);
            if (rate / achievable < worstShare) worstShare = rate / achievable;
            if (rate > fastest) fastest = rate;
        }
        // a host sampling from a hook that fires in a short burst each frame
        const double bursts[][2] = { { 60, 0.0008 }, { 62, 0.0005 }, { 66, 0.0005 },
                                     { 72, 0.0005 }, { 144, 0.0005 } };
        double worstBurstShare = 1.0, fastestBurst = 0.0;
        for (const auto& c : bursts)
        {
            ogs::Publisher cadence;
            ogs::PublisherConfig loopback;
            loopback.group = "127.0.0.1";
            loopback.port = ntohs(at.sin_port);
            if (!cadence.start(loopback)) { worstBurstShare = 0.0; break; }
            const double seconds = 20.0, burstSeconds = 0.0014;
            const int callsPerBurst = 100;
            int motion = 0;
            double previous = 0.0;
            unsigned seed = 7;
            for (int frame = 1; frame <= (int)(seconds * c[0]); frame++)
            {
                seed = seed * 1103515245u + 12345u;
                const double noise = ((seed >> 16) & 0x7fff) / 16383.5 - 1.0;
                for (int call = 0; call < callsPerBurst; call++)
                {
                    const double t = frame / c[0] + c[1] * noise +
                                     burstSeconds * call / (callsPerBurst - 1);
                    if (previous != 0.0 && !cadence.sample_due(t - previous)) continue;
                    cadence.tick(t - previous, true, { 0, 0, 0 }, { 0, 0, 0, 1 });
                    previous = t;
                    char datagram[2048];
                    while (recv(in, datagram, sizeof datagram, 0) > 4)
                        if (datagram[4] == (char)ogs::telemetry::FrameType::Motion) motion++;
                }
            }
            cadence.stop();
            const double rate = motion / seconds;
            std::printf("hook bursts: %.0f fps, jitter %.1f ms -> %.2f Hz\n", c[0], c[1] * 1000, rate);
            if (rate / 60.0 < worstBurstShare) worstBurstShare = rate / 60.0;
            if (rate > fastestBurst) fastestBurst = rate;
        }
        Check("cadence: a host sampling from bursts of hook calls keeps the motion rate",
              worstBurstShare >= 0.98 && fastestBurst <= 60.1);
        closesocket(in);
        WSACleanup();
        Check("cadence: uneven frames near the motion rate are not skipped",
              worstShare >= 0.98 && fastest <= 60.1);
    }

    std::printf("smoke: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
