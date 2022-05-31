# crtp-based exchange matching-engine simulator framework

## Motivation
  Run-time polymorphism has a big overhead if a program uses dynamic dispatch over and over due to vtable lookup and memory cache misses. For a program that needs performance, we'd better design a framework that will avoid dynamic dispatch while keep code abstraction and reusability.

## Description
  This project uses modern c++ meta-programming techniques that the makes sure dispatch of the method calls will take place a compile time to overcome the performance bottleneck of dynamic dispatching. In general, the program will, at runtime, read a list of instruments configuration from file, listening order actions of different instruments from clients, and then maintaining an up-to-date order book for each instrument. (more details following later sections)

## Libraries
  This project included a list of third-party liraries but not limited to:  
    1. [yaml-cpp](https://github.com/jbeder/yaml-cpp) </br>
    2. [boost asio 1.76.0](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html)
    
## Components and Usage
  - **Yaml file configuration**
  - **Streamer**
  - **Order**
  - **Order Book**
  - **Matching Policy**

## User-defined Component Example

## Build and Run
  this project uses Cmake (requires minimum version of 3.5.0) to build, and a build script is provided.
  from project root directory, run </br>
  `./build_script` </br>
  and exeutalbe will be built at </br>
  `./build/main`
  
