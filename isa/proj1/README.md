# TFTP client-server
## Autor
Roland Schulz (xschul06)

datum: 15.10.2023

# TODO
- [x] Rozchodit nix (`nix develop "git+https://github.com/schullzroll/dev-envs-m1.git?dir=isa#c"`)
- [x] Zkopírovat si relevantní RFCčka do `rfcs/`
- [ ] První ACK na RRQ/WRQ musí přijít už od child procesu a po celou dobu jen od child procesu
- [ ] Podpora pro hostname na klientovi (i serveru?)
- [ ] Nutno kontrolovat v serveru i clientu TIDs (porty)
- [ ] Zprovoznit option v paketech podle RFC
- [ ] Transformace dat Data packetu do spravneho formatu (netascii, octet)
- [ ] Logovat pouze příchozí zprávy (jak na klientu tak na serveru)
- 