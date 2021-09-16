// Only for testing.
// A self-contained program with threads for a server and some clients.
// An example that does not read any config file.

#include <thread>
#include <vector>

#include "ClockClient.h"
#include "HighResolutionClock.h"
#include "PhaseLockedClock.h"

using namespace dex;
using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " port duration\n";
        return 1;
    }
    const auto port = atoi(argv[1]);
    std::chrono::microseconds runtime(int64_t(1000000 * atof(argv[2])));

    auto& clockHiRes = HighResolutionClock::instance();

    ClockClient client(ost::InetHostAddress("127.0.0.1"), port);
    client.setTimeout(1000);
    client.setAcknowledge(true);
    PhaseLockedClock plc(clockHiRes, client);
    plc.setPhasePanic(5000);
    plc.setUpdatePanic(5000000);

    std::atomic_bool end_clocks(false);

    std::thread plcThread(&PhaseLockedClock::run, &plc, std::ref(end_clocks));

    while (runtime.count() > 0) {
        std::cout << "offset: " << plc.getOffset() << "\n" << timestampToString(plc.getValue()) << std::endl;
        std::cout << std::endl;
        constexpr auto wait = 600ms;
        std::this_thread::sleep_for(wait);
        runtime -= wait;
    }

    plc.die();
    plcThread.join();
    return 0;
}
