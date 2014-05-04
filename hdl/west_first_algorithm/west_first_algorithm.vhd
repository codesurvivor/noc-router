-- XY Routing Algorithm
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_signed.all;
use ieee.numeric_std.all;
entity west_first_algorithm is
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
end west_first_algorithm;

architecture structure of west_first_algorithm is
    component west_first_Pipe is
        generic(
            length : integer := 32
        ); 
        port(
            rst,Enable : in std_logic;
            Input : in std_logic_vector(length-1 downto 0);
            Output : out std_logic_vector(length-1 downto 0)
        );
    end component;
    signal i_result,o_result : std_logic_vector(4 downto 0);
    signal internal_data : std_logic_vector(9 downto 0);
begin 
    nlst_dir : entity work.west_first_Pipe generic map(length => 5)
        port map(
            rst => rst,
            Enable => Enable,
            Input => i_result,
            Output => o_result
        );
    nlst_data : entity work.west_first_Pipe generic map(length => 10)
        port map(
            rst => rst,
            Enable => Enable,
            Input => internal_data,
            Output => Output
        );
    processing : process(rst,Enable,Input)
        variable cur_x, cur_y : signed(2 downto 0);
        variable dst_x, dst_y : signed(2 downto 0);
        variable temp : signed(4 downto 0);
        variable header : signed(1 downto 0);
        variable xOff,yOff : signed(2 downto 0);
        variable cov_temp : signed(63 downto 0);
        variable holdrand  : signed(31 downto 0);
    begin
    cur_x := to_signed(loc_x,3); 
    cur_y := to_signed(loc_y,3); 
    dst_x := signed(Input(7 downto 5));
    dst_y := signed(Input(4 downto 2));
    header := signed(Input(9 downto 8));
    
    holdrand := to_signed(seed,32);
    xOff := dst_x - cur_x;
    yOff := dst_y - cur_y;
    
        if rst = '0' then
            temp := (others => '0');
            holdrand := (others => '0');  
        elsif Enable = '1' then 
          
            cov_temp := (holdrand * 214013 + 2531011);
            holdrand := cov_temp(31 downto 0) srl 16;
            case header is
                when "01" => 
                    if xoff < 0 then
                        temp := "00010"; -- west  
                    elsif (xoff > 0) and (yoff < 0) then
                        if holdrand(15) = '1' then
                            temp := "00001"; -- east
                        else
                            temp := "00100"; -- south
                        end if;   
                    elsif (xoff > 0) and (yoff > 0) then
                        if holdrand(15) = '1' then
                            temp := "00001"; -- east
                        else
                            temp := "01000"; -- north
                        end if;
                    elsif (xoff > 0) and (yoff = 0) then
                        temp := "00001"; -- east   
                    elsif (xoff = 0) and (yoff < 0) then
                        temp := "00100"; -- south    
                    else
                        temp := "01000"; -- north
                    end if;
                    internal_data <= Input;
                when "00" =>
                        internal_data <= Input;
                when "10" => 
                    internal_data <= Input;
                    temp := (others => '0');
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
