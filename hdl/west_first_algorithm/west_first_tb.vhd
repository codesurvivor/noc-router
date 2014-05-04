-- XY Routing Algorithm Testbench
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity west_first_tb is
end west_first_tb;

architecture structure of west_first_tb is
    component west_first_algorithm is
        generic( 
                seed : integer := 1;
                loc_x : integer := 2; 
                loc_y : integer := 2
        );
        port(
            rst,Enable : in std_logic;
            N_OUT,S_OUT,W_OUT,E_OUT,L_OUT : out std_logic;
            Input : in std_logic_vector(9 downto 0);
            Output : out std_logic_vector(9 downto 0)
        );
    end component;
    signal rst : std_logic;
    signal Enable : std_logic;
    signal N_OUT,S_OUT,W_OUT,E_OUT,L_OUT : std_logic;
    signal Input : std_logic_vector(9 downto 0);
    signal Output : std_logic_vector(9 downto 0);
begin
    nlst_block : entity work.west_first_algorithm 
    generic map (
                seed => 1,
                loc_x => 2,
                loc_y => 2
    ) 
    port map(
          rst => rst,
          Enable => Enable,
          N_OUT => N_OUT, S_OUT => S_OUT,
          W_OUT => W_OUT, E_OUT => E_OUT,
          L_OUT => L_OUT, Input => Input, Output => Output
    ); 
    test_process : process
    begin
        rst <= '0';
        Enable <= '0';
        wait for 30 ns;
        
        rst <= '1';
        wait  for 19 ns;
        
        Input <= "0111011000";
        wait for 50 ns;
        
        Enable <= '1';
        
        Input <= "0100101100"; --west
        wait for 50 ns;
        
        Input <= "0011011000";
        wait for 50 ns;
        
        Input <= "0011010100";
        wait for 50 ns;
        
        Input <= "0001110100";
        wait for 50 ns;
        
        Input <= "0011100100";
        wait for 50 ns;
        
        Input <= "1001101100";
        wait for 50 ns;
        
        Input <= "1101101100";
        wait for 100 ns;
        
        Input <= "0101101000"; -- east
        wait for 50 ns;
        
        Input <= "0011111111";
        wait for 50 ns;
        
        Input <= "0000000000";
        wait for 50 ns;
        
        Input <= "0011111111";
        wait for 50 ns;
        
        Input <= "0000000000";
        wait for 50 ns;
        
        Input <= "1011111111";
        wait for 50 ns;
        
        Input <= "1101101100";
        wait for 100 ns;
        
        Input <= "0101000100"; -- south
        wait for 50 ns;
        
        Input <= "0011111111";
        wait for 50 ns;
        
        Input <= "0000000000";
        wait for 50 ns;
        
        Input <= "0011111111";
        wait for 50 ns;
        
        Input <= "0000000000";
        wait for 50 ns;
        
        Input <= "1011111111";
        wait for 50 ns;
        
        Input <= "1101101100";
        wait for 100 ns;
        
        Input <= "0101101100"; -- north
        wait for 50 ns;
        
        Input <= "0011111111";
        wait for 50 ns;
        
        Input <= "0000000000";
        wait for 50 ns;
        
        Input <= "0011111111";
        wait for 50 ns;
        
        Input <= "0000000000";
        wait for 50 ns;
        
        Input <= "1011111111";
        wait for 50 ns;
        
        Input <= "1101101100";
        wait for 100 ns;
        
    end process;
end;