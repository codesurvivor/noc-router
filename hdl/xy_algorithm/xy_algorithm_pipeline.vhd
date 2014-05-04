library ieee;
use ieee.std_logic_1164.all;

entity Pipeline is
  generic(length : integer := 32); 
  port(   clk,rst,Enable :  in std_ulogic;
          Input :           in std_ulogic_vector(length-1 downto 0);
          Output :          out std_ulogic_vector(length-1 downto 0)
  );
end Pipeline;

architecture structure of Pipeline is 
begin
    pipeline_pro : process(clk,rst,Input)
    variable temp : std_ulogic_vector(length-1 downto 0);
    begin 
      if rst = '0' then
          temp := (others => '0');  
      elsif Enable = '1' and rising_edge(clk) then
          temp := Input;   
      end if;
    end process;
    Output <= Input;
end structure;