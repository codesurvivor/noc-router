-- XY Routing Algorithm Testbench
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity odd_even_tb is
end odd_even_tb;

architecture structure of odd_even_tb is
    component odd_even_algorithm is
        generic( loc_x : integer := 5; 
                 loc_y : integer := 5
        );
        port(
            clk,rst,Enable : in std_logic;
            N_OUT,S_OUT,W_OUT,E_OUT,L_OUT : out std_logic;
            Input : in std_logic_vector(15 downto 0)
        );

    end component;  
    signal clk : std_logic;
    signal rst : std_logic;
    signal Enable : std_logic;
    signal N_OUT,S_OUT,W_OUT,E_OUT,L_OUT : std_logic;
    signal Input : std_logic_vector(15 downto 0);
begin
    odd_even_block : odd_even_algorithm  
    generic map (loc_x => 5,loc_y => 5) 
    port map(
          clk => clk,
          rst => rst, Enable => Enable,
          N_OUT => N_OUT,
          S_OUT => S_OUT,
          W_OUT => W_OUT,
          E_OUT => E_OUT,
          L_OUT => L_OUT,
          Input => Input
    ); 
    clock_generator : process
    begin
        clk <= '0';
        wait for 6 ns;
        clk <= '1';
        wait for 6 ns;    
    end process;
    
    test_process : process
    begin
        rst <= '0';
        Enable <= '0';
        wait for 30 ns;
        
        rst <= '1';
        wait  for 19 ns;
        
        Input <= "0111011000000001";
        wait for 50 ns;
        
        Enable <= '1';
        
        Input <= "0110100100000001";
        wait for 12 ns;
        
        Input <= "0001101100000001";
        wait for 12 ns;
        
        Input <= "1001101100000001";
        wait for 12 ns;
        
        Input <= "0110111100000001";
        wait for 12 ns;
        
        Input <= "0001101100000001";
        wait for 12 ns;
        
        Input <= "1001101100000001";
        wait for 12 ns;
        
        Input <= "0110111100000001";
        wait for 12 ns;
        
        Input <= "0011111111000001";
        wait for 12 ns;
        
        Input <= "1001101100000001";
        wait for 12 ns;
        
        Input <= "0101110100000001";
        wait for 12 ns;
        
        Input <= "0011011111000001";
        wait for 12 ns;
        
        Input <= "1001101100000001";
        wait for 12 ns;
        
        Input <= "0111100100000001";
        wait for 12 ns;
        
        Input <= "0000111100000001";
        wait for 12 ns;
        
        Input <= "1001101100000001";
        wait for 12 ns;
        
    end process;
end;