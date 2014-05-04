-- XY Routing Algorithm Testbench
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity xy_mesh_tb is
end xy_mesh_tb;

architecture structure of xy_mesh_tb is
    component xy_algorithm is
        generic( loc_x : integer := 6; loc_y : integer := 6);
        port(
            clk,rst,Enable : in std_ulogic;
            N_OUT,S_OUT,W_OUT,E_OUT,L_OUT : out std_ulogic;
            Input : in std_ulogic_vector(9 downto 0);
            Output : out std_ulogic_vector(9 downto 0)
        );
    end component;  
    signal clk : std_ulogic;
    signal rst : std_ulogic;
    signal Enable : std_ulogic;
    signal N_OUT,S_OUT,W_OUT,E_OUT,L_OUT : std_ulogic;
    signal Input : std_ulogic_vector(9 downto 0);
    signal Output : std_ulogic_vector(9 downto 0);
begin
    xy_mesh_block : xy_algorithm 
    generic map (loc_x => 6,loc_y => 6) 
    port map(
          clk => clk,
          rst => rst,
          Enable => Enable,
          N_OUT => N_OUT,
          S_OUT => S_OUT,
          W_OUT => W_OUT,
          E_OUT => E_OUT,
          L_OUT => L_OUT,
          Input => Input,
          Output => Output
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
        
        Input <= "0111011000";
        wait for 50 ns;
        
        Enable <= '1';
        
        Input <= "0110101100";
        wait for 12 ns;
        
        Input <= "0001101100";
        wait for 12 ns;
        
        Input <= "1001101100";
        wait for 12 ns;
        
        Input <= "0110111000";
        wait for 12 ns;
        
        Input <= "0001101100";
        wait for 12 ns;
        
        Input <= "1001101100";
        wait for 12 ns;
        
        Input <= "0110111100";
        wait for 12 ns;
        
        Input <= "0011111111";
        wait for 12 ns;
        
        Input <= "1001101100";
        wait for 12 ns;
        
        Input <= "0101110100";
        wait for 12 ns;
        
        Input <= "0011011111";
        wait for 12 ns;
        
        Input <= "1001101100";
        wait for 12 ns;
        
        Input <= "0111100100";
        wait for 12 ns;
        
        Input <= "0000111100";
        wait for 12 ns;
        
        Input <= "1001101100";
        wait for 12 ns;
                
    end process;
end;