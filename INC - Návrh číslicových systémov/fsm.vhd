-- fsm.vhd: Finite State Machine
-- Author(s): Samuel Valaštín <xvalas10@stud.fit.vutbr.cz>
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
   type t_state is (STT_1, STT_2, STT_3, STT_4, STT_5_1, STT_5_2, STT_6_1, STT_6_2, STT_7_1, STT_7_2, STT_8_1, 
						  STT_8_2,STT_9_1, STT_9_2, STT_10_1, STT_10_2, STT_END, STT_ERROR, STT_PRINT_OK, STT_PRINT_ERR, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= STT_1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- xvalas10 : kod1 = 1257672526 	 kod2 = 1255732965
   when STT_1 =>
      next_state <= STT_1;
      if (KEY(1) = '1') then
         next_state <= STT_2;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- 12
	 when STT_2 =>
      next_state <= STT_2;
      if (KEY(2) = '1') then
         next_state <= STT_3;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- 125
	when STT_3 =>
      next_state <= STT_3;
      if (KEY(5) = '1') then
         next_state <= STT_4;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- rozvetvenie 1257 && 1255
	when STT_4 =>
      next_state <= STT_4;
      if (KEY(7) = '1') then
         next_state <= STT_5_1;
		elsif (KEY(5) = '1') then
			next_state <= STT_5_2;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- k�d 1 pokra�ovanie 12576
	when STT_5_1 =>
      next_state <= STT_5_1;
      if (KEY(6) = '1') then
         next_state <= STT_6_1;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- k�d 2 pokra�ovanie 12557
	when STT_5_2 =>
      next_state <= STT_5_2;
      if (KEY(7) = '1') then
         next_state <= STT_6_2;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- k�d1 125767 
	when STT_6_1 =>
      next_state <= STT_6_1;
      if (KEY(7) = '1') then
         next_state <= STT_7_1;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	--k�d2 125573
	when STT_6_2 =>
      next_state <= STT_6_2;
      if (KEY(3) = '1') then
         next_state <= STT_7_2;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- k�d1 1257672 
	when STT_7_1 =>
      next_state <= STT_7_1;
      if (KEY(2) = '1') then
         next_state <= STT_8_1;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	--k�d2 1255732
	when STT_7_2 =>
      next_state <= STT_7_2;
      if (KEY(2) = '1') then
         next_state <= STT_8_2;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	  -- - - - - - - - - - - - - - - - - - - - - - -
	-- k�d1 12576725 
	when STT_8_1 =>
      next_state <= STT_8_1;
      if (KEY(5) = '1') then
         next_state <= STT_9_1;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	--k�d2 12557329
	when STT_8_2 =>
      next_state <= STT_8_2;
      if (KEY(9) = '1') then
         next_state <= STT_9_2;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1: 125767252
	when STT_9_1 =>
      next_state <= STT_9_1;
      if (KEY(2) = '1') then
         next_state <= STT_10_1;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- kod2: 125573296
	when STT_9_2 =>
      next_state <= STT_9_2;
      if (KEY(6) = '1') then
         next_state <= STT_10_2;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- kod1: 1257672526
	when STT_10_1 =>
      next_state <= STT_10_1;
      if (KEY(6) = '1') then
         next_state <= STT_END;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- kod2: 1255732965
	when STT_10_2 =>
      next_state <= STT_10_2;
      if (KEY(5) = '1') then
         next_state <= STT_END;
		elsif (KEY(15) = '1') then
			next_state <= STT_PRINT_ERR;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
  -- - - - - - - - - - - - - - - - - - - - - - -
  -- ukoncenie vstupu #  
  when STT_END =>
      next_state <= STT_END;
      if (KEY(15) = '1') then
         next_state <= STT_PRINT_OK;
		elsif (KEY(14 downto 0) /= "000000000000000") then
	      next_state <= STT_ERROR;
      end if;
  -- - - - - - - - - - - - - - - - - - - - - - -	
  -- chybov� stav 
  when STT_ERROR =>
      next_state <= STT_ERROR;
      if (KEY(15) = '1') then
         next_state <= STT_PRINT_ERR;
      end if;
  -- - - - - - - - - - - - - - - - - - - - - - -
  -- spracovanie chyboveho stavu aktiv�cia vstupneho sign�lu cnt_of
	when STT_PRINT_ERR =>
		next_state <= STT_PRINT_ERR;
		if (CNT_OF = '1') then
			next_state <= FINISH;
		end if;
 -- - - - - - - - - - - - - - - - - - - - - - -		
 -- spracovanie validneho vstupu aktiv�cia vstupneho sign�lu cnt_of
	when STT_PRINT_OK =>
		next_state <= STT_PRINT_OK;
		if (CNT_OF = '1') then
			next_state <= FINISH;
		end if;
 -- - - - - - - - - - - - - - - - - - - - - - -			
 -- finish, next state = state 1 
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= STT_1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= STT_1;
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
	-- vy�isti LCD
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- pristup odopren�, FMS_MX_MEM ostav� nastaven� na '0' 
   when STT_PRINT_ERR =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
	-- vyp� povolen�, nastav�me FSM_MX_MEM na '1',rovnako v�etky ostatn� v�stupy na '1'
	when STT_PRINT_OK =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
		FSM_MX_MEM     <= '1';   
	-- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
	--  write * multiplexory s� '0';
		if (KEY(14 downto 0) /= "000000000000000") then
			FSM_LCD_WR     <= '1';
		elsif (KEY(15) = '1') then
			FSM_LCD_CLR    <= '1';
		end if;
     
   end case;
end process output_logic;

end architecture behavioral;

