library ieee;
use ieee.std_logic_1164.all;

entity west_first_Pipe is
  generic(
      length : integer := 32
  ); 
  port(
      rst,Enable : in std_logic;
      Input : in std_logic_vector(length-1 downto 0);
      Output : out std_logic_vector(length-1 downto 0)
  );
end west_first_Pipe;

architecture structure of west_first_Pipe is 
begin
    pipeline_pro : process(rst,Enable,Input)
    variable temp : std_logic_vector(length-1 downto 0);
    begin 
      if rst = '0' then
          temp := (others => '0');  
      elsif Enable = '1' then
          temp := Input;   
      end if;
    end process;
    Output <= Input;
end structure;