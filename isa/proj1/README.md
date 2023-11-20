# TFTP client-server
## Autor
Roland Schulz (xschul06)

datum vytvoření: 15.10.2023
datum poslední změny: 20.11.2023

## Popis rozšíření a omezení
Projekt je implementací TFTP klienta a konkurentního TFTP serveru s podporou rozšíření blocksize a adaptivního timeoutu.
Rozšíření `blksize` je podporováno na obou stranách, u klienta s možností nastavení hodnoty pomocí parametru příkazové řádky.

Timeout je implementován pouze z části, a to s implicitní podporou adaptivního timeoutu na obou stranách, avšak bez podpory vyjednávání timeoutu pomocí rozšíření `timeout` (pouze implicitní hodnoty v kódu).

Timeout nastane po jedné sekundě čekání na příchozí paket, v případě že do té doby paket nedojde, zašle se předchozí odeslaný paket a doba čekání je zdvojnásobena. Opakování se dějí doku se nevyčerpá množství pokusů (nastaveno max. množství 5 pokusů) nevyčerpá. Pokud dojde k odpovědi serveru, je timeout nastaven zpět na 1 sekundu a pokusy se resetují na 0. Toto chování je implementováno na obou stranách.

Rozšíření `tsize` není zcela implementováno, pouze některé metody naznačují možnost jeho použití (např. metody pro zjišťování velikosti souboru nebo rezervování místa pro soubor při čtení). Server neumožňuje klientovi vyjednat tuto hodnotu.

Server má též podporu režimů `netascii` a `octet`. Klientu nelze nastavit který režim použije, výchozím režimem klienta je `octet`. Režim `mail` není podporován vůbec.

## Vývoj
Komunikace mezi serverem a klientem byla průběžně v rámci vývoje testována pomocí programu Wireshark, ve kterém bylo možné sledovat korektně rozpoznané TFTP pakety a jejich obsah.

## Jak přeložit
`make` - přeloží všechny soubory, vytvoří spustitelné soubory `tftp-client` a `tftp-server`
`make clean` - smaže všechny soubory vytvořené při překladu

## Jak použít
### Server
```
tftp-server [-p port] root_dirpath
    -p místní port, na kterém bude server očekávat příchozí spojení
       pokud není specifikován předpokládá se výchozí dle specifikace (69)
    root_dirpath cesta k adresáři, pod kterým se budou ukládat příchozí soubory
```
### Client
```
tftp-client -h hostname [-p dest_port] [-f filepath] [-b blksize] -t dest_filepath 
    -h IP adresa/doménový název vzdáleného serveru 
    -p port vzdáleného serveru, pokud není specifikován předpokládá se výchozí dle specifikace (69)
    -f cesta ke stahovanému souboru na serveru (pro download), pokud není specifikován používá se obsah stdin (pro upload) 
    -t cesta, pod kterou bude soubor na vzdáleném serveru/lokálně uložen
    -b velikost dat. bloku v bajtech, pokud není specifikován použije se výchozí dle specifikace (512)
```

### Příklad použití
```
./tftp-server /home/user/tftp -p 69
./tftp-client -h localhost -f test.txt -t test.txt
nebo
./tftp-client -h localhost -t test.txt < test.txt
```

## Seznam odevzdávaných souborů
- `README.md` - Tento soubor
- `Makefile` - Makefile pro překlad programu
- /src - složka se zdrojovými kódy
    - /src/client/ - složka se zdrojovými kódy klienta
        - `client.cpp` - main program klienta
    - /src/server/ - složka se zdrojovými kódy serveru
        - `server.cpp` - main program serveru
    - /src/shared/ - složka se zdrojovými kódy sdílenými klientem i serverem
        - `Packet.{cpp,hpp}` - implementace třídy Packet jako základní třídy pro pakety
        - `ACKPacket.{cpp,hpp}` - implementace třídy pro pakety typu ACK
        - `OACKPacket.{cpp,hpp}` - implementace třídy pro pakety typu OACK
        - `DATAPacket.{cpp,hpp}` - implementace třídy pro pakety typu DATA
        - `ERRORPacket.{cpp,hpp}` - implementace třídy pro pakety typu ERROR
        - `xRQPacket.{cpp,hpp}` - implementace základní třídy pro pakety typu WRQ i RRQ (WRQPacket a RRQPacket dědí z této třídy)
        - `isnum.{cpp,hpp}` - implementace pomocné funkce isNum pro kontrolu, zda je řetězec číslo
        - `upperlower.{cpp,hpp}` - implementace pomocných funkcí pro převod řetězce na velká/malá písmena
        - `definitions.hpp` - definice konstant a typů používaných vycházejících z relevantních RFC
        - `error_codes` - soubor s chybovými kódy a popisky
        - `DataTransfer.{cpp,hpp}`
          - implementace třídy pro přenos dat mezi klientem a serverem.
          - Obsahuje hlavní metody `uploadData` a `downloadFile`.
          - Server využívá `uploadData` v případě přijatého požadavku `RRQ` a `downloadFile` v případě přijatého požadavku `WRQ`.
          - Klient využívá `uploadData` v případě odeslaného požadavku `WRQ` a `downloadFile` v případě odeslaného požadavku `RRQ`.

