-- odd_even Routing Algorithm
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_signed.all;
use ieee.numeric_std.all;
entity odd_even_algorithm is
    generic( loc_x : integer := 5; 
             loc_y : integer := 5
    );
    port(
        clk,rst,Enable : in std_logic;
        N_OUT,S_OUT,W_OUT,E_OUT,L_OUT : out std_logic;
        Input : in std_logic_vector(15 downto 0);
        output : out std_logic_vector(15 downto 0)
    );
end odd_even_algorithm;

architecture structure of odd_even_algorithm is
    component Pipeline is
        generic(
            length : integer
        ); 
        port(
            clk,rst,Enable : in std_logic;
            Input : in std_logic_vector(length-1 downto 0);
            Output : out std_logic_vector(length-1 downto 0)
        );    
    end component;
    signal i_result,o_result : std_logic_vector(4 downto 0);
    signal data_int : std_logic_vector(15 downto 0);
begin
    pipe_register: entity work.Pipeline 
        generic map (
            length => 5
        ) 
    port map(
        clk => clk,
        rst => rst,
        Enable => Enable,
        Input => i_result,
        Output => o_result
    );
    data_register: entity work.Pipeline
        generic map(
            length => 16
        )
        port map(
            clk => clk,
            rst => rst,
            Enable => Enable,
            Input => data_int,
            Output => Output
        );
    processing : process(clk,rst,Input)
    variable src_x, E0,E1 : unsigned(2 downto 0);
    variable cur_x, cur_y : unsigned(2 downto 0);
    variable dst_x, dst_y : unsigned(2 downto 0);
    variable temp : unsigned(4 downto 0);
    variable header : unsigned(1 downto 0);
    begin
    --gV := to_unsigned(gK,3);
    cur_x := to_unsigned(loc_x,3); 
    cur_y := to_unsigned(loc_y,3); 
    dst_x := unsigned(Input(13 downto 11));
    dst_y := unsigned(Input(10 downto 8));
    src_x := unsigned(Input(7 downto 5));
    header := unsigned(Input(15 downto 14));
    E0 := dst_x - cur_x;
    E1 := dst_y - cur_y;
    
    if rst = '0' then
        temp := (others => '0');
        data_int <= (others => '0');
    elsif Enable = '1' and rising_edge (clk) then
        case header is
            when "01" => 
                if E0 = 0 then
                        if E1 > 0 then
                            temp := "01000"; -- north    
                        else
                            temp := "00100"; -- south    
                        end if;
                else
                        if E0 > 0 then
                            if E1 = 0 then
                                temp := "00001"; -- east   
                            else
                                if (((cur_x mod 2) /= 0) or (cur_x = src_x)) then 
                                    if E1 > 0 then
                                        temp := "01000"; -- north    
                                    else
                                        temp := "00100"; -- south    
                                    end if;
                                end if;
                                if (((dst_x mod 2) /= 0) or (E0 /= 1)) then
                                    temp := "00001"; -- east     
                                end if;
                            end if;  
                        else
                            temp := "00010"; -- west  
                            if (cur_x mod 2) = 0 then
                                if E1 > 0 then
                                    temp := "01000"; -- north 
                                else
                                    temp := "00100"; -- south  
                                end if;
                            end if;
                        end if;    
                end if; 
                data_int <= Input;
            when "00" =>
                if temp /= "00000" then
                    data_int <= Input;
                end if;
            when "10" =>
                temp := "00000"; 
                data_int <= Input;
            when others =>
                temp := (others => '0');  
        end case;
    end if; 
        i_result <= std_logic_vector(temp);   
    end process;
        L_OUT <= o_result(4);
        N_OUT <= o_result(3);
        S_OUT <= o_result(2);
        W_OUT <= o_result(1);
        E_OUT <= o_result(0);
end structure;
