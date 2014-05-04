-- XY Routing Algorithm
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_signed.all;
use ieee.numeric_std.all;
entity xy_algorithm is
    generic(loc_x : integer := 6; loc_y : integer := 6);
    port(   clk,rst,Enable :                in std_ulogic;
            N_OUT,S_OUT,W_OUT,E_OUT,L_OUT : out std_ulogic;
            Input :                         in std_ulogic_vector(9 downto 0);
            Output :                        out std_ulogic_vector(9 downto 0)
    );
end xy_algorithm;

architecture structure of xy_algorithm is
    component Pipeline is
        generic(length : integer := 32); 
        port(   clk,rst,Enable :  in std_ulogic;
                Input :           in std_ulogic_vector(length-1 downto 0);
                Output :          out std_ulogic_vector(length-1 downto 0)
        );
    end component;
    signal i_result,o_result : std_ulogic_vector(4 downto 0);
    signal internal_data : std_ulogic_vector(9 downto 0);
begin 
    pipe_direction : entity work.Pipeline generic map(length => 5)
    port map(
        clk => clk,
        rst => rst,
        Enable => Enable,
        Input => i_result,
        Output => o_result
    );
    pipe_data : entity work.Pipeline generic map(length => 10)
    port map(
        clk => clk,
        rst => rst,
        Enable => Enable,
        Input => internal_data,
        Output => Output
    );
    processing : process(clk,rst)
    variable cur_x, cur_y : unsigned(2 downto 0);
    variable dst_x, dst_y : unsigned(2 downto 0);
    variable temp : unsigned(4 downto 0);
    variable frame_type : unsigned(1 downto 0);
    begin
    cur_x := to_unsigned(loc_x,3); 
    cur_y := to_unsigned(loc_y,3); 
    dst_x := unsigned(Input(7 downto 5));
    dst_y := unsigned(Input(4 downto 2));
    frame_type := unsigned(Input(9 downto 8));
        if rst = '0' then
            temp := (others => '0');
        elsif Enable = '1' and rising_edge (clk) then 
            case frame_type is
                when "01" => 
                    if cur_x = dst_x and cur_y = dst_y then
                        temp := "10000";
                    elsif cur_x > dst_x then
                        temp := "01000";
                    elsif cur_x < dst_x then
                        temp := "00100";
                    elsif cur_y > dst_y then
                        temp := "00010"; 
                    elsif cur_y < dst_y then 
                        temp := "00001";
                    end if;
                    internal_data <= Input;
                when "00" =>
                    if temp /= "00000" then
                        internal_data <= Input;
                    end if;
                when "10" =>
                    temp := "00000"; 
                    internal_data <= Input;
                when others =>
                    temp := (others => '0');  
            end case;
        end if; 
    i_result <= std_ulogic_vector(temp);   
    end process;
    L_OUT <= o_result(4);
    N_OUT <= o_result(3);
    S_OUT <= o_result(2);
    W_OUT <= o_result(1);
    E_OUT <= o_result(0);
end structure;
