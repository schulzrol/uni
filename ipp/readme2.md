Implementační dokumentace k 2. úloze do IPP 2021/2022  
Jméno a příjmení: Roland Schulz  
Login: xschul06

# Interpret
## Analýza
Instance třídy `InstructionManager` a `LabelManager` provádí analýzu ve dvou po sobě jdoucích bězích.
V prvním běhu vytvoří InstructionManager instance konkrétních instrukcí na základě XML reprezentace instrukce, do instrukce se též zapouzdří argumenty instrukce. Kontroluje se správnost operačních kódu, návaznost pořadí instrukcí (atribut `order`) i argumentů (arg1 až arg3). Vytváří se list instrukcí.
V druhém běhu `LabelManager` agreguje definované návěští instrukcí `LABEL` a mapuje je na pořadí v listu instrukcí. Probíhá i preventivní kontrola skoků na nedefinované návěští.

## Běh
Instance třídy `Context` zapouzdřuje stav interpretu. Obsahuje v sobě informace o instrukcích (i právě zpracovávaných), návěštích, rámcích a použitém vstupu programu. Implementuje datový stack.
Pro každou instrukci existuje zvláštní třída s konkrétní implementací metody `.execute()`, metoda `.execute()` abstrahuje provedení funkcionality konkrétní instrukce.

Samotný běh probíhá inkrementováním ukazatele na instrukce a eventuelním zavoláním metody `.execute()` pro právě interpretovanou instrukci. Metodě se předá jako parametr stav interpretu, aby jej instrukce mohla modifikovat.

Konec nastane v případě vyčerpání listu instrukcí, modifikací stavu způsobem značícím konec běhu programu (instrukce `EXIT`) nebo odchycenou běhovou chybou.

# Test
Nejdříve se externím příkazem `find` (použito hlavně kvůli usnadnění práce při rekurzivním průchodu) zjistí testovací případy. Z testovacích případů se sesbírají informace potřebné k porovnání. Výsledky testů se ukládají do pole `htmlData`.

Vykreslování reportu probíhá pomocí vzorů pro hlavní stránku reportu a řádek tabulky proběhlých testů, `resources/main_view_template.html` a `resources/test_row_template.html` respektive. Do tabulky hlavní stránky se vkládá vzor pro řádek naplněný daty z pole `htmlData`.

Porovnávání je řešenou programem `diff`, implementace neobsahuje použití externího nástroje A7Soft JExamXML.

Výstupy skriptů jsou ukládány do dočasných unikátně pojmenovaných souborů v adresáři `/tmp`.

## Rozsireni
### NVP
Použil jsem vzor Továrny pro vytváření objektů instrukcí na základě jejich operačního kódu.
`InstructionFactory` je obsažena v `InstructionManager`.