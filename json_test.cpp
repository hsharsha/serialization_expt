#include <chrono>
#include <iostream>
#include <random>
#include <cassert>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace mynamespace{

    template <class T>
        void to_json(json& j, const T& msg) {
            j = msg.to_json();
        }

    template <class T>
        void from_json(const json& j, T& msg) {
            msg.from_json(j);
        }

    struct PointXY {
        PointXY();
        PointXY(double, double, double);
        json to_json(void) const;
        void from_json(const json&);
        bool operator == (const PointXY&) const;
        double x;
        double y;
        double thetaInRadians;
    };

    struct PointLatLon {
        PointLatLon();
        PointLatLon(double, double, double);
        json to_json(void) const;
        void from_json(const json&);
        bool operator == (const PointLatLon&) const;
        double latitude;
        double longitude;
        double bearingDegrees;
    };

    struct Goal {
        json to_json(void) const;
        void from_json(const json&);
        bool operator == (const Goal&) const;
        PointXY targetPointXY;
        PointLatLon targetPointLatLon;
        std::string purpose;
    };

    struct MyTable {
        json to_json(void) const;
        void from_json(const json&);
        bool operator == (const MyTable&) const;
        std::string typeName;
        long createdAtTimeStamp;
        std::string token;
        PointXY pointXY;
        PointLatLon pointLatLon;
        double len;
        double speed;
        std::vector<Goal> goals;
        std::string state;
    };

    PointXY::PointXY() : x(-1.0), y(-1.0), thetaInRadians(0.0) {}
    PointXY::PointXY(double x_, double y_, double thetaInRadians_)
        : x(x_), y(y_), thetaInRadians(thetaInRadians_) {}

    json PointXY::to_json(void) const {
        return json{{"X", x},
            {"Y", y},
            {"ThetaRadians", thetaInRadians}
        };
    }

    void PointXY::from_json(const json& j) {
        x = j.at("X").get<double>();
        y = j.at("Y").get<double>();
        thetaInRadians = j.at("ThetaRadians").get<double>();
    }

    bool PointXY::operator == (const PointXY& other) const {
        if(this == &other) return true;
        else return ((this->x == other.x) && (this->y == other.y) && (this->thetaInRadians == other.thetaInRadians));
    }

    PointLatLon::PointLatLon() : latitude(-1.0), longitude(-1.0), bearingDegrees(0.0) {}

    json PointLatLon::to_json(void) const {
        return json{{"Latitude", latitude},
            {"Longitude", longitude},
            {"BearingDegrees", bearingDegrees}
        };
    }

    void PointLatLon::from_json(const json& j) {
        latitude = j.at("Latitude").get<double>();
        longitude = j.at("Longitude").get<double>();
        bearingDegrees = j.at("BearingDegrees").get<double>();
    }

    bool PointLatLon::operator == (const PointLatLon& other) const {
        if(this == &other) return true;
        else return ((this->latitude == other.latitude) && (this->longitude == other.longitude) && (this->bearingDegrees == other.bearingDegrees));
    }

    json Goal::to_json(void) const {
        return json{{"TargetPointXY", json(targetPointXY)},
            {"TargetPointLatLon", json(targetPointLatLon)},
            {"Purpose", purpose}
        };
    }

    void Goal::from_json(const json& j) {
        targetPointXY = j.at("TargetPointXY").get<PointXY>();
        targetPointLatLon = j.at("TargetPointLatLon").get<PointLatLon>();
        purpose = j.at("Purpose").get<std::string>();
    }

    bool Goal::operator == (const Goal& other) const {
        return (this != &other) &&
            (this->targetPointXY == other.targetPointXY) &&
            (this->targetPointLatLon == other.targetPointLatLon) &&
            (this->purpose == other.purpose);
    }

    json MyTable::to_json(void) const {
        json js_goals;
        for (Goal goal : goals) {
            js_goals.emplace_back(json(goal));
        }
        return json{{"TypeName", typeName},
            {"CreatedAtTimestamp", createdAtTimeStamp},
            {"Token", token},
            {"PointXY", json(pointXY)},
            {"PointLatLon", json(pointLatLon)},
            {"Length", len},
            {"Speed", speed},
            {"Goals", js_goals},
            {"State", state}
        };
    }

    void MyTable::from_json(const json& j) {
        createdAtTimeStamp = j.at("CreatedAtTimestamp").get<unsigned int>();
        typeName = j.at("TypeName").get<std::string>();
        token = j.at("Token").get<std::string>();
        pointXY = j.at("PointXY").get<PointXY>();
        pointLatLon = j.at("PointLatLon").get<PointLatLon>();
        len = j.at("Length").get<double>();
        speed = j.at("Speed").get<double>();
        json js_goals = j.at("Goals");
        for(json goal : js_goals) {
            goals.emplace_back(goal.get<Goal>());
        }
        state = j.at("State").get<std::string>();
    }

    bool MyTable::operator == (const MyTable& other) const {
        if(this == &other) return true;
        else return((this->createdAtTimeStamp == other.createdAtTimeStamp) &&
                (this->typeName == other.typeName) &&
                (this->token == other.token) &&
                (this->pointXY == other.pointXY) &&
                (this->pointLatLon == other.pointLatLon) &&
                (this->len == other.len) &&
                (this->speed == other.speed) &&
                (this->goals == other.goals) &&
                (this->state == other.state)
                );
    }

}

int test_json_serialization(size_t iterations)
{
    using namespace mynamespace;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> rand_double(10, 100);
    std::cout << "Running json encode decode loop" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; i++) {
        std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::now();
        int seconds = (int) std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
        json js_mytable = {{"CreatedAtTimestamp", seconds},
            {"PointLatLon",{{"BearingDegrees", rand_double(gen)}, {"Latitude", rand_double(gen)}, {"Longitude", rand_double(gen)}}},
            {"PointXY",{{"ThetaRadians", rand_double(gen)}, {"X", rand_double(gen)},{"Y", rand_double(gen)}}},
            {"Length", rand_double(gen)},
            {"Speed", rand_double(gen)},
            {"TypeName", "Mytype"},
            {"Goals", {
                          {{"Purpose","P1"},
                              {"TargetPointLatLon",{{"BearingDegrees", rand_double(gen)},{"Latitude", rand_double(gen)},{"Longitude", rand_double(gen)}}},
                              {"TargetPointXY",{{"ThetaRadians", rand_double(gen)},{"X", rand_double(gen)},{"Y", rand_double(gen)}}}},
                          {{"Purpose","P2"},
                              {"TargetPointLatLon",{{"BearingDegrees", rand_double(gen)},{"Latitude", rand_double(gen)},{"Longitude", rand_double(gen)}}},
                              {"TargetPointXY",{{"ThetaRadians", rand_double(gen)},{"X", rand_double(gen)},{"Y", rand_double(gen)}}}}}
            },
            {"State","inplay"},
            {"Token","tk1"}
        };

        MyTable mtable = js_mytable.get<MyTable>();
        json js_mytable2 = json(mtable);
        assert(js_mytable == js_mytable2);
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "json encode decode: time = " << duration << " milliseconds for " << iterations << " iterations."<< std::endl << std::endl;
    return 0;
}

