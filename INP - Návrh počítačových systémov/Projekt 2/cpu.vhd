-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2020 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Samuel Valaštín
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WE    : out std_logic;                    -- cteni (0) / zapis (1)
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

 signal PROG_COUNTER      : std_logic_vector(11 downto 0);
 signal PROG_COUNTER_INC  : std_logic;
 signal PROG_COUNTER_DEC  : std_logic;
 signal PROG_COUNTER_LOAD : std_logic;
 
 signal PTR_COUNTER       : std_logic_vector(9 downto 0);
 signal PTR_COUNTER_INC   : std_logic;
 signal PTR_COUNTER_DEC   : std_logic;
 
 signal RET_ADDR			  : std_logic_vector(11 downto 0);
 signal RET_ADDR_PUSH     : std_logic;
 signal RET_ADDR_POP      : std_logic;
 
 ------------ MEALY STATE MACHINE -----------------
 type FSMstate is(
			------- CPU STATES ----------
			INIT_STATE,
			FETCH_STATE,
			DECODE_STATE,
			STOP_STATE,
			------------ > -------------
			PTR_VAL_INC,
			------------ < -------------
			PTR_VAL_DEC,
			------------ + -------------
			PTR_CELL_INC_READ,
			PTR_CELL_INC_WRITE,
			------------ - -------------
			PTR_CELL_DEC_READ,
			PTR_CELL_DEC_WRITE,
			---------- CYCLE -----------
			WHILE_START,
			WHILE_PTR,
			WHILE_SKIP,
			WHILE_END,
			WHILE_POP,
			---------- PRINT ------------
			PRINT_CHECK,
			PRINT,
			---------- LOAD ------------
			LOAD_CHECK,
			LOAD,
			----------------------------
			OTHER_INST );
 signal PRESENT_STATE : FSMstate;
 signal NEXT_STATE : FSMstate;
 ------------------------------------------------
 
 ---------------- MX SELECTOR -------------------
 signal SEL : std_logic_vector (1 downto 0);
 ------------------------------------------------
begin

	program_counter : process(RESET,CLK,PROG_COUNTER_INC,PROG_COUNTER_DEC,PROG_COUNTER_LOAD) is
		begin
			if(RESET = '1') then
				PROG_COUNTER <= "000000000000";
			elsif (CLK'event) and (CLK='1') then
				if (PROG_COUNTER_INC = '1') then
					PROG_COUNTER <= PROG_COUNTER + 1;
				elsif (PROG_COUNTER_DEC = '1') then
					PROG_COUNTER <= PROG_COUNTER - 1;
				elsif (PROG_COUNTER_LOAD = '1') then
					PROG_COUNTER <= RET_ADDR;
				end if;
			end if;
		end process;
		CODE_ADDR <= PROG_COUNTER;
	
	
	pointer_counter : process (RESET,CLK,PTR_COUNTER_INC,PTR_COUNTER_DEC) is
		begin
			if(RESET = '1') then
				PTR_COUNTER <= "0000000000";
			elsif (CLK' event) and (CLK = '1') then
				if(PTR_COUNTER_INC = '1') then
					PTR_COUNTER <= PTR_COUNTER +1;
				elsif (PTR_COUNTER_DEC = '1') then
					PTR_COUNTER <= PTR_COUNTER -1;
				end if;
			end if;
		end process;
		DATA_ADDR <= PTR_COUNTER;
	
	
	mx4t1: process(SEL,IN_DATA,DATA_RDATA) is
		begin
			if (SEL = "00") then
				DATA_WDATA <= IN_DATA;
			elsif(SEL = "01") then
				DATA_WDATA <= DATA_RDATA - 1;
			elsif(SEL = "10") then
				DATA_WDATA <= DATA_RDATA + 1;
			else
				DATA_WDATA <= DATA_RDATA;
			end if;
		end process;
		
		
	return_adress : process(RESET,CLK,RET_ADDR_PUSH,RET_ADDR_POP) is
		begin
			if(RESET = '1') then
				RET_ADDR <= "000000000000";
			elsif (CLK' event) and (CLK ='1') then
				if(RET_ADDR_PUSH = '1') then
					RET_ADDR <= PROG_COUNTER + 1;
				elsif( RET_ADDR_POP = '1') then
					RET_ADDR <= "000000000000";
				end if;
			end if;
		end process;
	
	
	present_state_reg : process(EN,CLK,RESET) is
		begin
			if(RESET ='1') then
				PRESENT_STATE <= INIT_STATE;
			elsif(CLK' event) and (CLK = '1') then
				if (EN = '1') then
					PRESENT_STATE <= NEXT_STATE;
				end if;
			end if;
		end process;
		
		
	next_state_logic : process (CODE_DATA, PRESENT_STATE, IN_VLD, OUT_BUSY, EN,DATA_RDATA) is
	   ------------------------------ SET INSTRUCTION CONSTANTS --------------------------------
		constant INST_INC     :  std_logic_vector(7 downto 0) := "00111110"; -- 0x3E (decimal 62)
		constant INST_DEC     :  std_logic_vector(7 downto 0) := "00111100"; -- 0x3C (decimal 60)
		constant INST_PLUS    :  std_logic_vector(7 downto 0) := "00101011"; -- 0x2B (decimal 43)
		constant INST_MINUS   :  std_logic_vector(7 downto 0) := "00101101"; -- 0x2D (decimal 45)
		constant INST_WHILEST :  std_logic_vector(7 downto 0) := "01011011"; -- 0x5B (decimal 91)
		constant INST_WHILEND :  std_logic_vector(7 downto 0) := "01011101"; -- 0x5D (decimal 93)
		constant INST_PRINT   :  std_logic_vector(7 downto 0) := "00101110"; -- 0x2E (decimal 46)
		constant INST_LOAD    :  std_logic_vector(7 downto 0) := "00101100"; -- 0x2C (decimal 44)
		constant INST_NULL    :  std_logic_vector(7 downto 0) := "00000000"; -- 0x00 (decimal 0)
		-----------------------------------------------------------------------------------------
		begin
			------- OWN VARIABLES INIT ----------
			PROG_COUNTER_INC  <= '0';
			PROG_COUNTER_DEC  <= '0';
			PROG_COUNTER_LOAD <= '0';
			PTR_COUNTER_INC 	<= '0';
			PTR_COUNTER_DEC	<= '0';
			RET_ADDR_PUSH		<= '0';
			RET_ADDR_POP		<= '0';
			SEL					<= "11";
			-------------------------------------
			------ CPU OUT VARIABLES INIT -------
			CODE_EN				<= '0';
			DATA_WE				<= '0';
			DATA_EN				<= '0';
			IN_REQ				<= '0';
			OUT_WE				<= '0';
			-------------------------------------
			case PRESENT_STATE is
				when INIT_STATE   =>
					NEXT_STATE <= FETCH_STATE;
				when FETCH_STATE  =>
					NEXT_STATE <= DECODE_STATE;
					CODE_EN 	  <= '1'; -- povolenie èinnosti
				when DECODE_STATE =>
					case CODE_DATA is
						when INST_INC 		=>		NEXT_STATE <= PTR_VAL_INC;
						when INST_DEC 		=>		NEXT_STATE <= PTR_VAL_DEC;
						when INST_PLUS 	=>		NEXT_STATE <= PTR_CELL_INC_READ;
						when INST_MINUS 	=>		NEXT_STATE <= PTR_CELL_DEC_READ;
						when INST_WHILEST =>		NEXT_STATE <= WHILE_START;
						when INST_WHILEND =>		NEXT_STATE <= WHILE_END;
						when INST_PRINT 	=>		NEXT_STATE <= PRINT_CHECK;
						when INST_LOAD 	=>		NEXT_STATE <= LOAD_CHECK;
						when INST_NULL 	=>		NEXT_STATE <= STOP_STATE;
						when others 		=> 	NEXT_STATE <= OTHER_INST;
					end case;
					
				when PTR_VAL_INC  =>
					PTR_COUNTER_INC <= '1';				-- PTR = PTR+1
					PROG_COUNTER_INC <= '1';			-- PC = PC+1
					NEXT_STATE <= FETCH_STATE;	
					
				when PTR_VAL_DEC  =>
					PTR_COUNTER_DEC 	<= '1';			-- PTR = PTR-1
					PROG_COUNTER_INC <= '1';			-- PC = PC+1
				   NEXT_STATE <= FETCH_STATE;
					
				when PTR_CELL_INC_READ =>
					DATA_EN <= '1';			-- povolenie èinnosti			
					NEXT_STATE <= PTR_CELL_INC_WRITE;
					
				when PTR_CELL_INC_WRITE =>
					DATA_EN <= '1';			-- povolenie èinnosti
					DATA_WE <= '1';			-- zápis povoleny
					PROG_COUNTER_INC <= '1';	-- PC = PC+1
					SEL <= "10";					-- ram[ptr] <= DATA_RDATA+1
					NEXT_STATE <= FETCH_STATE;	
					
				when PTR_CELL_DEC_READ =>
					DATA_EN <= '1';			-- povolenie èinnosti
					NEXT_STATE <= PTR_CELL_DEC_WRITE;
					
				when PTR_CELL_DEC_WRITE =>		
					DATA_EN <= '1';				-- povolenie èinnosti
					DATA_WE  <= '1';				-- zápis povoleny
					PROG_COUNTER_INC <= '1';	-- PC = PC+1
					SEL	  <= "01";				-- ram[ptr] <= DATA_RDATA-1
					NEXT_STATE <= FETCH_STATE;
					
				when PRINT_CHECK 	=>
					if(OUT_BUSY = '0') then
						DATA_EN <= '1';				-- povolenie èinnosti
						NEXT_STATE <= PRINT;
					else
						NEXT_STATE <= PRINT_CHECK;
					end if;
					
				when PRINT	=>
					OUT_WE  <= '1';
					PROG_COUNTER_INC <= '1';	-- PC = PC+1
					NEXT_STATE <= FETCH_STATE;	
					
				when LOAD_CHECK =>
					IN_REQ <= '1';
					if (IN_VLD = '0') then			-- kontrola validity vstupu cyklime kym vstup nie je validny
						NEXT_STATE <= LOAD_CHECK;
					else
						NEXT_STATE <= LOAD;
					end if;		
					
				when LOAD =>
					IN_REQ <= '1';
					DATA_EN <= '1';
					DATA_WE  <= '1';				-- povolenie zapisu
					SEL	  <= "00";				--ram[ptr] <= IN_DATA
					PROG_COUNTER_INC <= '1';	-- PC = PC+1
					NEXT_STATE <= FETCH_STATE;	
					
				when WHILE_START	=>
					PROG_COUNTER_INC <= '1';	-- PC = PC+1
					RET_ADDR_PUSH 	  <= '1';	-- RAS[0] = PC
					DATA_EN <= '1';
					NEXT_STATE <= WHILE_PTR;
					
				when WHILE_PTR  =>
					if(DATA_RDATA = "00000000") then
						NEXT_STATE <= WHILE_SKIP;	
					else
						NEXT_STATE <= FETCH_STATE;
					end if;	
					
				when WHILE_SKIP =>
					if(CODE_DATA = INST_WHILEND) then
						NEXT_STATE <= WHILE_POP;	-- narazili sme na uzatvaraciu zatvorku popneme navratovu adresu
					else
						CODE_EN <= '1';
						PROG_COUNTER_INC <= '1';	-- PC = PC+1
						NEXT_STATE <= WHILE_SKIP;	-- prechádzame pokial nenarazíme na uzatvaraciu zátvorku
					end if;		
						when WHILE_END  =>
					if(DATA_RDATA = "00000000") then
						PROG_COUNTER_INC <= '1';	-- PC = PC+1
						NEXT_STATE <= WHILE_POP;	-- popneme navratovu adresu a vychadzame z cyklu
					else
						PROG_COUNTER_LOAD <= '1';	-- nacitame do programoveho citaca navratovu adresu
						NEXT_STATE <= FETCH_STATE; 
					end if;
					
				when WHILE_POP =>
					RET_ADDR_POP  <= '1';			-- inicializujeme navratovu adresu na 0
					NEXT_STATE <= FETCH_STATE;
					
				when STOP_STATE	=>
					NEXT_STATE <= STOP_STATE;		-- koniec		
				when OTHER_INST =>
					PROG_COUNTER_INC <= '1';	-- PC = PC+1
					NEXT_STATE <= FETCH_STATE;
					
				when others => NEXT_STATE <= STOP_STATE;	
			end case;
		end process;
		OUT_DATA <= DATA_RDATA;		-- LCD <- OUT_DATA
end behavioral;

