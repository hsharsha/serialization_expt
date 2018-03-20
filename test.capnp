@0xed174307696f2226;

$import "/capnp/c++.capnp".namespace("mynamespace");

struct PointXY {
    x @0: Float64;
    y @1: Float64;
    thetaInRadians @2: Float64;
}

struct PointLatLon {
    latitude @0: Float64;
    longitude @1: Float64;
    bearingDegrees @2: Float64;
}

struct Goal {
    targetPointXY @0: PointXY;
    targetPointLatLon @1: PointLatLon;
    purpose @2: Text;
}

struct Mytable {
    typeName @0: Text;
    createdAtTimeStamp @1: Int64;
    token @2: Text;
    pointXY @3: PointXY;
    pointLatLon @4: PointLatLon;
    len @5: Float64;
    speed @6: Float64;
    goals @7: List(Goal);
    state @8: Text;
}


