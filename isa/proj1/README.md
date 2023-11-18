# TFTP client-server
## Autor
Roland Schulz (xschul06)

datum: 15.10.2023

# TODO
- [x] Rozchodit nix (`nix develop "git+https://github.com/schullzroll/dev-envs-m1.git?dir=isa#c"`)
- [x] Zkopírovat si relevantní RFCčka do `rfcs/`
- [x] Děcko procesy komunikují s klientem a klient s děckem
- [.] Udělat packety tak, aby se daly jednoduše instanciovat z bufferu ideálně i s kontrolou validity v nějaké funkci
    - vyreseno skrz virtualni destuktor a paket factory
    - neozkouseno ve valgrindu
- [ ] Kontrola netascii a octet - udelat ji case insensitive
- [ ] První ACK na RRQ/WRQ musí přijít už od child procesu a po celou dobu jen od child procesu
- [ ] Podpora pro hostname na klientovi (i serveru?)
- [ ] Nutno kontrolovat v serveru i clientu TIDs (porty)
- [ ] Zprovoznit option v paketech podle RFC
- [ ] fread potencialne neposila prazdne radky (radky jen s newline)
- [ ] Zprovoznit timeouty a retry podle RFC
    - jak bude fungovat timeout na klientovi? (vzdyt to je jen jeden proces) asi https://stackoverflow.com/a/39841004/8354587
    A: timeout bude vždy v child procesu, který bude čekat na ACK nebo v klientovi, který bude čekat na DATA nebo ACK
- [ ] Zprovoznit přenos souborů delších než 512B
- [.] Transformace dat Data packetu do spravneho formatu (netascii, octet)
- [ ] Logovat pouze příchozí zprávy (jak na klientu tak na serveru)
- [ ] Testy
    - testovat pomoci shell skriptu, ktery bude kontrolovat obsahy souboru
    - testovat pomoci valgrindu
    - testovat pomoci wiresharku
    - jednotkove testy asi nebude treba
    - jak testovat timeouty? (asi jenom vytvořit mockovací třídu, která bude mít vždy timeout)
- [ ] Dokumentace
  - [ ] Jak se kompiluje
  - [ ] Jak se testuje
  - [ ] Jak se používá
  - [ ] docstringy
  - [ ] komentáře
  - [ ] README.md
  - [ ] hlavicka s autorem a datumem všude v souborech