#include <capnp/message.h>
#include <capnp/serialize.h>
#include "test.capnp.h"
#include <chrono>
#include <iostream>
#include <random>
#include <cassert>


int test_capnp(size_t iterations)
{
    using namespace mynamespace;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> rand_double(10, 100);
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Running capnproto encode decode loop" << std::endl;
    for (size_t i = 0; i < iterations; i++) {
        capnp::MallocMessageBuilder message;
        Mytable::Builder tb = message.getRoot<Mytable>();

        tb.setTypeName("mytype");
        std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::now();
        int seconds = (int) std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
        auto createdAtTimeStamp = seconds;
        tb.setCreatedAtTimeStamp(createdAtTimeStamp);
        tb.setToken("tk1");
        tb.setLen(rand_double(gen));
        tb.setSpeed(rand_double(gen));
        auto ptbuilder = tb.initPointXY();
        ptbuilder.setX(rand_double(gen));
        ptbuilder.setY(rand_double(gen));
        ptbuilder.setThetaInRadians(rand_double(gen));
        auto ptllbuilder = tb.initPointLatLon();
        ptllbuilder.setLatitude(rand_double(gen));
        ptllbuilder.setLongitude(rand_double(gen));
        ptllbuilder.setBearingDegrees(rand_double(gen));

        auto goalsbuilder = tb.initGoals(2);
        auto p1builder = goalsbuilder[0];
        auto pt1builder = p1builder.initTargetPointXY();
        pt1builder.setX(rand_double(gen));
        pt1builder.setY(rand_double(gen));
        pt1builder.setThetaInRadians(rand_double(gen));
        auto ptll1builder = p1builder.initTargetPointLatLon();
        ptll1builder.setLatitude(rand_double(gen));
        ptll1builder.setLongitude(rand_double(gen));
        ptll1builder.setBearingDegrees(rand_double(gen));
        p1builder.setPurpose("P1");
        auto p2builder = goalsbuilder[1];
        auto pt2builder = p2builder.initTargetPointXY();
        pt2builder.setX(rand_double(gen));
        pt2builder.setY(rand_double(gen));
        pt2builder.setThetaInRadians(rand_double(gen));
        auto ptll2builder = p2builder.initTargetPointLatLon();
        ptll2builder.setLatitude(rand_double(gen));
        ptll2builder.setLongitude(rand_double(gen));
        ptll2builder.setBearingDegrees(rand_double(gen));
        p2builder.setPurpose("P2");

        tb.setState("inplay");


        auto serialized = message.getSegmentsForOutput();
        capnp::SegmentArrayMessageReader reader(serialized);
        auto tbreader = reader.getRoot<Mytable>();
        auto typeName2 = tbreader.getTypeName();
        assert(typeName2 == "mytype");
        auto seconds2 = tbreader.getCreatedAtTimeStamp();
        assert(seconds == seconds2);
        auto token2 = tbreader.getToken();
        assert(token2 == "tk1");
        auto ptreader = tbreader.getPointXY();
        assert(ptreader.getX() == ptbuilder.getX());
        assert(ptreader.getY() == ptbuilder.getY());
        assert(ptreader.getThetaInRadians() == ptbuilder.getThetaInRadians());
        auto ptllreader = tbreader.getPointLatLon();
        assert(ptllreader.getLatitude() == ptllbuilder.getLatitude());
        assert(ptllreader.getLongitude() ==  ptllbuilder.getLongitude());
        assert(ptllreader.getBearingDegrees() == ptllbuilder.getBearingDegrees());

        auto len2 = tbreader.getLen();
        assert(tb.getLen() == len2);
        assert(tb.getSpeed() == tbreader.getSpeed());

        auto goalsreader= tbreader.getGoals();
        auto p1reader = goalsreader[0];
        auto cmppt1 = p1reader.getTargetPointXY();
        assert(cmppt1.getX() == pt1builder.getX());
        assert(cmppt1.getY() == pt1builder.getY());
        assert(cmppt1.getThetaInRadians() == pt1builder.getThetaInRadians());
        auto cmpptll1 = p1reader.getTargetPointLatLon();
        assert(cmpptll1.getLatitude() == ptll1builder.getLatitude());
        assert(cmpptll1.getLongitude() == ptll1builder.getLongitude());
        assert(cmpptll1.getBearingDegrees() == ptll1builder.getBearingDegrees());
        assert(p1reader.getPurpose() == "P1");

        auto p2reader = goalsreader[1];
        auto cmppt2 = p2reader.getTargetPointXY();
        assert(cmppt2.getX() == pt2builder.getX());
        assert(cmppt2.getY() == pt2builder.getY());
        assert(cmppt2.getThetaInRadians() == pt2builder.getThetaInRadians());
        auto cmpptll2 = p2reader.getTargetPointLatLon();
        assert(cmpptll2.getLatitude() == ptll2builder.getLatitude());
        assert(cmpptll2.getLongitude() == ptll2builder.getLongitude());
        assert(cmpptll2.getBearingDegrees() == ptll2builder.getBearingDegrees());
        assert(p2reader.getPurpose() == "P2");
        assert(tbreader.getState() == "inplay");

    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "capnproto encode decode: time = " << duration << " milliseconds for iterations " << iterations << std::endl << std::endl;
    return 0;
}
