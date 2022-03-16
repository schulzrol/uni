Implementační dokumentace k 1. úloze do IPP 2021/2022  
Jméno a příjmení: Roland Schulz  
Login: xschul06

# Průběh analýzy kódu IPPcode22
Analýza probíhá po řádcích čtených ze standardního vstupu, které se následně kategorizují pomocí regulárních výrazů.
Prázdné řádky a řádky pouze s komentářem nejsou hlouběji analyzovány. Řádky obsahující komentář jsou zpracovávany pro potřeby rozšíření STATP. Kontroluje se zároveň absence neprázdných řádků co nejsou komentář před hlavičkou `.IPPcode22`.

Jakékoliv jiné řádky po povinné hlavičce jazyka jsou dále očištěny od přebytečných bílých znaků a komentářů a rozděleny na tokeny. Nad kolekcí tokenů probíhá analýza podle pravidel jazyka.
Instrukce se řadí do podtříd podle své arity (nullary, unary, binary, ternary), třída instrukce implementuje jednu konkrétní instrukci z instrukční sady.
Kontrola typů hodnot operandů je delegována konkrétním třídám typů výrazů na základě regulárního výrazu popisující přípustné hodnoty.

Překlad do reprezentace XML probíhá skrze volání metody `toXMLTemplate()` rozhraní [`XMLPrintable`](XML.php) nad kolekcí korektních instrukcí s operandy.

# Implementovaná rozšíření
## STATP
Implementováno pouze částečně - sbírání statistik je implementováno, avšak rozhraní skrze parametry skriptu `parse.php` není implementováno kvůli potížím s přístupem k `$argv` na vývojovém stroji.

Sběr statistik funguje na základě modifikací globálně přístupného kontextu [`Context`](Context.php) pomocí zasílání konkrétních zpráv o úpravách, nebo delegováním úprav skrze rozhraní `ModifiesContext`.

## NVP
Pro implementaci parseru jsem využil následující návrhové vzory:

### Abstract Factory  
Implementované továrny delegují rozlišitelnost konkrétních instrukcí a typů parametrů na konkrétní podtřídy bázových tříd [`BaseInstruction`](instruction/BaseInstruction.php) (podle operačního kódu) a [`BaseExpression`](expression/BaseExpression.php) (podle regexu přípustných hodnot).

### Singleton  
Využito pro jednotný přístup k kontextu uchovávajícím statistiky o parsování vstupního kódu. Třída implementující [`Singleton`](Singleton.php) poskytuje svým podtřídám opakovaný přístup vždy pouze k jedné instanci podtřídy odkudkoliv, kde se podtřída instancuje třídní metodou `getInstance()`.

### Command (zjednodušený)  
Implementace shromažďování statistik pro rozšíření STATP bylo implementováno jednotným rozhraním umožňující vrátit volajícímu funkci, jejímž provoláním volající "slepě" upravil statistiky pro potřeby konkrétní parsované instrukce.
      