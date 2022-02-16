# IPK Projekt 1 - HINFOSVC

## Author
Roland Schulz (xschul06)

## About
Project Hardware INFO SerViCe is a server written in C++ providing hardware resources information.

Server is intended for use and tested on Ubuntu 20.04 LTS. Other linux distros may or may not work as expected.

## Structure
| File                                             | Description                                                                   |
|--------------------------------------------------|-------------------------------------------------------------------------------|
| [hinfosvc.cpp](./hinfosvc.cpp)                   | Server implementation                                                         |
| [HttpStatusCodes_Cpp.h](./HttpStatusCodes_Cpp.h) | Imported single header library providing mapping between HTTP code and status |
| [Makefile](./Makefile)                           | Build description for project, also provides basic jobs for testing           |
| [Readme.md](./Readme.md)                         | Documentation (you are currently reading this)                                |

## Services
| Resource    | Description                                      | Usage                               |
|-------------|--------------------------------------------------|-------------------------------------|
| `/hostname` | Returns current hostname of the server           | `curl -s localhost:{port}/hostname` |
| `/cpu-name` | Returns CPU model name                           | `curl -s localhost:{port}/hostname` |
| `/load`     | Returns current cpu utilization/load in percents | `curl -s localhost:{port}/hostname` |

## Makefile jobs
| Job       | Description                                                                                                            | Usage          |
|-----------|------------------------------------------------------------------------------------------------------------------------|----------------|
| `all`     | Builds the server and produces an `hinfosvc` executable                                                                | `make all`     |
| `test`    | Provides very basic test capabilities by starting the server on predefined port and `curl`-ing each of it's resources. | `make test`    |
| `archive` | Archives this project as specified in project assignment. Simplifies turn in process.                                  | `make archive` |
| `clean`   | Removes compiled executable and archive                                                                                | `make clean`   |

## Run locally
### Compile the server
```bash
make clean all
```

### Run
Server expects a single commandline argument, that is a port number to which to bind the server.

When specified port is `0`, server will use any available port.
```bash
./hinfosvc 0 &
```

or
```bash
./hinfosvc 8080 &
```

Upon succesfull start, server will inform of the port it's listening on.

With every client accepted, server outputs the clients IP address.

Server forks children processes to handle each client.

Server closes every client connection after each request, so each header `Connection: keep-alive` is always answered with `Connection: close`. (Possible room for improvement left as an exercise for the reader;-))