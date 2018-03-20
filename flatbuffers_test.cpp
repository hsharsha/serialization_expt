#include <cassert>
#include <chrono>
#include <iostream>
#include <random>

#include "test_generated.h"

int test_flatbuffers(size_t iterations)
{
    using namespace mynamespace;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> rand_double(10, 100);
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Running flatbuffer encode decode loop" << std::endl;
    for (size_t i = 0; i < iterations; i++) {
        flatbuffers::FlatBufferBuilder builder;

        auto typeName = builder.CreateString("mytype");
        std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::now();
        int seconds = (int) std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
        auto createdAtTimeStamp = seconds;
        auto token = builder.CreateString("tk1");
        auto pt = PointXY(rand_double(gen), rand_double(gen), rand_double(gen));
        auto ptll = PointLatLon(rand_double(gen), rand_double(gen), rand_double(gen));
        auto len = rand_double(gen);
        auto speed = rand_double(gen);
        auto pt1 = PointXY(rand_double(gen), rand_double(gen), rand_double(gen));
        auto ptll1 = PointLatLon(rand_double(gen), rand_double(gen), rand_double(gen));
        auto p1 = builder.CreateString("P1");
        auto pt2 = PointXY(rand_double(gen), rand_double(gen), rand_double(gen));
        auto ptll2 = PointLatLon(rand_double(gen), rand_double(gen), rand_double(gen));
        auto p2 = builder.CreateString("P2");

        auto g1 = CreateGoal(builder, &pt1, &ptll1, p1);
        auto g2 = CreateGoal(builder, &pt2, &ptll2, p2);
        std::vector<flatbuffers::Offset<Goal>> goalvec;
        goalvec.push_back(g1);
        goalvec.push_back(g2);
        auto goals = builder.CreateVector(goalvec);
        auto state = builder.CreateString("inplay");

        auto mytable = CreateMytable(builder, typeName, createdAtTimeStamp, token,
                &pt, &ptll, len, speed, goals, state);

        builder.Finish(mytable);


        uint8_t *buf = builder.GetBufferPointer();
        auto mytable2 = GetMytable(buf);
        auto typeName2 = mytable2->typeName()->c_str();
        assert(strcmp(typeName2, "mytype") == 0);
        auto seconds2 = mytable2->createdAtTimeStamp();
        assert(seconds == seconds2);
        auto token2 = mytable2->token()->c_str();
        assert(strcmp(token2, "tk1") == 0);
        auto cmppt = mytable2->pointXY();
        assert(cmppt->x() == pt.x());
        assert(cmppt->y() == pt.y());
        assert(cmppt->thetaInRadians() == pt.thetaInRadians());

        auto cmpptll = mytable2->pointLatLon();
        assert(cmpptll->latitude() == ptll.latitude());
        assert(cmpptll->longitude() ==  ptll.longitude());
        assert(cmpptll->bearingDegrees() == ptll.bearingDegrees());
        auto len2 = mytable2->len();
        assert(len2 == len);
        assert(speed == mytable2->speed());
        auto cmpgoals = mytable2->goals();

        auto  cmpgoal1 = cmpgoals->Get(0);
        auto cmppt1 = cmpgoal1->targetPointXY();
        assert(cmppt1->x() == pt1.x());
        assert(cmppt1->y() == pt1.y());
        assert(cmppt1->thetaInRadians() == pt1.thetaInRadians());
        auto cmpptll1 = cmpgoal1->targetPointLatLon();
        assert(cmpptll1->latitude() == ptll1.latitude());
        assert(cmpptll1->longitude() == ptll1.longitude());
        assert(cmpptll1->bearingDegrees() == ptll1.bearingDegrees());
        assert(strcmp(cmpgoal1->purpose()->c_str(), "P1") == 0);

        auto cmpgoal2 = cmpgoals->Get(1);
        auto cmppt2 = cmpgoal2->targetPointXY();
        assert(cmppt2->x() == pt2.x());
        assert(cmppt2->y() == pt2.y());
        assert(cmppt2->thetaInRadians() == pt2.thetaInRadians());
        auto cmpptll2 = cmpgoal2->targetPointLatLon();
        assert(cmpptll2->latitude() == ptll2.latitude());
        assert(cmpptll2->longitude() == ptll2.longitude());
        assert(cmpptll2->bearingDegrees() == ptll2.bearingDegrees());
        assert(strcmp(cmpgoal2->purpose()->c_str(), "P2") == 0);

        assert(strcmp(mytable2->state()->c_str(), "inplay") ==0);

        builder.ReleaseBufferPointer();

    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "flatbuffers encode decode: time = " << duration << " milliseconds for " << iterations << " iterations."<< std::endl << std::endl;
    return 0;
}
