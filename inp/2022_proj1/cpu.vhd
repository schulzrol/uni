-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2022 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): jmeno <login AT stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
USE ieee.numeric_std.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
  -- Programovy citac (PC) 0 - 4096
  signal PC : std_logic_vector(12 downto 0); -- registr PC
  signal PC_fwd : std_logic; -- PC posuneme na dalsi instrukci
  signal PC_bwd : std_logic; -- PC posuneme na predchozi instrukci (pri hledani [)
  signal PC_init : std_logic; -- PC nastavime na start instrukcni pameti 0

  -- Ukazatel do pameti (PTR) 4096 - 8192
  signal PTR : std_logic_vector(12 downto 0); -- registr PTR
  signal PTR_fwd : std_logic; -- PTR posuneme na dalsi datovou adresu
  signal PTR_bwd : std_logic; -- PTR posuneme na predchozi datovou adresu
  signal PTR_init : std_logic; -- PTR nastavime na start datove pameti 4096

  -- Počet (vyrovnanost) závorek (CNT)
  signal CNT : std_logic_vector(15 downto 0); -- registr počtu závorek CNT
  signal CNT_inc : std_logic; -- inkrementace počtu nalezenych zavorek 
  signal CNT_dec : std_logic; -- dec počtu nalezenych zavorek 
  signal CNT_clear : std_logic; -- CNT vynulujeme

  -- Multiplexor pro výběr zda přistupujeme k instrukční/datové části paměti (MX1)
  signal MX1 : std_logic_vector(12 downto 0); -- registr s pristupovou adresou do pameti
  signal MX1_sel : std_logic; -- selekce přístupu do paměti instrukci/dat
  -- 0 : instrukce
  -- 1 : data

  -- Multiplexor pro výběr hodnoty zapisované do paměti (MX2)
  signal MX2 : std_logic_vector(7 downto 0); -- registr s hodnotou co se zapise do pameti
  signal MX2_sel : std_logic_vector(1 downto 0); -- výběr hodnoty pro zapsání (IO, aktualni bunka +-1)
  -- 00 : z IO vstupu (IN_DATA)
  -- 01 : aktualni bunka +1
  -- 10 : aktualni bunka -1
  
  type FSM is (
    state_start, -- vychozi startovaci stav

    state_fetch, -- stav nacitani instrukce
    state_decode, -- stav dekodovani instrukce

    state_PTR_fwd, -- stav posunu v datove pameti dopredu >
    state_PTR_bwd, -- stav posunu v datove pameti dozadu <

    state_DATA_inc, -- stav inkrementace aktualni bunky o 1 +
    state_DATA_inc_mx, -- stav vyberu inkrementovane hodnoty multiplexerem
    state_DATA_inc_wb, -- stav zapisu inkrementovane hodnoty do RAM

    state_DATA_dec, -- stav dekrementace aktualni bunky o 1 -
    state_DATA_dec_mx, -- stav vyberu dekrementovane hodnoty multiplexerem
    state_DATA_dec_wb, -- stav zapisu dekrementovane hodnoty do RAM

    state_while, state_while_condition, state_while_cycle, state_while_next_instruction,
    state_while_end, state_end_while_condition, state_end_while_previous_instruction, state_end_while_cycle, state_end_while_cycle_condition,

    state_DATA_print, -- stav vytisknuti hodnoty aktualni bunky .
    state_DATA_print_out, -- vypis if not busy
    state_DATA_read, -- stav načteni hodnoty do aktualni bunky ,
    state_DATA_read_wb, -- 

    state_unknown, -- neznamy znak => ignoruju
    state_end -- zastaveni vykonavani programu
  );

  signal FSM_state : FSM := state_start; -- aktualni stav FSM
  signal FSM_next_state : FSM;            -- nasledujici stav po přechodu

begin
  -- Obsluha instrukci PC
  -- TODO vyresit loopback
  pc_counter : process (CLK, RESET, PC_fwd, PC_bwd)
  begin
    if RESET = '1' then
      PC <= (others => '0');
    elsif CLK'event and CLK = '1' then -- nabezna hrana
      if PC_fwd = '1' then
        PC <= PC + 1;
      elsif PC_bwd = '1' then
        PC <= PC - 1;
      elsif PC_init = '1' then
        PC <= (others => '0');
      end if;
    end if;
  end process;

  -- Obsluha instrukci PTR
  -- TODO vyresit loopback
  ptr_counter : process (CLK, RESET, PTR_fwd, PTR_bwd)
  begin
    if RESET = '1' then
        PTR <= std_logic_vector(to_unsigned(4096, PTR'length)); -- 4096 == 0x1000
    elsif CLK'event and CLK = '1' then -- nabezna hrana
      if PTR_fwd = '1' then
        PTR <= PTR + 1;
      elsif PC_bwd = '1' then
        PTR <= PTR - 1;
      elsif PTR_init = '1' then
        PTR <= std_logic_vector(to_unsigned(4096, PTR'length)); -- 4096 == 0x1000
      end if;
    end if;
  end process;

  -- Obsluha čitače zavorek
  cnt_counter : process (CLK, RESET, CNT_inc, CNT_dec)
  begin
    if RESET = '1' then
      CNT <= (others => '0');
    elsif CLK'event and CLK = '1' then -- nabezna hrana
      if CNT_inc = '1' then
        CNT <= CNT + 1;
      elsif CNT_dec = '1' then
        CNT <= CNT - 1;
      elsif CNT_clear = '1' then
        CNT <= (others => '0');
      end if;
    end if;
  end process;

  -- Režie výběru ukazatele do paměti
  mx1_ptr : process (CLK, RESET, MX1_sel, PC, PTR)
  begin
    if RESET = '1' then
      MX1 <= (others => '0');
    elsif CLK'event and CLK = '1' then -- nabezna hrana
      if MX1_sel = '0' then
          MX1 <= PC;
      else
          MX1 <= PTR;
      end if;
    end if;
  end process;

  DATA_ADDR <= MX1; -- nastaveni adresy do pameti podle MX1

  -- Režie výběru zapisované hodnoty do paměti
  mx2_wdata : process (CLK, RESET, MX2_sel)
  begin
    if RESET = '1' then
      MX2 <= (others => '0');
    elsif CLK'event and CLK = '1' then -- nabezna hrana
      case MX2_sel is
          when "00" => -- z IO vstupu
            MX2 <= IN_DATA;
          when "01" => -- +1
            MX2 <= DATA_RDATA + 1;
          when "10" => -- -1
            MX2 <= DATA_RDATA - 1;
          when others => -- nedefinovano, vratim nuly
            MX2 <= (others => '0');
        end case;
    end if;
  end process;

  DATA_WDATA <= MX2; -- nastaveni potencialne zapisovanych dat podle MX2
  OUT_DATA <= MX2;

  -- Režie FSM
  -- Přechod na další stav
  fsm_walk: process (CLK, RESET, EN)
  begin
    if RESET = '1' then
      FSM_state <= state_start;
    elsif CLK'event and CLK = '1' then -- nabezna hrana
      if EN = '1' then
        FSM_state <= FSM_next_state;
      end if;
    end if;
  end process;
  
  -- Logika stavů
  fsm_state_work: process (FSM_state, OUT_BUSY, IN_VLD, CNT, DATA_RDATA) -- TODO
  begin
    -- inicializace drátů
		PC_fwd  <= '0';   -- neposouvame se dopredu o instrukci
		PC_bwd  <= '0';   -- neposouvame se dozadu o instrukci
		PC_init <= '0';   -- neresetujeme ukazatel na instrukci

		PTR_fwd  <= '0';  -- neposouvame se dopredu o dat. bunku
		PTR_bwd  <= '0';  -- neposouvame se dozadu o dat. bunku
		PTR_init <= '0';  -- neresetujeme ukazatel do pameti

		CNT_inc   <= '0';  -- neinkrementujeme počet zavorky
		CNT_dec   <= '0';  -- nedekrementujeme počet zavorky
		CNT_clear <= '0';  -- neresetujeme počet zavorek

    MX1_sel   <= '0';  -- pristup do paměti instrukce
		MX2_sel   <= "00"; -- zapis hodnoty z IO do paměti

		OUT_WE    <= '0'; -- nezapisovat data na LCD
		IN_REQ    <= '0'; -- nepozadovat vstup dat z klavesnice
		DATA_EN   <= '0'; -- nepovoleni cinnosti RAM
		DATA_RDWR <= '0'; -- cteni dat z RAM

    case FSM_state is
      when state_start =>
        PC_init <= '1';  -- nastavime ukazatel instrukci na svuj pocatek
        PTR_init <= '1'; -- nastavime ukazatel do pameti na svuj pocatek
        CNT_clear <= '1'; -- vynulujeme pocet zavorek

        FSM_next_state <= state_fetch;

      when state_fetch =>
        DATA_EN <= '1'; -- chceme zapnout RAM
        MX1_sel <= '0'; -- chceme cist INSTRUKCI z RAM

        FSM_next_state <= state_decode;

        -- >  <  +  -  .  ,  [  ]  (  )
        -- 3E 3C 2B 2D 2E 2C 5B 5D 28 29
      when state_decode =>
        case DATA_RDATA is 
          when X"3E" => -- >
            FSM_next_state <= state_PTR_fwd;
          when X"3C" => -- <
            FSM_next_state <= state_PTR_bwd;
          when X"2B" => -- +
            FSM_next_state <= state_DATA_inc;
          when X"2D" => -- -
            FSM_next_state <= state_DATA_dec;
          when X"2E" => -- .
            FSM_next_state <= state_DATA_read;
          when X"2C" => -- ,
            FSM_next_state <= state_DATA_print;
          when X"5B" => -- [
            FSM_next_state <= state_while;
          when X"5D" => -- ]
            FSM_next_state <= state_while_end;
          when X"28" => -- (
          when X"29" => -- )
          when X"00" => -- null
            FSM_next_state <= state_end;
          when others =>
            FSM_next_state <= state_unknown;
        end case;

      when state_while =>
        PC_fwd <= '1';
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        MX1_sel <= '1';

        FSM_next_state <= state_while_condition;

      when state_while_condition =>
        if DATA_RDATA = (DATA_RDATA'range => '0') then -- DATA_RDATA == 0
          CNT_inc <= '1'; -- increment CNT
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          MX1_sel <= '0'; -- nacteme dalsi instrukci
          -- data jsou 0 => musim najit ] a skocit za nej
          FSM_next_state <= state_while_cycle;
        else -- DATA_RDATA != 0
          FSM_next_state <= state_fetch; -- not 0 => no jumping
        end if;

      when state_while_cycle =>
        if CNT = (CNT'range => '0') then -- CNT == 0
          FSM_next_state <= state_fetch;
        else -- CNT != 0
          if DATA_RDATA = X"5B" then -- narazili jsme na [
            CNT_inc <= '1';
          elsif DATA_RDATA = X"5D" then -- narazili jsme na ]
            CNT_dec <= '1';
          end if;

          PC_fwd <= '1';
          FSM_next_state <= state_while_next_instruction;
          
        end if;

      when state_while_next_instruction =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        MX1_sel <= '0'; -- nacteme dalsi instrukci

        FSM_next_state <= state_while_cycle;
        
      when state_while_end =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        MX1_sel <= '1'; 

        FSM_next_state <= state_end_while_condition;

      when state_end_while_condition =>
        if DATA_RDATA = (DATA_RDATA'range => '0') then --DATA_RDATA == 0
          PC_fwd <= '1'; -- leaving loop
          FSM_next_state <= state_fetch;

        else -- DATA_RDATA != 0
          CNT_inc <= '1'; -- CNT++
          PC_bwd <= '1'; -- going back

          FSM_next_state <= state_end_while_previous_instruction;
        end if;
      
      when state_end_while_previous_instruction =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        MX1_sel <= '0'; -- nacteme dalsi instrukci

        FSM_next_state <= state_end_while_cycle;

      when state_end_while_cycle =>
        if CNT = (CNT'range => '0') then -- if (CNT == 0)
					FSM_next_state <= state_fetch;
				else
					if DATA_RDATA = X"5D" then -- if (CODE_DATA == ']')
						CNT_inc <= '1'; -- CNT += 1
					elsif DATA_RDATA = X"5B" then -- if (CODE_DATA == '[')
						CNT_dec <= '1'; -- CNT -= 1
					end if;

					FSM_next_state <= state_end_while_cycle_condition;
				end if;

      when state_end_while_cycle_condition =>
        if CNT = (CNT'range => '0') then -- CNT == 0
          PC_fwd <= '1'; -- PC += 1
        else -- if (CNT != 0)
          PC_bwd <= '1'; -- PC -= 1
        end if;

        FSM_next_state <= state_end_while_previous_instruction;
        

      when state_PTR_fwd =>
        PTR_fwd <= '1'; -- dalsi datova bunka
        PC_fwd <= '1'; -- dalsi instrukce
        FSM_next_state <= state_fetch; -- exekuce hotova, fetchujeme dalsi instrukci

      when state_PTR_bwd =>
        PTR_bwd <= '1'; -- predchozi datova bunka
        PC_fwd <= '1'; -- dalsi instrukce
        FSM_next_state <= state_fetch; -- exekuce hotova, fetchujeme dalsi instrukci

      -- inkrementace
      -- nacteni dat do DATA_RDATA
      when state_DATA_inc =>
        DATA_EN <= '1';   -- zapneme RAM
        DATA_RDWR <= '0'; -- chceme CIST
        MX1_sel <= '1';    -- chceme cist DATA
        
        FSM_next_state <= state_DATA_inc_mx;

      -- inkrementace nactenych dat
      when state_DATA_inc_mx =>
        MX2_sel <= "01";     -- vybereme DATA_WDATA + 1
        DATA_EN <= '1'; -- zapneme RAM
        MX1_sel <= '1';
        DATA_RDWR <= '1'; -- chceme zapsat DATA_WDATA do RAM

        FSM_next_state <= state_DATA_inc_wb;

      -- ulozeni inkrementovane dat. bunky
      when state_DATA_inc_wb =>
      
        PC_fwd <= '1'; -- dalsi instrukce
        FSM_next_state <= state_fetch; -- exekuce hotova, fetchujeme dalsi instrukci


      -- dekrementace
      -- nacteni dat do DATA_RDATA
      when state_DATA_dec =>
        DATA_EN <= '1';   -- zapneme RAM
        DATA_RDWR <= '0'; -- chceme CIST
        MX1_sel <= '1';    -- chceme cist DATA
        
        FSM_next_state <= state_DATA_dec_mx;

      -- inkrementace nactenych dat
      when state_DATA_dec_mx =>
        MX2_sel <= "10";     -- vybereme DATA_WDATA - 1

        FSM_next_state <= state_DATA_dec_wb;

      -- ulozeni inkrementovane dat. bunky
      when state_DATA_dec_wb =>
        DATA_EN <= '1'; -- zapneme RAM
        DATA_RDWR <= '1'; -- chceme zapsat DATA_WDATA do RAM
        MX1_sel <= '1';
      
        PC_fwd <= '1'; -- dalsi instrukce
        FSM_next_state <= state_fetch; -- exekuce hotova, fetchujeme dalsi instrukci

      when state_DATA_print =>
        DATA_EN <= '1'; -- zapneme RAM
        DATA_RDWR <= '0'; -- chceme CIST
        MX1_sel <= '1';    -- chceme cist DATA

        FSM_next_state <= state_DATA_print_out;

      when state_DATA_print_out =>
        if OUT_BUSY = '1' then
          FSM_next_state <= state_DATA_print;
        else
          OUT_WE <= '1'; -- zapis DATA_RDATA do OUT_DATA

          PC_fwd <= '1'; -- dalsi instrukce
          FSM_next_state <= state_fetch; -- exekuce hotova, fetchujeme dalsi instrukci
        end if;

      when state_DATA_read =>
        DATA_EN <= '1'; -- zapneme RAM
        DATA_RDWR <= '0'; -- chceme CIST
        MX2_sel <= "00";    -- chceme cist IO

        FSM_next_state <= state_DATA_read_wb;

      when state_DATA_read_wb =>
        if IN_VLD = '1' then
          DATA_EN <= '1'; -- zapneme RAM
          MX1_sel <= '1';    -- chceme zapisovat DATA
          DATA_RDWR <= '1'; -- chceme zapisovat

          PC_fwd <= '1'; -- dalsi instrukce
          FSM_next_state <= state_fetch; -- exekuce hotova, fetchujeme dalsi instrukci
        else
          FSM_next_state <= state_DATA_read;
        end if;

      when state_unknown =>
        PC_fwd <= '1'; -- dalsi instrukce
        FSM_next_state <= state_fetch; -- znak ignorujeme, fetchujeme dalsi instrukci

      when state_end =>
        FSM_next_state <= state_end; -- program skoncil, nekonecny cyklus, nic jineho nedelat
    end case;
  end process;


end behavioral;

