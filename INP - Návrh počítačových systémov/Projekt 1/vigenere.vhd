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

	signal OFFSET: std_logic_vector(7 downto 0);
	signal ADDCORRECT: std_logic_vector(7 downto 0);
	signal SUBCORRECT: std_logic_vector(7 downto 0);
	
	type FSMstate is(add,sub);
	signal presentState : FSMstate;
	signal nextState : FSMstate;
	
	signal MEALYOUT: std_logic_vector(1 downto 0);

begin
	
	calculateOffset: process (DATA,KEY) is
		constant basis : std_logic_vector (7 downto 0) := "01000000";
		variable tmp_offset : std_logic_vector(7 downto 0);
		
		begin
		
		tmp_offset := DATA xor basis; -- calculate offset by xor 
		if(tmp_offset > 26) then
			OFFSET <= "00000000";  -- number
		else
			OFFSET <= KEY xor basis; -- calculate offset by xor with 64 base
		end if;
		
	end process;
	
		
	addCorrection : process (OFFSET,DATA) is
		variable sum : std_logic_vector (7 downto 0); -- save sum
		constant Z : std_logic_vector(7 downto 0):= "01011010"; -- Z = 90
		constant AlphabetLength : std_logic_vector(7 downto 0) := "00011010"; -- 26
		constant HASH : std_logic_vector(7 downto 0):= "00100011"; -- #
		
		begin
		
		sum := OFFSET + DATA;
		if(sum > Z) then
			sum := sum - AlphabetLength;
			ADDCORRECT <= sum ;
		else
			if(OFFSET = "00000000") then
				ADDCORRECT <= HASH; -- #
			else
				ADDCORRECT <= sum ;
			end if;
		end if;
		
	end process;
	
	
	subCorrection : process (OFFSET,DATA) is
		variable diff : std_logic_vector (7 downto 0); -- save difference
		constant A : std_logic_vector(7 downto 0):= "01000001";
		constant AlphabetLength : std_logic_vector(7 downto 0) := "00011010";
		constant HASH : std_logic_vector(7 downto 0):= "00100011";
		
		begin
		
		diff := DATA - OFFSET;
		if(diff < A ) then
			if(OFFSET = "00000000") then
				SUBCORRECT <= HASH;
			else
				diff := diff + AlphabetLength;
				SUBCORRECT <= diff;
			end if;
		else
			SUBCORRECT <= diff;
		end if;
		
	end process;
	
	
	presentStateReg : process (RST,CLK) is
	begin
	
		if(RST='1') then
			presentState <= add;
		elsif (CLK'event) and (CLK='1') then
			presentState <= nextState;
		end if;
		
	end process;
	
	
	nextStateLogic: process (DATA,RST,presentState) is
	begin
	
		nextState <= add;
		case presentState is
			when add =>
				MEALYOUT <= "00";
				nextState <= sub;
			when sub =>
				MEALYOUT <= "11";
				nextState <= add;
		end case;
		
		if(RST='1') then
			MEALYOUT <= "10"; -- if rst on then # 
		end if;
			
	end process;
	
	
	mx2t1 : process (ADDCORRECT,SUBCORRECT,MEALYOUT) is
	constant HASH : std_logic_vector(7 downto 0):= "00100011";
	
	begin
	
		if(MEALYOUT = "00")	then
			CODE <= ADDCORRECT;
		elsif (MEALYOUT = "11") then
			CODE <= SUBCORRECT;
		else
			CODE <= HASH;
		end if;
		
	end process;
	
end behavioral;