library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity user_logic is
  Port ( a      : in std_logic_vector(31 downto 0);
         b      : in std_logic_vector(31 downto 0);
         z      : out std_logic_vector(31 downto 0);
         req    : in std_logic;
         rdy    : out std_logic;
         ck     : in std_logic;
         reset  : in std_logic
  );
end user_logic;

architecture Behavioral of user_logic is
COMPONENT floating_point_0 IS
  PORT (
    aclk : IN STD_LOGIC;
    s_axis_a_tvalid : IN STD_LOGIC;
    s_axis_a_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    s_axis_b_tvalid : IN STD_LOGIC;
    s_axis_b_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    s_axis_c_tvalid : IN STD_LOGIC;
    s_axis_c_tdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axis_result_tvalid : OUT STD_LOGIC;
    m_axis_result_tdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0)
  );
END COMPONENT;

signal in_tvalid    : std_logic;
signal f_ma_tvalid  : std_logic;
signal f_ma_data    : std_logic_vector(31 downto 0);
signal temp         : std_logic_vector(31 downto 0);

begin

    U:  floating_point_0 
          PORT MAP(
            aclk                => ck,
            s_axis_a_tvalid     => in_tvalid,
            s_axis_a_tdata      => a,
            s_axis_b_tvalid     => in_tvalid,
            s_axis_b_tdata      => b,
            s_axis_c_tvalid     => in_tvalid,
            s_axis_c_tdata      => temp,
            m_axis_result_tvalid => f_ma_tvalid,
            m_axis_result_tdata => f_ma_data
          );
    
    process(ck)
    type state is (wait_on_req, wait_on_req_low, wait_on_f_ma_tvalid, wait_on_f_ma_tvalid_low);
    variable n_s    : state;
    begin
        if ck = '1' and ck'event then
            if reset = '1' then
                temp        <= (others => '0');
                in_tvalid   <= '0';
                rdy         <= '1';
                n_s         := wait_on_req;
            else
                case n_s is
                    when wait_on_req    =>
                        if req = '1' then
                            in_tvalid   <= '0';
                            rdy         <= '0';
                            n_s         := wait_on_req_low;
                        else
                            in_tvalid   <= '0';
                            rdy         <= '1';
                        end if;
                    when wait_on_req_low    =>
                        if req = '0' then
                            in_tvalid   <= '1';
                            rdy         <= '0';
                            n_s         := wait_on_f_ma_tvalid;
                        else
                            in_tvalid   <= '0';
                            rdy         <= '0';
                        end if;
                    when wait_on_f_ma_tvalid    =>
                        in_tvalid       <= '0';
                        rdy             <= '0';
                        if f_ma_tvalid = '1' then
                            n_s         := wait_on_f_ma_tvalid_low;
                        end if;
                    when wait_on_f_ma_tvalid_low    =>
                        if f_ma_tvalid = '0' then
                            temp        <= f_ma_data;
                            in_tvalid   <= '0';
                            rdy         <= '1';
                            n_s         := wait_on_req;
                        else
                            in_tvalid   <= '0';
                            rdy         <= '0';
                        end if;
                end case;
            end if;
        end if;
    end process;
    
    z   <= temp;
              
end Behavioral;


























