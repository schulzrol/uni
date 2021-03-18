library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- rozhrani Vigenerovy sifry
entity vigenere is
   port(
         CLK : in std_logic;
         RST : in std_logic;
         DATA : in std_logic_vector(7 downto 0);
         KEY : in std_logic_vector(7 downto 0);

         CODE : out std_logic_vector(7 downto 0)
    );
end vigenere;

-- V souboru fpga/sim/tb.vhd naleznete testbench, do ktereho si doplnte
-- znaky vaseho loginu (velkymi pismeny) a znaky klice dle vaseho prijmeni.

architecture behavioral of vigenere is

    -- Sem doplnte definice vnitrnich signalu, prip. typu, pro vase reseni,
    -- jejich nazvy doplnte tez pod nadpis Vigenere Inner Signals v souboru
    -- fpga/sim/isim.tcl. Nezasahujte do souboru, ktere nejsou explicitne
    -- v zadani urceny k modifikaci.
	signal shift: std_logic_vector(7 downto 0);
	signal plusKorekce: std_logic_vector(7 downto 0);
	signal minusKorekce: std_logic_vector(7 downto 0);
	-- Vystupne stavy FSM
	-- "00" - cisla
	-- "01" - pricita
	-- "10" - odecita
	-- "11" - reset stav
	signal fsmOut: std_logic_vector(1 downto 0);
	type tAction is (pridat, odebrat);
	signal nowState: tAction := pridat;
	signal nextState: tAction := odebrat;
	signal hashtag: std_logic_vector(7 downto 0) := "00100011";

begin
    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- AUTOREM PROJEKTU JE XSCHUL06 ROLAND SCHULZ LOREM IPSUM DOLOR SI AMET
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.

	-- VYBER ZNAKU NA VYSTUP MULTIPLEXOREM
	multiplexor: process (fsmOut) is
	begin
		if (fsmOut = "01") then
			CODE <= plusKorekce;
		elsif (fsmOut = "10") then
			CODE <= minusKorekce;
		elsif (fsmOut = "00") then
			CODE <= hashtag;
		else
			-- hashtag pro reset 1
			CODE <= hashtag;
		end if;
	end process;
	 

	-- STAVOVY AUTOMAT --------------------------------------------------
	-- LOGIKA STAVU
	stateLogic: process (CLK, RST) is
	begin
		if (RST = '1') then
			nowState <= pridat;
		elsif rising_edge(CLK) then
			nowState <= nextState;
		end if;	
	end process; 

	-- LOGIKA PRECHODU
	fsmMealy: process (nowState, DATA, RST) is
	begin
		nextState <= nowState;
		case nowState is
			when pridat =>
				nextState <= odebrat;
				fsmOut <= "01";
			when odebrat =>
				nextState <= pridat;
				fsmOut <= "10";
		end case;
		-- pokud je cislo
		if (DATA > 47 and DATA < 58) then
				fsmOut <= "00";
		end if;
		-- pokud jsme v resetu
		if (RST = '1') then
			fsmOut <= "11";
		end if;
	end process;

	-- RESOLUTION POSUVU
	shiftProcess: process (KEY) is
	begin
		-- odecteme minus znak pred ascii 'A' abysme vedeli kolik posunout
		shift <= KEY - 64;
	end process;
	

	-- POSUVY --------------------------------------------------------------------------------
	-- ZAPORNY POSUV
	minusProces: process (shift, DATA) is
		variable posunute: std_logic_vector(7 downto 0);
	begin
		-- odecteme posuv k DATA hodnote
		posunute := DATA - shift;
		-- pokud podteceme pred ascii 'A'
		if (posunute < 65) then
			-- pricteme rozdil ascii 'A' a 'Z' k ziskani spravneho znaku po podteceni
			posunute := posunute + 25;
		end if;
		
		minusKorekce <= posunute;
	end process;

	-- KLADNY POSUV
	plusProces: process (shift, DATA) is
		variable posunute: std_logic_vector(7 downto 0);
	begin
		-- pricteme posuv k DATA hodnote
		posunute := DATA + shift;
		-- pokud preteceme za ascii 'Z'
		if (posunute > 90) then
			-- odecteme rozdil ascii 'A' a 'Z' k ziskani spravneho znaku po preteceni
			posunute := posunute - 25;
		end if;
		
		plusKorekce <= posunute;
	end process;
end behavioral;
