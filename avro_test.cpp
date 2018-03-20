#include "test_avro.hpp"
#include <chrono>
#include <iostream>
#include <random>
#include <cassert>

int test_avro_serialization(size_t iterations)
{
    using namespace mynamespace;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> rand_double(10, 100);

    std::cout << "Running avro encode decode loop" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        Mytable mytable, mytable2;

        mytable.typeName = "Mytype";
        std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::now();
        int seconds = (int) std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
        mytable.createdAtTimeStamp  = seconds;
        mytable.token = "tk1";
        mytable.speed = rand_double(gen);
        mytable.len = rand_double(gen);
        mytable.state = "inplay";
        PointXY pointXY;
        pointXY.x = rand_double(gen);
        pointXY.y = rand_double(gen);
        pointXY.thetaInRadians = rand_double(gen);
        mytable.pointXY = pointXY;
        PointLatLon pointLatLon;
        pointLatLon.latitude = rand_double(gen);
        pointLatLon.longitude = rand_double(gen);
        pointLatLon.bearingDegrees = rand_double(gen);
        mytable.pointLatLon = pointLatLon;
        PointXY p1, p2;
        p1.x = rand_double(gen);
        p1.y = rand_double(gen);
        p1.thetaInRadians = rand_double(gen);
        p2.x = rand_double(gen);
        p2.y = rand_double(gen);
        p2.thetaInRadians = rand_double(gen);
        PointLatLon pll1, pll2;
        pll1.latitude = rand_double(gen);
        pll1.longitude = rand_double(gen);
        pll1.bearingDegrees = rand_double(gen);
        pll2.latitude = rand_double(gen);
        pll2.longitude = rand_double(gen);
        pll2.bearingDegrees = rand_double(gen);
        Goal g1, g2;
        g1.targetPointXY = p1;
        g1.targetPointLatLon = pll1;
        g1.purpose = "P1";
        g2.targetPointXY = p2;
        g2.targetPointLatLon = pll2;
        g2.purpose = "P2";
        mytable.goals.push_back(g1);
        mytable.goals.push_back(g2);

        std::unique_ptr<avro::OutputStream> out = avro::memoryOutputStream();
        avro::EncoderPtr encoder = avro::binaryEncoder();

        encoder->init(*out);
        avro::encode(*encoder, mytable);
        auto in = avro::memoryInputStream(*out);
        auto decoder = avro::binaryDecoder();
        decoder->init(*in);
        avro::decode(*decoder, mytable2);

        assert(mytable.token == mytable2.token);
        assert(mytable.typeName == mytable2.typeName);
        assert(mytable.state == mytable2.state);
        assert(mytable.speed == mytable2.speed);
        assert(mytable.len == mytable2.len);
        assert(mytable.pointXY.x == mytable2.pointXY.x);
        assert(mytable.pointXY.y == mytable2.pointXY.y);
        assert(mytable.pointXY.thetaInRadians = mytable2.pointXY.thetaInRadians);
        assert(mytable.pointLatLon.latitude == mytable2.pointLatLon.latitude);
        assert(mytable.pointLatLon.longitude == mytable2.pointLatLon.longitude);
        assert(mytable.pointLatLon.bearingDegrees == mytable2.pointLatLon.bearingDegrees);
        assert(mytable.goals[0].targetPointXY.x = mytable2.goals[0].targetPointXY.x);
        assert(mytable.goals[0].targetPointXY.y = mytable2.goals[0].targetPointXY.y);
        assert(mytable.goals[0].targetPointXY.thetaInRadians = mytable2.goals[0].targetPointXY.thetaInRadians);
        assert(mytable.goals[0].targetPointLatLon.latitude == mytable2.goals[0].targetPointLatLon.latitude);
        assert(mytable.goals[0].targetPointLatLon.longitude == mytable2.goals[0].targetPointLatLon.longitude);
        assert(mytable.goals[0].targetPointLatLon.bearingDegrees == mytable2.goals[0].targetPointLatLon.bearingDegrees);
        assert(mytable.goals[0].purpose == mytable2.goals[0].purpose);
        assert(mytable.goals[1].targetPointXY.x = mytable2.goals[1].targetPointXY.x);
        assert(mytable.goals[1].targetPointXY.y = mytable2.goals[1].targetPointXY.y);
        assert(mytable.goals[1].targetPointXY.thetaInRadians = mytable2.goals[1].targetPointXY.thetaInRadians);
        assert(mytable.goals[1].targetPointLatLon.latitude == mytable2.goals[1].targetPointLatLon.latitude);
        assert(mytable.goals[1].targetPointLatLon.longitude == mytable2.goals[1].targetPointLatLon.longitude);
        assert(mytable.goals[1].targetPointLatLon.bearingDegrees == mytable2.goals[1].targetPointLatLon.bearingDegrees);
        assert(mytable.goals[1].purpose == mytable2.goals[1].purpose);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "avro encode decode: time = " << duration << " milliseconds for " << iterations << " iterations."<< std::endl << std::endl;
    return 0;
}
