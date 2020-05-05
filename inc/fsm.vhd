-- fsm.vhd: Finite State Machine
-- Author(s): Roland Schulz (xschul06)
-- Date: 2020-05-01
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (T_START, T_ERR, T_1, T_17, T_171, T_1713, T_17135, T_171357, T_171359, T_1713576, T_1713594, T_17135764, T_17135940, T_171357645, T_171359404, T_1713576453, T_1713594040, PRINT_ERR, PRINT_OK, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= T_START;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- kod1=17135 76453
-- kod2=17135 94040

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T_START =>
      next_state <= T_START;
		if (KEY(1)  = '1') then
			next_state <= T_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when T_ERR =>
      next_state <= T_ERR;
      if (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when T_1 =>
      next_state <= T_1;
		if (KEY(7)  = '1') then
			next_state <= T_17;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when T_17 =>
      next_state <= T_17;
		if (KEY(1)  = '1') then
			next_state <= T_171;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when T_171 =>
      next_state <= T_171;
		if (KEY(3)  = '1') then
			next_state <= T_1713;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when T_1713 =>
      next_state <= T_1713;
		if (KEY(5)  = '1') then
			next_state <= T_17135;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_17135 =>
      next_state <= T_17135;
		if (KEY(7)  = '1') then
			next_state <= T_171357;
		elsif (KEY(9)  = '1') then
			next_state <= T_171359;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_171357 =>
      next_state <= T_171357;
		if (KEY(6)  = '1') then
			next_state <= T_1713576;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_171359 =>
      next_state <= T_171359;
		if (KEY(4)  = '1') then
			next_state <= T_1713594;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_1713576 =>
      next_state <= T_1713576;
		if (KEY(4)  = '1') then
			next_state <= T_17135764;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_1713594 =>
      next_state <= T_1713594;
		if (KEY(0)  = '1') then
			next_state <= T_17135940;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_17135764 =>
      next_state <= T_17135764;
		if (KEY(5)  = '1') then
			next_state <= T_171357645;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_17135940 =>
      next_state <= T_17135940;
		if (KEY(4)  = '1') then
			next_state <= T_171359404;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_171357645 =>
      next_state <= T_171357645;
		if (KEY(3)  = '1') then
			next_state <= T_1713576453;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - - 
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_171359404 =>
      next_state <= T_171359404;
		if (KEY(0)  = '1') then
			next_state <= T_1713594040;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_ERR;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - - konec
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_1713576453 =>
      next_state <= T_1713576453;
      if (KEY(15) = '1') then
         next_state <= PRINT_OK;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - - konec
	-- kod1=17135 76453
	-- kod2=17135 94040
   when T_1713594040 =>
      next_state <= T_1713594040;
      if (KEY(15) = '1') then
         next_state <= PRINT_OK;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= T_ERR;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ERR =>
      next_state <= PRINT_ERR;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_OK =>
      next_state <= PRINT_OK;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= T_START; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= T_START;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when T_START | T_ERR | T_1 | T_17 | T_171 | T_1713 | T_17135 | T_171357 | T_171359 | T_1713576 | T_1713594 | T_17135764 | T_17135940 | T_171357645 | T_171359404 | T_1713576453 | T_1713594040 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_ERR =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
	when PRINT_OK =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
		FSM_MX_MEM     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
   end case;
end process output_logic;

end architecture behavioral;

