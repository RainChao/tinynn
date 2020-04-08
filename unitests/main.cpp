#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <comlog/comlog.h>
#include <utility/register.h>
//#pragma message("here")

DEFINE_int32(log_level, 4, "");

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    google::ParseCommandLineFlags(&argc, &argv, true);

    com_logstat_t logstat;
    logstat.sysevents = FLAGS_log_level;
    com_device_t dev[1];
    strcpy(dev[0].type, "TTY");
    COMLOG_SETSYSLOG(dev[0]);
    com_openlog("main", dev, 1, &logstat);

    tinynn::run_all_routinues(argc, argv);
    return RUN_ALL_TESTS();
}
