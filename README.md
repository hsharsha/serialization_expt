### About
Compare various poular data serialization libraries for C++.
* [Avro](http://avro.apache.org/)
* [Capnproto](https://capnproto.org/)
* [Flatbuffers](https://google.github.io/flatbuffers/)
* [nlohmann/json](https://github.com/nlohmann/json)

### Build
All external dependencies are downloaded
```
$ mkdir build && cd build
$ cmake ..
$ make
```
### Usage
```
$ ./serialization_expt
```
### Results
A million iterations of encode/decode is run with above mentioned libraries and following result was observed

|serializer | time for 1M iterations |
|-----------|------------------------|
|flatbuffers| 9560 ms                |
|avro       | 7336 ms                |
|capnproto  | 5981 ms                |
|json       | 128849 ms              |

Overall capnproto is fastest and avro performs slightly worse than capnproto
Avro has json schema for IDL, and generates more readable headers.
In my opinion Avro is preferred choice in terms of code clarity and schema definition.

