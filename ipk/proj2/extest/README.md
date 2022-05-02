# Packet Sniffer - IPK projekt
Síťový analyzátor v C++, který je schopný na určitém síťovém rozhraním zachytávat a filtrovat pakety

## Podpora
Otestovaná funkčnost v prostředí `Ubuntu 21.10 impish`.

## Kompilace
```
make
```

## Příklady použití
```
./ipk-sniffer -i eth0 --tcp -p 80
./ipk-sniffer
```

## Závislosti
- `libpcap-dev`
- externí inkludovaná single header knihovna [`lib/cxxopts.hpp`](https://github.com/jarro2783/cxxopts) pro zpracování argumentů příkazové řádky

