proc isim_script {} {

   add_divider "Signals of the Vigenere Interface"
   add_wave_label "" "CLK" /testbench/clk
   add_wave_label "" "RST" /testbench/rst
   add_wave_label "-radix ascii" "DATA" /testbench/tb_data
   add_wave_label "-radix ascii" "KEY" /testbench/tb_key
   add_wave_label "-radix ascii" "CODE" /testbench/tb_code

   # sem doplnte vase vnitrni signaly. chcete-li v diagramu zobrazit desitkove
   # cislo, vlozte do prvnich uvozovek: -radix dec
   add_divider "Vigenere Inner Signals"
   add_wave_label "" "nowState" /testbench/uut/nowState
   add_wave_label "" "nextState" /testbench/uut/nextState
   add_wave_label "-radix dec" "POSUV" /testbench/uut/shift
   add_wave_label "-radix ascii" "PLUS" /testbench/uut/plusKorekce
   add_wave_label "-radix ascii" "MINUS" /testbench/uut/minusKorekce
   add_wave_label "" "FSM VYSTUP" /testbench/uut/fsmOut
   

   run 8 ns
}
