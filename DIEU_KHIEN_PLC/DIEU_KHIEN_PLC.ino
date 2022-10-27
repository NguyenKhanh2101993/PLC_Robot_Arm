#include "ModbusMaster.h"
#include "ST7920GLCD.h"
#include "Keypad.h"
#include "defineButton.h"
#include "defineModbusPLC.h"
#include "robot09.h"
//#include "robot25.h"
//#include "robot23.h"


const uint8_t ROWS = 7;
const uint8_t COLS = 5;
char keys[ROWS][COLS] = {
  {'1','3','5','7','9'},
  {'2','4','6','8','0'},
  {'A','B','C','D','E'},
  {'F','#','H','I','J'},
  {'K','L','M','N','O'},
  {'P','Q','R','S','T'},
  {'W','X','Y','Z','G'},
};
byte rowPins[ROWS] = {37, 36, 35, 34, 33, 43, 44}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {49, 48, 47, 46, 45}; //connect to the column pinouts of the keypad

int8_t COMMAND = NO_KEY_ACTIVE; // hien thi cac phim chuc nang keypad
int8_t COMMAND_HOLD = NO_KEY_ACTIVE;
int8_t COMMAND_RELEASE = NO_KEY_ACTIVE;
int8_t commandKeyPad = NO_KEY_ACTIVE;
int8_t stateRobot = -1; int8_t manualAction = -1;
uint16_t readInputSignals;
char *strReadInputSignals[16] = {};
//===========================================================
Keypad KEYPAD = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
ModbusMaster            NODE;
ST7920GLCD              GLCD;

//================================================================
// Hàm quét phím đơn: mỗi thời điểm chỉ nhận tín hiệu của 1 phím
//================================================================
int8_t getSingleKeypad(uint8_t state){
  int8_t result = NO_KEY_ACTIVE;
  result = KEYPAD.getKey(state);
  return result;
}
//================================================================
void Show_Infor(void){
  GLCD.glcd_init_text_mode();
  GLCD.text_display_clear();
  GLCD.printTxt(0,0,"KHOI NGUYEN .JSC"); 
  GLCD.printTxt(1,0,"INJECTION ROBOT"); 
  if (PLC_SLAVE_ADDR == 25) {
    GLCD.printTxt(2,1,"ROBOT M25"); 
    GLCD.printTxt(3,0,"HARMO HEC 250G"); 
  }
  if (PLC_SLAVE_ADDR == 23){
    GLCD.printTxt(2,1,"ROBOT M23"); 
    GLCD.printTxt(3,0,"FUCHUSHIN 450T"); 
  }
  if (PLC_SLAVE_ADDR == 9) {
    GLCD.printTxt(2,1,"ROBOT M09"); 
    GLCD.printTxt(3,0,"FUCHUSHIN 520T");
  }
}
//================================================================
//================================================================
void Remove_Pointer_Program(int x, int y){
  //=======================================
  if (x == 0 && y == 0) { GLCD.printTxt(1,0," ");}
  if (x == 1 && y == 0) { GLCD.printTxt(1,3," ");}
  if (x == 2 && y == 0) { GLCD.printTxt(1,6," ");}
//=======================================
  if (x == 0 && y == 1) { GLCD.printTxt(2,0," ");}
  if (x == 1 && y == 1) { GLCD.printTxt(2,3," ");}
  if (x == 2 && y == 1) { GLCD.printTxt(2,6," ");} 
//=======================================
  if (x == 0 && y == 2) { GLCD.printTxt(3,0," ");}
  if (x == 1 && y == 2) { GLCD.printTxt(3,3," ");}
  if (x == 2 && y == 2) { GLCD.printTxt(3,6," ");}
}
//============================================================================================
// Hiện thị con trỏ trước tên chương trình
uint8_t Show_Pointer_Program(int x, int y){
  static int8_t x_prev = 0; static int8_t y_prev = 0;
  uint8_t pos = 0xFF;
  if (x_prev != x || y_prev != y){
    Remove_Pointer_Program(x_prev, y_prev); x_prev = x; y_prev = y;
  }
//=======================================
  if (x == 0 && y == 0) { GLCD.printTxt(1,0,">"); pos = 1;}
  if (x == 1 && y == 0) { GLCD.printTxt(1,3,">"); pos = 2;}
  if (x == 2 && y == 0) { GLCD.printTxt(1,6,">"); pos = 3;}
//=======================================
  if (x == 0 && y == 1) { GLCD.printTxt(2,0,">"); pos = 4;}
  if (x == 1 && y == 1) { GLCD.printTxt(2,3,">"); pos = 5;}
  if (x == 2 && y == 1) { GLCD.printTxt(2,6,">"); pos = 6;} 
//=======================================
  if (x == 0 && y == 2) { GLCD.printTxt(3,0,">"); pos = 7;}
  if (x == 1 && y == 2) { GLCD.printTxt(3,3,">"); pos = 8;}
  if (x == 2 && y == 2) { GLCD.printTxt(3,6,">"); pos = 9;}
  return pos;
}
//============================================================================================
//============================================================================================
void Remove_Pointer_Function(int x, int y){
//=======================================
  if (x == 0 && y == 0) { GLCD.printTxt(1,0," ");}
  if (x == 1 && y == 0) { GLCD.printTxt(1,4," ");}
//=======================================
  if (x == 0 && y == 1) { GLCD.printTxt(2,0," ");}
  if (x == 1 && y == 1) { GLCD.printTxt(2,4," ");}
//=======================================
  if (x == 0 && y == 2) { GLCD.printTxt(3,0," ");}
  if (x == 1 && y == 2) { GLCD.printTxt(3,4," ");}
}
//============================================================================================
// Hiện thị con trỏ trước tên chương trình
uint8_t Show_Pointer_Function(int x, int y){
  static int8_t x_prev = 0; static int8_t y_prev = 0;
  uint8_t pos = 0xFF;
  if (x_prev != x || y_prev != y){
    Remove_Pointer_Function(x_prev, y_prev);  x_prev = x; y_prev = y;
  }
//=======================================
  if (x == 0 && y == 0) { GLCD.printTxt(1,0,">"); pos = 1;}
  if (x == 1 && y == 0) { GLCD.printTxt(1,4,">"); pos = 2;}
//=======================================
  if (x == 0 && y == 1) { GLCD.printTxt(2,0,">"); pos = 3;}
  if (x == 1 && y == 1) { GLCD.printTxt(2,4,">"); pos = 4;}
//=======================================
  if (x == 0 && y == 2) { GLCD.printTxt(3,0,">"); pos = 5;}
  if (x == 1 && y == 2) { GLCD.printTxt(3,4,">"); pos = 6;}
  return pos;
}
//============================================================================================
void Show_list_modify_plc(void){
  GLCD.text_display_clear();
  GLCD.printTxt(0,0," Che do Modify");
  GLCD.printTxt(1,0,"  Cai Timer    ");
  GLCD.printTxt(2,0,"  Cai Tay Xoay ");
  GLCD.printTxt(3,0,"  Cai KiemTra SP");
}
//============================================================================================
//============================================================================================
// Ghi giá trị 1 word value vào eeprom board slave bắt đầu ở địa chỉ addr
// Kiểm tra giá trị ghi vào eeprom board slave đúng hay chưa
uint8_t Write_single_data_to_Eeprom_slave_board(uint16_t value, uint16_t addr){
    uint16_t value_read; uint8_t status = 1; 
    return status;
}
//============================================================================================
// Đọc giá trị 1 word data từ eeprom board slave bắt đầu ở địa chỉ addr
uint16_t Read_single_data_from_Eeprom_slave_board(uint16_t addr){
    uint8_t result = 1;

    return 1;
}
//============================================================================================
// Ghi data vào eeprom của board slave
// Set bit thứ epprom_type của COIL[] board slaver để chọn đối tượng cần ghi giá trị
// index: số word cần write
// addr: địa chỉ bắt đầu write
uint8_t Write_packet_data_to_Eeprom_slave_board(uint16_t *data, uint8_t index, uint16_t addr){
    uint8_t result = 1; uint8_t status = 0;
    uint16_t *value_read;
  return status;
}
//============================================================================================
// Đọc data eeprom của board slave
// Set bit thứ epprom_type của COIL[] board slaver để chọn đối tượng cần đọc giá trị
// index: số word cần đọc
// addr: địa chỉ bắt đầu đọc
uint16_t *Read_packet_data_from_Eeprom_slave_board(uint8_t index, uint16_t addr){
    static uint16_t data_receive[4]; // Khai báo static uint16_t *data_receive bị lỗi
    return data_receive;
}
//============================================================================================
// Ghi data vào register output của board slave
// Set bit thứ register_name của COIL[] board slaver để chọn đối tượng cần ghi giá trị
// index: số word cần write
// addr: địa chỉ bắt đầu write: 0
void Write_packet_data_to_output_slave_board(uint16_t *data, uint8_t index){
    uint8_t result = 1;
}
//============================================================================================
// Đọc data từ feedback register của board slave để monitor
// index: số word cần read
// addr: địa chỉ bắt đầu read: SINGLE_REGISTER_ADDR
uint16_t *Read_packet_data_from_feedback_register_slave_board(uint8_t index){
    static uint16_t data_receive[4]; // Khai báo static uint16_t *data_receive bị lỗi
    uint8_t result = 1;
    return data_receive;
}
//============================================================================================
// Set coil PLC bắt đầu ở địa chỉ addr, state: 1 = ON, 0 = OFF
uint8_t Write_SingleCoil_PLC(uint16_t addr, uint8_t state){
    uint8_t result = 1;
    result = NODE.writeSingleCoil(addr, state); 
    return result;
}
//============================================================================================
// Đọc giá trị Coils của slave bắt đầu từ địa chỉ addr
// trả về giá trị tối đa 16 bit
uint16_t Read_SingleCoil_PLC(uint16_t addr, uint16_t bitQty){
    uint8_t result = 1; 
    static uint16_t value;
    result = NODE.readCoils(addr, bitQty);
    if (result == 0){
          value = NODE.getResponseBuffer(0);
      }
    NODE.clearResponseBuffer();
    return value;
}
//============================================================================================
// Đọc giá trị coil X bắt đầu từ M125 tới M140
// Đọc giá trị input từ PLC.
uint16_t read_input_from_PLC(void){
    uint16_t dataInput[2];
    static uint16_t resultInput;
    //dataInput[0] = Read_SingleCoil_PLC(MONITOR_INPUT_START_ADDR, 8);
    //dataInput[1] = Read_SingleCoil_PLC(MONITOR_INPUT_START_ADDR + 8, 8);
    //resultInput = (dataInput[1] << 8) | (dataInput[0]);
    resultInput = Read_SingleCoil_PLC(MONITOR_INPUT_START_ADDR, 16);
    Serial.println(resultInput);
    return resultInput;
}
//============================================================================================
// Set coils tay xoay trên PLC ở các vị trí tương ứng, state = XOAY_XUONG/XoayLen
void Write_Coil_Modify_TayXoay(uint8_t pos, uint8_t state){
    uint8_t value = (state == XOAY_XUONG) ? ON: OFF;
    if (pos == VT_CHO_SP) { Write_SingleCoil_PLC(TAYXOAYXUONG_vtCHO_ADDR, value); Write_SingleCoil_PLC(TAYXOAYLEN_vtCHO_ADDR, !value);}
    if (pos == VT_RUT_SP) { Write_SingleCoil_PLC(TAYXOAYXUONG_vtRUT_ADDR, value); Write_SingleCoil_PLC(TAYXOAYLEN_vtRUT_ADDR, !value);}
    if (pos == VT_RUT_LEN) {Write_SingleCoil_PLC(TAYXOAYXUONG_vtLEN_ADDR, value); Write_SingleCoil_PLC(TAYXOAYLEN_vtLEN_ADDR, !value);}
    if (pos == VT_THA_SP)  {Write_SingleCoil_PLC(TAYXOAYXUONG_vtTHA_ADDR, value); Write_SingleCoil_PLC(TAYXOAYLEN_vtTHA_ADDR, !value);}
}
//============================================================================================
// Đọc giá trị coils tay xoay của PLC
// đọc giá trị từ M523 tới M530 tổng là 8 bit
uint8_t Read_Coil_Modify_TayXoay(void){
    static uint8_t bitValue;
    uint16_t value_tayxoay = Read_SingleCoil_PLC(TAYXOAYLEN_vtTHA_ADDR, 8);
    bitValue = value_tayxoay;
    //Serial.println(bitValue);
    return bitValue;
}
//============================================================================================
// Đọc giá trị coils chọn cảm biến phát hiện sản phẩm
// Đọc 3 bit M533 - M535
uint8_t Read_Coil_Phat_Hien_SP(void){
    static uint8_t bitValue;
    uint16_t value_phathienSP = Read_SingleCoil_PLC(CB_PHAT_HIEN_SP_ADDR, 3);
    bitValue = value_phathienSP;
    //Serial.println(bitValue);
    return bitValue;
}
//============================================================================================
// Ghi giá trị set bit chọn cảm biến phát hiện sản phẩm
// Ghi 3 bit M533 - M535
void Write_Coil_Phat_Hien_SP(uint16_t addr, uint8_t value){
    if (addr == CB_PHAT_HIEN_SP_ADDR) { 
      Write_SingleCoil_PLC(CB_PHAT_HIEN_SP_ADDR, value); Write_SingleCoil_PLC(CB_CHAN_KHONG_SP_ADDR, !value); Write_SingleCoil_PLC(CB_KEP_SP_ADDR, !value);}
    if (addr == CB_CHAN_KHONG_SP_ADDR) { 
      Write_SingleCoil_PLC(CB_CHAN_KHONG_SP_ADDR, value); Write_SingleCoil_PLC(CB_PHAT_HIEN_SP_ADDR, !value); Write_SingleCoil_PLC(CB_KEP_SP_ADDR, !value);}
    if (addr == CB_KEP_SP_ADDR) { 
      Write_SingleCoil_PLC(CB_KEP_SP_ADDR, value); Write_SingleCoil_PLC(CB_PHAT_HIEN_SP_ADDR, !value); Write_SingleCoil_PLC(CB_CHAN_KHONG_SP_ADDR, !value);}
}
//============================================================================================
// Đọc giá trị số nguyên trong thanh ghi D của PLC
uint16_t *Read_Resgister_PLC(uint16_t addr, uint8_t resQty) {
    uint8_t result = 1;
    static uint16_t value[64];
    result = NODE.readHoldingRegisters(addr, resQty); 
    if (result == 0) { 
      for (uint8_t i = 0; i < resQty; i++){
        value[i] = NODE.getResponseBuffer(i);
      }
    }
    return value;
}
//============================================================================================
// Ghi giá trị số nguyên vào thanh ghi D của PLC
void Write_delayValue_to_PLC(uint16_t addr, uint16_t value){
  if (value > 0 && value <= 90) {
  NODE.writeSingleRegister(addr, value);
  }
}
//============================================================================================
// Xóa ký tự trong mảng char
void EaseString(char *Str, int len){
  for (int i = 0; i < len; i++) {Str[i] = ' ';} 
  Str[len-1] = '\0';
}
//============================================================================================
void Send_manual_command_to_PLC(int8_t command){
  if (command == banXoayLen) { Write_SingleCoil_PLC(XOAY_XUONG_ADDR, OFF); Write_SingleCoil_PLC(XOAY_LEN_ADDR, ON);}
  else if (command == banXoayXuong) {Write_SingleCoil_PLC(XOAY_LEN_ADDR, OFF); Write_SingleCoil_PLC(XOAY_XUONG_ADDR, ON); }
  else if (command == penToi) {Write_SingleCoil_PLC(PEN_TOI_ADDR, ON); }
  else if (command == penLui) {Write_SingleCoil_PLC(PEN_TOI_ADDR, OFF); }
  else if (command == penLen) {Write_SingleCoil_PLC(PEN_XUONG_ADDR, OFF);}
  else if (command == penXuong) {Write_SingleCoil_PLC(PEN_XUONG_ADDR, ON);} 
  else if (command == toiVtTha) {Write_SingleCoil_PLC(TOI_VT_THA_ADDR, ON);}
  else if (command == toiVtGap) {Write_SingleCoil_PLC(TOI_VT_GAP_ADDR, ON);}
  else if (command == kepSanPham) {Write_SingleCoil_PLC(KEP_SP_ADDR, ON);}
  else if (command == hutSanPham) {Write_SingleCoil_PLC(HUT_SP_ADDR, ON);}
  else if (command == thaSanPham) {Write_SingleCoil_PLC(HUT_SP_ADDR, OFF);  Write_SingleCoil_PLC(KEP_SP_ADDR, OFF);}
  else { delay(1); }
}
//================================================================
// Configure thanh ghi timer/counter 
// http://arduino.vn/bai-viet/411-timercounter-tren-avrarduino
//================================================================
/* 
TCNTn: Thanh ghi thực hiện đếm giá trị 
TCCRnA/B: Thanh ghi điều khiển:
+ CSn0,CSn1,CSn2: Cài đặt prescale
TIMSKn: Thanh ghi điều khiển ngắt. 
+ bit 5 - ICIEn: Input Capture Interrupt Enable - Cho phép ngắt khi dùng Input Capture.
+ bit 2 - OCIEnB: Output Compare Interrupt Enable 1 channel B -  Cho phép ngắt khi dùng Output Compare ở channel B.
+ bit 1 - OCIEnA: Output Compare Interrupt Enable 1 channel A -  Cho phép ngắt khi dùng Output Compare ở channel A.
+ bit 0 - TOIEn: Overflow Interrupt Enable 1 - Cho phép ngắt khi xảy ra tràn trên T/C.
*/
/////////////////////////////////////////////////////////////////////////////
// Tạo Hàm timer 4. Dùng đọc giá trị input, output sau 100 ms
// Dùng cho chế độ monitor
void timer4_setting(void) {
  // timer 4 setting prescale = 64 
    cli();
    TCCR4A = 0;
    TCCR4B = 0;
    TIMSK4 = 0;
    TCCR4B |= ((0 << CS42) | (1 << CS41) | (1 << CS40));    // clk/64 prescaler (1 xung = 4 us)
    TCNT4 = 40536; // 100 ms tạo ngắt 1 lần để thực hiện đọc giá trị input, output
    sei();
}
ISR (TIMER4_OVF_vect) {
  TCNT4 = 40536;
}
//============================================================================================
void setup() {  
  Serial.begin(9600);
  MODBUS_SERIAL.begin(MODBUS_BAUDRATE);
  NODE.begin(PLC_SLAVE_ADDR, MODBUS_SERIAL);
  GLCD.init_display();
  Show_Infor();
  timer4_setting(); // Hàm ngắt dùng để đọc giá trị input/output
}
//============================================================================================
//============================================================================================
void loop() {
//============================================================================================
// Chống dội phím
  COMMAND_HOLD = getSingleKeypad(HOLD);
  if (COMMAND_HOLD == MODIFY) { while(true) { COMMAND_RELEASE = getSingleKeypad(RELEASED); if (COMMAND_RELEASE == MODIFY) { commandKeyPad = MODIFY; break; }}}
  else if (COMMAND_HOLD == MANUAL) {while(true) { COMMAND_RELEASE = getSingleKeypad(RELEASED); if (COMMAND_RELEASE == MANUAL) { commandKeyPad = MANUAL; break; }}}
  else if (COMMAND_HOLD == REMOTR) {while(true) { COMMAND_RELEASE = getSingleKeypad(RELEASED); if (COMMAND_RELEASE == REMOTR) { commandKeyPad = REMOTR; break; }} }
  else if (COMMAND_HOLD == MONITOR) { while(true) { COMMAND_RELEASE = getSingleKeypad(RELEASED); if (COMMAND_RELEASE == MONITOR) { commandKeyPad = MONITOR; break; }}}
  else if (COMMAND_HOLD == HOME) { while(true) { COMMAND_RELEASE = getSingleKeypad(RELEASED); if (COMMAND_RELEASE == HOME) { commandKeyPad = HOME; break; }}}
  else if (COMMAND_HOLD == AUTO_1 || COMMAND_HOLD == AUTO_2) { while(true) { COMMAND_RELEASE = getSingleKeypad(RELEASED); if (COMMAND_RELEASE == AUTO_1 || COMMAND_RELEASE == AUTO_2) { commandKeyPad = AUTO_1; break; }} }
  else if (COMMAND_HOLD == STOP_1 || COMMAND_HOLD == STOP_2) { while(true) { COMMAND_RELEASE = getSingleKeypad(RELEASED); if (COMMAND_RELEASE == STOP_1 || COMMAND_RELEASE == STOP_2) { commandKeyPad = STOP_1; break; }} }
  else {COMMAND_HOLD = NO_KEY_ACTIVE; commandKeyPad = NO_KEY_ACTIVE; COMMAND_RELEASE = NO_KEY_ACTIVE; }
//============================================================================================
  if (commandKeyPad == MODIFY) {
    GLCD.text_display_clear(); 
    Show_list_modify_plc();
    int8_t state_mode = MODIFY;
    int8_t pos = 0;
    int8_t index = Show_Pointer_Function(0,pos);
    while(state_mode == MODIFY){
      COMMAND = getSingleKeypad(RELEASED);
      switch (COMMAND){
        case DOWN: pos ++; if (pos > 2) {pos = 2;} index = Show_Pointer_Function(0,pos); break;
        case UP:   pos --; if (pos < 0) {pos = 0;} index = Show_Pointer_Function(0,pos); break;
        case MODIFY:   
                      Show_Infor();
                      state_mode = -1; 
                      break;
        case PGDN:
          // cài đặt delay
          if (index == TIMER_PLC_MODIFY) {  
            GLCD.text_display_clear();
            char delayvalue[9]; // Hiện thị giá trị delay đọc từ PLC
            char *delayArray;
            char delayString[9];  int8_t count_value = 0; uint16_t delaySetting; // Hiện thị giá trị delay cài đặt từ bàn phím
            uint16_t *delayPacket;
            EaseString(delayString, 9);
            GLCD.printTxt(3,0,"Timer:"); GLCD.printTxt(3,3,"0s"); count_value = 0;

            delayPacket = Read_Resgister_PLC (DELAY_XUONG_ADDR, 7); // Đọc 7 thanh ghi giá trị D512 -> D518
            delayArray = convert_delay_number_to_str(delayPacket[DELAY_XUONG*2]);
            
            GLCD.printTxt(0,0,"Cai Timer");
            GLCD.printTxt(1,0,"1.Delay Xuong  "); // Vị trí delay xuống lấy sản phẩm
            //for (int i = 0; i < 9; i++) {delayvalue[i] = delayArray[i]; if (delayArray[i] == '\0') { break; } } 
                        GLCD.printTxt(2,3,delayArray);

            uint8_t vitri = 1; // vị trí cần delay
            
            bool timer_state = true;
            while(timer_state) {
              COMMAND = getSingleKeypad(RELEASED);
              switch (COMMAND) {
                case GOBACK: 
                  timer_state = false; 
                  Show_list_modify_plc(); 
                  index = Show_Pointer_Function(0,pos);
                  break;

                case ONE:     delayString[count_value++] = '1'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case TWO:     delayString[count_value++] = '2'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case THREE:   delayString[count_value++] = '3'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case FOUR:    delayString[count_value++] = '4'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case FIVE:    delayString[count_value++] = '5'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case SIX:     delayString[count_value++] = '6'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case SEVEN:   delayString[count_value++] = '7'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case EIGHT:   delayString[count_value++] = '8'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case NINE:    delayString[count_value++] = '9'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case ZERO:    delayString[count_value++] = '0'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                case POINT:   delayString[count_value++] = '.'; delayString[count_value] = 's'; delayString[count_value+1] = '\0';GLCD.printTxt(3,3,delayString); if (count_value == 7) {count_value = 6;} break;
                
                case DEL:  
                  EaseString(delayString, 9);
                  GLCD.printTxt(3,3,delayString);
                  GLCD.printTxt(3,3,"0s"); count_value = 0;
                  break;

                case PGDN: 
                  EaseString(delayvalue, 9); // Xóa giá trị delay value
                  delaySetting = convert_str_to_delay_number(delayString); // xử lý giá trị delaySetting
                  EaseString(delayString, 9);
                  GLCD.printTxt(3,3,delayString);
                  GLCD.printTxt(3,3,"0s"); count_value = 0;

                  if (vitri == DELAY_XUONG) {
                    // ghi giá trị delaySetting vào thanh ghi Delay lấy sản phẩm
                    Write_delayValue_to_PLC( DELAY_LAYSP_ADDR, delaySetting);
                    // Đọc lại 7 thanh ghi delay giá trị D512 -> D518;
                    delayPacket = Read_Resgister_PLC (DELAY_XUONG_ADDR, 7); 
                    // Vị trí delay xuống lấy sản phẩm
                    GLCD.printTxt(1,0,"1.Delay Xuong  "); 
                    delayArray = convert_delay_number_to_str(delayPacket[DELAY_XUONG*2]);
                    //for (int i = 0; i < 9; i++) {delayvalue[i] = delayArray[i]; if (delayArray[i] == '\0') { break;}} 
                    GLCD.printTxt(2,3,delayArray);
                  }
                  if (vitri == DELAY_TOILUI) {
                    // ghi giá trị delaySetting vào thanh ghi Delay Xuống
                    Write_delayValue_to_PLC( DELAY_XUONG_ADDR, delaySetting);
                    // Vị trí delay tới lui lấy sản phẩm 
                    GLCD.printTxt(1,0,"2.Delay Toi/Lui"); 
                    delayArray = convert_delay_number_to_str(delayPacket[DELAY_TOILUI*2]);
                    //for (int i = 0; i < 9; i++) {delayvalue[i] = delayArray[i]; if (delayArray[i] == '\0') { break;}} 
                    GLCD.printTxt(2,3,delayArray);
                  }
                  if (vitri == DELAY_CHOLAYSP) {
                    // ghi giá trị delaySetting vào thanh ghi Delay tới lui
                    Write_delayValue_to_PLC( DELAY_TOILUI_ADDR, delaySetting);
                    // Vị trí delay chờ lấy sản phẩm 
                    GLCD.printTxt(1,0,"3.Delay choSP  "); 
                    delayArray = convert_delay_number_to_str(delayPacket[DELAY_CHOLAYSP*2]);
                    //for (int i = 0; i < 9; i++) {delayvalue[i] = delayArray[i]; if (delayArray[i] == '\0') { break;}} 
                    GLCD.printTxt(2,3,delayArray);
                  }
                  if (vitri == DELAY_LAYSP) {
                    // ghi giá trị delaySetting vào thanh ghi Delay chờ lấy sản phẩm
                    Write_delayValue_to_PLC( DELAY_CHOLAYSP_ADDR, delaySetting);
                    // Vị trí delay sau khi lấy sản phẩm 
                    GLCD.printTxt(1,0,"4.Delay laySP  "); 
                    delayArray = convert_delay_number_to_str(delayPacket[DELAY_LAYSP*2]);
                    //for (int i = 0; i < 9; i++) {delayvalue[i] = delayArray[i]; if (delayArray[i] == '\0') { break;}} 
                    GLCD.printTxt(2,3,delayArray);
                  }
                    vitri++; if (vitri > 3) { vitri = 0; } 
                    break;       
                }
              }
          }
          // Cài đặt tay xoay
          if (index == TAY_XOAY_PLC_MODIFY) {
            // đọc giá trị cài đặt tay xoay từ PLC
            uint8_t tayxoayValue = 0; tayxoayValue = Read_Coil_Modify_TayXoay();
            GLCD.text_display_clear();
            GLCD.printTxt(0,0,"Cai tay xoay");
            GLCD.printTxt(1,0,"1.ViTri Cho SP ");
            GLCD.printTxt(2,0,"   XOAY LEN   ");  // doc gia tri tay xoay tu PLC
            GLCD.printTxt(3,0,"   XOAY XUONG ");  // doc gia tri tay xoay tu PLC
            if (bitRead(tayxoayValue, TAYXOAYLEN_vtCHO) == ON && bitRead(tayxoayValue, TAYXOAYXUONG_vtCHO) == OFF){
                GLCD.printTxt(2,0,">>"); GLCD.printTxt(3,0,"  ");
            }
            else if (bitRead(tayxoayValue, TAYXOAYXUONG_vtCHO) == ON && bitRead(tayxoayValue, TAYXOAYLEN_vtCHO) == OFF){
                GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,">>");
            }
            else { GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,"  "); }

            uint8_t vitri = 1; // vị trí tay xoay
            bool tayxoay_state = true;
            bool tayxoayLen = true;
            uint8_t choose_tayxoay = 0;
            while(tayxoay_state){
              COMMAND = getSingleKeypad(RELEASED);
              switch (COMMAND) {
                case XOAY_XUONG:  GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,">>"); choose_tayxoay = XOAY_XUONG; break;
                case XOAY_LEN:    GLCD.printTxt(2,0,">>"); GLCD.printTxt(3,0,"  "); choose_tayxoay = XOAY_LEN; break;
                case PGDN:   
                      
                  if (vitri == VT_CHO_SP){ 
                    // ghi giá trị tay xoay vị trí thả sản phẩm
                    if (choose_tayxoay != 0) { Write_Coil_Modify_TayXoay(VT_THA_SP,choose_tayxoay); }
                    // đọc lại giá tri bit từ M523 - M530
                    tayxoayValue = Read_Coil_Modify_TayXoay(); 
                    // chuyển sang cài đặt tay xoay vị trí chờ lấy sản phẩm
                    GLCD.printTxt(1,0,"1.ViTri Cho SP ");
                    if (bitRead(tayxoayValue, TAYXOAYLEN_vtCHO) == ON && bitRead(tayxoayValue, TAYXOAYXUONG_vtCHO) == OFF){
                      GLCD.printTxt(2,0,">>"); GLCD.printTxt(3,0,"  ");
                    }
                    else if (bitRead(tayxoayValue, TAYXOAYXUONG_vtCHO) == ON && bitRead(tayxoayValue, TAYXOAYLEN_vtCHO) == OFF){
                      GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,">>");
                    }
                    else { GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,"  "); }
                  } 

                  if (vitri == VT_RUT_SP){ 
                    // ghi gia trị tay xoay vị trí chờ sản phẩm
                    if (choose_tayxoay != 0) { Write_Coil_Modify_TayXoay(VT_CHO_SP,choose_tayxoay); }
                    // chuyển sang cài đặt tay xoay vị trí rút sản phẩm trong khuôn
                    GLCD.printTxt(1,0,"2.ViTri Rut SP ");
                    if (bitRead(tayxoayValue, TAYXOAYLEN_vtRUT) == ON && bitRead(tayxoayValue, TAYXOAYXUONG_vtRUT) == OFF){
                      GLCD.printTxt(2,0,">>"); GLCD.printTxt(3,0,"  ");
                    }
                    else if (bitRead(tayxoayValue, TAYXOAYXUONG_vtRUT) == ON && bitRead(tayxoayValue, TAYXOAYLEN_vtRUT) == OFF){
                      GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,">>");
                    }
                    else { GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,"  "); }
                  }

                  if (vitri == VT_RUT_LEN){ 
                    // ghi giá trị tay xoay vị trí rút sản phẩm trong khuôn
                    if (choose_tayxoay != 0) { Write_Coil_Modify_TayXoay(VT_RUT_SP,choose_tayxoay); }
                    // chuyển sang cài đặt tay xoay vị trí rút lên hết
                    GLCD.printTxt(1,0,"3.ViTri Rut Len");
                    if (bitRead(tayxoayValue, TAYXOAYLEN_vtLEN) == ON && bitRead(tayxoayValue, TAYXOAYXUONG_vtLEN) == OFF){
                      GLCD.printTxt(2,0,">>"); GLCD.printTxt(3,0,"  ");
                    }
                    else if (bitRead(tayxoayValue, TAYXOAYXUONG_vtLEN) == ON && bitRead(tayxoayValue, TAYXOAYLEN_vtLEN) == OFF){
                      GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,">>");
                    }
                    else { GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,"  "); }
                  }

                  if (vitri == VT_THA_SP){
                    // ghi giá trị tay xoay vị trí rút sản phẩm lên hết
                    if (choose_tayxoay != 0) { Write_Coil_Modify_TayXoay(VT_RUT_LEN,choose_tayxoay); }
                    // chuyển sang cài đặt tay xoay vị trí thả sản phẩm
                    GLCD.printTxt(1,0,"4.ViTri Tha SP ");
                    if (bitRead(tayxoayValue, TAYXOAYLEN_vtTHA) == ON && bitRead(tayxoayValue, TAYXOAYXUONG_vtTHA) == OFF){
                      GLCD.printTxt(2,0,">>"); GLCD.printTxt(3,0,"  ");
                    }
                    else if (bitRead(tayxoayValue, TAYXOAYXUONG_vtTHA) == ON && bitRead(tayxoayValue, TAYXOAYLEN_vtTHA) == OFF){
                      GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,">>");
                    }
                    else { GLCD.printTxt(2,0,"  "); GLCD.printTxt(3,0,"  "); }
                  }

                  choose_tayxoay = 0;
                  vitri++; if (vitri > 3) { vitri = 0;}  
                  break;

                case GOBACK: 
                  tayxoay_state = false;
                  Show_list_modify_plc(); 
                  index = Show_Pointer_Function(0,pos);
                  break;
              }
            }
          }
          if (index == PHAT_HIEN_SP_PLC_MODIFY) {
            GLCD.text_display_clear();
            GLCD.printTxt(0,0,"Cai Phat Hien SP");
            GLCD.printTxt(1,0,"  CB Chan Khong");
            GLCD.printTxt(2,0,"  CB Tiem Can  ");
            GLCD.printTxt(3,0,"  CB Kep       ");
            int8_t posIndex = 0; int8_t posMode = 0;
            // Đọc cờ M từ PLC xem trạng thái chọn sử dụng cảm biến phát hiện vật M533 -> M535
            uint8_t coilPhatHienSP = 0 ; coilPhatHienSP = Read_Coil_Phat_Hien_SP();
            if (bitRead(coilPhatHienSP, CB_PHAT_HIEN_SP) == ON) { posMode = Show_Pointer_Function(0,CB_PHAT_HIEN_SP); posIndex = CB_PHAT_HIEN_SP;}
            else if (bitRead(coilPhatHienSP, CB_CHAN_KHONG) == ON) { posMode =  Show_Pointer_Function(0,CB_CHAN_KHONG); posIndex = CB_CHAN_KHONG;}
            else if (bitRead(coilPhatHienSP, CB_KEP) == ON) { posMode =  Show_Pointer_Function(0,CB_KEP); posIndex = CB_KEP;}
            else { GLCD.printTxt(1,0,"  ");  GLCD.printTxt(2,0,"  ");  GLCD.printTxt(3,0,"  ");}
            bool chedo_state = true;
            while(chedo_state) {
              COMMAND = getSingleKeypad(RELEASED);
                switch (COMMAND){
            
                  case GOBACK:
                    chedo_state = false;
                    Show_list_modify_plc(); 
                    index = Show_Pointer_Function(0,pos);
                    break;

                  case DOWN: posIndex ++; if (posIndex > 2) {posIndex = 2;} posMode = Show_Pointer_Function(0,posIndex); break;
                  case UP:   posIndex --; if (posIndex < 0) {posIndex = 0;} posMode = Show_Pointer_Function(0,posIndex); break;

                  case PGDN: 
                    if (posIndex == CB_PHAT_HIEN_SP)  { Write_Coil_Phat_Hien_SP(CB_PHAT_HIEN_SP_ADDR, ON);}
                    if (posIndex == CB_CHAN_KHONG)    { Write_Coil_Phat_Hien_SP(CB_CHAN_KHONG_SP_ADDR, ON); }
                    if (posIndex == CB_KEP)           { Write_Coil_Phat_Hien_SP(CB_KEP_SP_ADDR, ON); }
                    break;
                }
            }
          } 
        //Serial.println(index); 
        break;
      }
    }
  }
//============================================================================================
/*
//============================================================================================
  if (COMMAND == OPERATION_01 || COMMAND == OPERATION_02) {
    
    int8_t x = 0; int8_t y = 0; uint8_t program; uint8_t row,col; 
    uint16_t program_name[Program_number_max]; uint16_t program_step[Program_number_max];
    bool OPERATION_01_MODE = false; bool OPERATION_02_MODE = false; 
    choose_program = false; choose_teach_mode = false;

    GLCD.text_display_clear(); 

    switch (COMMAND) {
      case OPERATION_01: 
          OPERATION_01_MODE = true; 
          GLCD.printTxt(0,0,"OP.0");  Set_eeprom_type_to_slave_board(CHOOSE_EEPROM_01);
          break;

      case OPERATION_02: 
          OPERATION_02_MODE = true;  
          GLCD.printTxt(0,0,"OP.1");  Set_eeprom_type_to_slave_board(CHOOSE_EEPROM_02);
          break;
    }
    COMMAND = 0xFFFF;
    // Hiện thị tên các chương trình trong bộ nhớ EEPROM256
    // Nếu chương trình nào đã ready to autorun: stick dấu * trên tên chương trình
    row = 1; col = 0;
    for (int i = 0; i < Program_number_max; i++) {
        program_name[i] = Read_single_data_from_Eeprom_slave_board(Program_Addr[i]);     // Đọc số thứ tự chương trình
        program_step[i] = Read_single_data_from_Eeprom_slave_board(Program_Addr[i] + 2); // Đọc kiểm tra PROGRAM_NOT_READY/PROGRAM_READY
        GLCD.print_program_name(row,col,program_name[i],program_step[i]);
        col += 3;
        if (col == 9) { row ++; col = 0;}
    }
    while(OPERATION_02_MODE || OPERATION_01_MODE){
      for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++) { if (x == i && y == j) { program = Show_Pointer_Program(x,y); }}
      }
      COMMAND = KEYPAD.Switch_press_scan(); 
      
       switch (COMMAND) {
        case BUTTON_RIGHT:
          x++; if (x == 3)  {x = 2;} break;
        case BUTTON_LEFT:        
          x--; if (x == -1) {x = 0;} break;
        case BUTTON_UP:
          y--; if (y == -1) {y = 0;} break;
        case BUTTON_DOWN:
          y++; if (y == 3)  {y = 2;} break;
        case BUTTON_ENTER:{
            
           // Chọn chương trình để chạy
           // Lưu chương trình, truyền chương trình về board I/O qua bus RS485
          for (int i = 1; i < 10; i++) {
            if (program == i) { 
              Program_start_addr = Program_Addr[i - 1];
              Set_program_start_addr_to_slave_board(Program_start_addr);
              break;
            }
          }
            GLCD.text_display_clear(); 
            if (OPERATION_01_MODE == true) {GLCD.printTxt(0,0,"OP_0"); }
            if (OPERATION_02_MODE == true) {GLCD.printTxt(0,0,"OP_1"); }
            prog_name = program_name[program - 1];
            GLCD.printTxt(1,0,"CHUONG TRINH"); GLCD.print_number(1,6,prog_name);

            if (program_step[program - 1] != PROGRAM_READY) { // Chưa có chương trình
                choose_teach_mode = true;           // cho phép được vào teach mode
                GLCD.printTxt(3,3,"TEACH"); 
            } else {
              choose_program = true;    // cho phép chạy auto/modify/
              GLCD.printTxt(3,0,"AUTO/REMOTR/DEL");
              }                         // Serial.println(program_step[program - 1],HEX);                         
            OPERATION_01_MODE = OPERATION_02_MODE = false;
            break;
         }
        case OPERATION_01:
        case OPERATION_02:
           
           // Nếu nhấn phím operation_01 lần 2 sẽ thoát khỏi chế độ operation_01
           GLCD.text_display_clear();
           Show_Infor();
           choose_program = false; choose_teach_mode = false;
           OPERATION_01_MODE = OPERATION_02_MODE = false;
           break;
       } // END switch
    
    }
  } // END OPERATION MODE
//============================================================================================ 
// Vào chế độ dạy, tạo chương trình mới
// Mỗi lệnh write giá trị vào epprom -> cần read lại giá trị đó để kiểm tra eeprom đã lưu đúng giá trị chưa.
//============================================================================================ 
if (COMMAND == TEACH_MODE) {
  
  while(choose_teach_mode == true) {            
    uint16_t index_address; uint8_t write_eeprom_status;
    bool state_mode = true; bool waiting_position = false;
    bool main_position = false; bool pick_position = false; 
    bool delay_step = false; bool state_vaccum = false; bool state_pick = false;

    GLCD.text_display_clear(); 
    GLCD.printTxt(0,0,"OP0 "); GLCD.printTxt(0,2,"PROG"); GLCD.print_number(0,4,prog_name);
    GLCD.printTxt(1,0, "PgDn:GotoNextPg"); 
    GLCD.printTxt(2,0, "Stop:ExitNoSave");
    GLCD.printTxt(3,0, "Teach:Exit&Save");

    while (COMMAND != BUTTON_PAGE_DOWN){COMMAND = KEYPAD.Switch_press_scan();}
   
    // Địa chỉ Program_start_addr lưu tên chương trình: 2 bytes 
    // Địa chỉ index_address = Program_start_addr + 2 lưu trạng thái PROGRAM_NOT_READY/PROGRAM_READY 
    index_address = Program_start_addr + 2;
    write_eeprom_status = Write_single_data_to_Eeprom_slave_board(PROGRAM_NOT_READY, index_address);

    if (write_eeprom_status != WRITE_DONE) {
      choose_teach_mode = false; 
      Display_eeprom_error(prog_name, index_address);
      break;
    }
// Chọn mode arm cho tay máy: tùy vào từng mode arm mà cho phép set bit của từng mode arm tương ứng
//ví dụ: + chọn mode main_arm là mode tay hút, lúc này chỉ có thế set các bit output coil thuộc mode tay hút.
//các output coil thuộc mode tay kẹp sẽ không thể set được và ngược lại.
//       + nếu chọn mode kết hợp, sẽ set được cả hai. ****** trường hợp này chưa lập trình ******
 
    index_address += 2;
    GLCD.text_display_clear(); 
    GLCD.printTxt(0,0,"N00:MODE"); GLCD.printTxt(0,4," PROG"); GLCD.print_number(0,6,prog_name);
    GLCD.printTxt(1,0,">Mode tay hut"); GLCD.printTxt(2,0," Mode tay gap"); GLCD.printTxt(3,0," Mode ket hop"); 
    uint16_t arm_mode[MODE_ARM_SIZE/2];
    arm_mode[FC_CODE] = MODE_ARM; arm_mode[FC_VALUE_W1] = MAIN_ARM_MODE; // Default Mode tay hut
    while (state_mode){ // Chọn mode
      COMMAND = KEYPAD.Switch_press_scan();
      
      switch (COMMAND) {

        case MAIN_ARM_MODE: arm_mode[FC_VALUE_W1] = MAIN_ARM_MODE; 
                            GLCD.printTxt(2,0," "); GLCD.printTxt(3,0," "); GLCD.printTxt(1,0,">Mode tay hut"); break;
        case PICK_ARM_MODE: arm_mode[FC_VALUE_W1] = PICK_ARM_MODE; 
                            GLCD.printTxt(1,0," "); GLCD.printTxt(3,0," "); GLCD.printTxt(2,0,">Mode tay gap"); break;
        case TWO_ARM_MODE:  arm_mode[FC_VALUE_W1] = TWO_ARM_MODE;  
                            GLCD.printTxt(1,0," "); GLCD.printTxt(2,0," "); GLCD.printTxt(3,0,">Mode ket hop"); break;
        case BUTTON_PAGE_DOWN: write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(arm_mode, MODE_ARM_SIZE/2, index_address);
                                if (write_eeprom_status != WRITE_DONE) {
                                    choose_teach_mode = false; 
                                    Display_eeprom_error(prog_name, index_address);
                                    break;
                                }
                               index_address += MODE_ARM_SIZE;
                               state_mode = false; state_vaccum = true;
                               GLCD.text_display_clear();  break;
        case STOP_01:
        case STOP_02:          state_mode = false; state_vaccum = false; waiting_position = false; main_position = false; 
                               state_pick = false; choose_teach_mode = false; 
                               GLCD.text_display_clear();  Show_Infor(); break;
                               
        
      }
    }
    if (choose_teach_mode == false) { break; } // Thoát khỏi vòng lặp while choose_teach_mode

// Chọn cảm biến chân không cho tay máy
    GLCD.printTxt(0,0,"N01:USE VACCUM");
    GLCD.printTxt(1,0,"Vacuum sensor"); GLCD.printTxt(2,0,">Use sensor"); GLCD.printTxt(3,0," No sensor"); 
    uint16_t choose_vaccum_sensor[VACCUM_SENSOR_SIZE/2];
    choose_vaccum_sensor[FC_CODE] = VACCUM_SENSOR; choose_vaccum_sensor[FC_VALUE_W1] = 0;
    while (state_vaccum) { // Chọn cảm biến chân không
      COMMAND = KEYPAD.Switch_press_scan();
      
      switch (COMMAND) {
        case ARM_VACUUM:   
                       
                      if (!choose_vaccum_sensor[FC_VALUE_W1]){GLCD.printTxt(2,0," "); GLCD.printTxt(3,0,">No sensor"); 
                          choose_vaccum_sensor[FC_VALUE_W1] = 1;}
                      else {GLCD.printTxt(3,0," "); GLCD.printTxt(2,0,">Use sensor"); 
                          choose_vaccum_sensor[FC_VALUE_W1] = 0;}

                      break;
        case BUTTON_PAGE_DOWN: 
                              
                              write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(choose_vaccum_sensor, VACCUM_SENSOR_SIZE/2, index_address);
                               if (write_eeprom_status != WRITE_DONE) {
                                choose_teach_mode = false; 
                                Display_eeprom_error(prog_name, index_address);
                                break;
                               }
                               index_address += VACCUM_SENSOR_SIZE;
                               state_vaccum = false; state_pick = true; 
                               GLCD.text_display_clear();  break;
        case STOP_01:
        case STOP_02:          state_vaccum = false; waiting_position = false; main_position = false; 
                               state_pick = false; choose_teach_mode = false; 
                               GLCD.text_display_clear(); Show_Infor(); 
                               
                               break;
      }
    }
    if (choose_teach_mode == false) { break; } // Thoát khỏi vòng lặp while choose_teach_mode

// Chọn cảm biến tay kẹp cho tay máy
    GLCD.printTxt(0,0,"N02:USE GRIP");
    GLCD.printTxt(1,0,"Grip sensor"); GLCD.printTxt(2,0,">Use sensor"); GLCD.printTxt(3,0," No sensor"); 
    uint16_t choose_pick_sensor[VACCUM_SENSOR_SIZE/2];
    choose_pick_sensor[FC_CODE] = PICK_SENSOR; choose_pick_sensor[FC_VALUE_W1] = 0;
    while (state_pick) { // Chọn cảm biến tay kẹp
      COMMAND = KEYPAD.Switch_press_scan();
      
      switch (COMMAND) {
        case ARM_VACUUM:    
                      if (!choose_pick_sensor[FC_VALUE_W1]){GLCD.printTxt(2,0," "); GLCD.printTxt(3,0,">No sensor"); 
                          choose_pick_sensor[FC_VALUE_W1] = 1;}
                      else {GLCD.printTxt(3,0," "); GLCD.printTxt(2,0,">Use sensor"); 
                          choose_pick_sensor[FC_VALUE_W1] = 0;}
                      break;
        case BUTTON_PAGE_DOWN: 
                                write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(choose_pick_sensor, VACCUM_SENSOR_SIZE/2, index_address);
                               if (write_eeprom_status != WRITE_DONE) {
                                choose_teach_mode = false; 
                                Display_eeprom_error(prog_name, index_address);
                                break;
                               }
                               index_address += VACCUM_SENSOR_SIZE;
                               state_pick = false; waiting_position = true;
                               GLCD.text_display_clear();  break;
        case STOP_01:
        case STOP_02:          state_pick = false; waiting_position = false; main_position = false; 
                               choose_teach_mode = false; 
                               GLCD.text_display_clear();  Show_Infor(); break;
                               
      }
    }

    if (choose_teach_mode == false) { break; } // Thoát khỏi vòng lặp while choose_teach_mode

// chọn vị trí chờ lấy sản phẩm 
    GLCD.printTxt(0,0,"N03:WAITING POS");  
    GLCD.printTxt(1,0," Tay xoay: "); GLCD.printTxt(2,0," Tay toi/lui");
    uint16_t output_coil[INPUT_OUTPUT_SIZE/2]; 
    output_coil[FC_CODE] = WAITING_POSSITON; output_coil[FC_VALUE_W1] = 0x0000; output_coil[FC_VALUE_W2] = 0x0000;
    // Set chế độ robot
    output_coil[FC_VALUE_W1] = bitSet(output_coil[FC_VALUE_W1], ROBOT_MODE);
    while (waiting_position) { // Khai báo vị trí chờ lấy sản phẩm

      COMMAND = KEYPAD.Switch_press_scan();
     
      switch (COMMAND) {

        case ARM_ROTATE_UP:   output_coil[FC_VALUE_W2] = bitSet(output_coil[FC_VALUE_W2],ARM_SET_ROTATE_UP);  
                              output_coil[FC_VALUE_W2] = bitClear(output_coil[FC_VALUE_W2],ARM_SET_ROTATE_DOWN);
                              Write_packet_data_to_output_slave_board(output_coil, INPUT_OUTPUT_SIZE/2);
                              GLCD.printTxt(1,0,">Tay xoay len  "); break;

        case ARM_ROTATE_DOWN: output_coil[FC_VALUE_W2] = bitSet(output_coil[FC_VALUE_W2],ARM_SET_ROTATE_DOWN);
                              output_coil[FC_VALUE_W2] = bitClear(output_coil[FC_VALUE_W2],ARM_SET_ROTATE_UP); 
                              Write_packet_data_to_output_slave_board(output_coil, INPUT_OUTPUT_SIZE/2);
                              GLCD.printTxt(1,0,">Tay xoay xuong");  break;

        case ARM_GO_LEFT:     output_coil[FC_VALUE_W2] = bitSet(output_coil[FC_VALUE_W2],ARM_SUB_LEFT_RIGHT);   
                              Write_packet_data_to_output_slave_board(output_coil, INPUT_OUTPUT_SIZE/2);
                              GLCD.printTxt(2,0,">Tay di toi ");  break;

        case ARM_GO_RIGHT:    output_coil[FC_VALUE_W2] = bitClear(output_coil[FC_VALUE_W2],ARM_SUB_LEFT_RIGHT);
                              Write_packet_data_to_output_slave_board(output_coil, INPUT_OUTPUT_SIZE/2);  
                              GLCD.printTxt(2,0,">Tay di lui "); break;
                                
        case BUTTON_PAGE_DOWN: 
                                  if (output_coil[FC_VALUE_W2] != 0){ // Nếu chưa chọn thì tiếp tục vòng lăp chọn waiting_position
                                  write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(output_coil, INPUT_OUTPUT_SIZE/2, index_address);
                                      if (write_eeprom_status != WRITE_DONE) {
                                        choose_teach_mode = false; 
                                        Display_eeprom_error(prog_name, index_address);
                                        break;
                                      }
                                  index_address += INPUT_OUTPUT_SIZE;
                                  main_position = true; delay_step = true; waiting_position = false; 
                                  GLCD.text_display_clear(); 
                                } break;           
        case STOP_01:
        case STOP_02:           waiting_position = false; main_position = false; choose_teach_mode = false; 
                                GLCD.text_display_clear();  Show_Infor(); break; 
      }
    } // end waiting position

    if (choose_teach_mode == false) { break; } // Thoát khỏi vòng lặp while choose_teach_mode

    uint16_t delay_value[STEP_REPEAT_DELAY_SIZE/2]; char delayArray[9]; int8_t count_value;
    delay_value[FC_CODE] = DELAY_STEP; 

    while (main_position){ // Các thao tác trong chương trình chính

      delay_value[FC_VALUE_W1] = 0; count_value = 0;
      GLCD.printTxt(0,0,"N04:DELAY POS");
      GLCD.printTxt(1,0,"Delay: "); GLCD.printTxt(1,3,"0s");
      GLCD.printTxt(3,0, "Press PgDn ...");
      for (int i = 0; i < 9; i++) {delayArray[i] = ' ';} delayArray[8] = '\0'; // Xóa giá trị delay value
    
      while (delay_step){
        COMMAND = KEYPAD.Switch_press_scan();
        
        switch (COMMAND) {
          case _1_:    delayArray[count_value++] = '1'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _2_:    delayArray[count_value++] = '2'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _3_:    delayArray[count_value++] = '3'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _4_:    delayArray[count_value++] = '4'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _5_:    delayArray[count_value++] = '5'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _6_:    delayArray[count_value++] = '6'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _7_:    delayArray[count_value++] = '7'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _8_:    delayArray[count_value++] = '8'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _9_:    delayArray[count_value++] = '9'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _0_:    delayArray[count_value++] = '0'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
          case _point_:delayArray[count_value++] = '.'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;

          case BUTTON_DEL:  for (int i = 0; i < 9; i++) {delayArray[i] = ' ';} 
                            delayArray[8] = '\0'; GLCD.printTxt(1,3,delayArray);
                            GLCD.printTxt(1,3,"0s"); count_value = 0;
                            break; 
          case BUTTON_PAGE_DOWN:  delay_value[FC_VALUE_W1] = convert_str_to_delay_number(delayArray);
                                  write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(delay_value, STEP_REPEAT_DELAY_SIZE/2, index_address);
                                  if (write_eeprom_status != WRITE_DONE) {
                                    choose_teach_mode = false; 
                                    Display_eeprom_error(prog_name, index_address);
                                    break;
                                  }
                                  index_address += STEP_REPEAT_DELAY_SIZE;
                                  pick_position = true; delay_step = false; 
                                  GLCD.text_display_clear();  break;

          case TEACH_MODE:        pick_position = false; delay_step = false; main_position = false; choose_teach_mode = false;
                                  // Lưu mã kết thúc chương trình
                                  write_eeprom_status = Write_single_data_to_Eeprom_slave_board(REPEAT_STEP, index_address);
                                  if (write_eeprom_status != WRITE_DONE) {
                                      Display_eeprom_error(prog_name,index_address);
                                      break;
                                  }
                                  // Lưu mã sẵn sàng chạy auto/modify
                                  write_eeprom_status = Write_single_data_to_Eeprom_slave_board(PROGRAM_READY,  Program_start_addr + 2);
                                  if (write_eeprom_status != WRITE_DONE) {

                                      Display_eeprom_error(prog_name, Program_start_addr + 2);
                                      break;
                                  }
                                  GLCD.text_display_clear(); 
                                  GLCD.printTxt(1,0,"TEACH MODE DONE"); 
                                  GLCD.printTxt(3,0, "Press OP.0/OP.1");
                                  break;

          case STOP_01:           
          case STOP_02:           pick_position = false; delay_step = false; main_position = false; choose_teach_mode = false;
                                  GLCD.text_display_clear();  Show_Infor(); break;

        }
      } //end delay_step

      if (choose_teach_mode == false) { break; } // Thoát khỏi vòng lặp while choose_teach_mode

      uint16_t output_coil_index[INPUT_OUTPUT_SIZE/2]; 
      uint8_t count_act = 1; // đếm số action 
      output_coil[FC_CODE] = OUTPUT_STEP;
      output_coil_index[FC_VALUE_W1] = output_coil[FC_VALUE_W1];
      output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2];

      while (pick_position) {
        GLCD.printTxt(0,0,"N05:DEFINE POS");
        GLCD.printTxt(1,0,"Action:"); GLCD.print_number(1,3,Number_STT[count_act]); 
        GLCD.printTxt(3,0, " PgDn/Enter/Ins");
        COMMAND = KEYPAD.Switch_press_scan();
        
        switch (COMMAND) {

          case ARM_ROTATE_UP:     output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2];
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_SET_ROTATE_UP);  
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_SET_ROTATE_DOWN);
                                  GLCD.printTxt(2,0,">Tay xoay len  "); break;

          case ARM_ROTATE_DOWN:   output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2];
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_SET_ROTATE_DOWN);
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_SET_ROTATE_UP);                                 
                                  GLCD.printTxt(2,0,">Tay xoay xuong"); break;

          case ARM_GO_LEFT:       output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2]; 
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_SUB_LEFT_RIGHT);                                  
                                  GLCD.printTxt(2,0,">Tay di toi    "); break;

          case ARM_GO_RIGHT:      output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2]; 
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_SUB_LEFT_RIGHT);                                 
                                  GLCD.printTxt(2,0,">Tay di lui    "); break;

          case ARM_UP:            output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2]; 
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_SUB_UP_DOWN);                                 
                                  GLCD.printTxt(2,0,">Tay di len    "); break;

          case ARM_DOWN:          output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2];
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_SUB_UP_DOWN);                                
                                  GLCD.printTxt(2,0,">Tay di xuong  "); break;

          case ARM_FORWARD:       output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2]; 
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_SET_FORWARD);
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_SET_REVERS);                                 
                                  GLCD.printTxt(2,0,">Toi vi tri tha"); break;

          case ARM_REVERS:        output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2];
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_SET_REVERS);
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_SET_FORWARD);                                 
                                  GLCD.printTxt(2,0,">Toi vi tri gap"); break;

          case ARM_PICK:          output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2]; 
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_PICK_UNPICK_01);
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_PICK_UNPICK_02);
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_SET_VACUUM_01);
                                  output_coil_index[FC_VALUE_W2] = bitSet(output_coil_index[FC_VALUE_W2],ARM_SET_VACUUM_02);                                 
                                  GLCD.printTxt(2,0,">Lay san pham  "); break;

          case ARM_UNPICK:        output_coil_index[FC_VALUE_W2] = output_coil[FC_VALUE_W2]; 
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_PICK_UNPICK_01);
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_PICK_UNPICK_02);
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_SET_VACUUM_01);
                                  output_coil_index[FC_VALUE_W2] = bitClear(output_coil_index[FC_VALUE_W2],ARM_SET_VACUUM_02);                                 
                                  GLCD.printTxt(2,0,">Tha san pham  "); break;

          case ARM_VACUUM:        // Phát lệnh ép sản phẩm :đóng khuôn -> ép sản phẩm -> mở khuôn
                                  output_coil_index[FC_VALUE_W1] = output_coil[FC_VALUE_W1];
                                  output_coil_index[FC_VALUE_W1] = bitSet(output_coil_index[FC_VALUE_W1], MOLD_CLOSE);
                                  GLCD.printTxt(2,0,">EP SAN PHAM   "); break;

          case BUTTON_THANG:      // Khai báo OUTPUT_STEP là bước lấy sản phẩm
                                  output_coil_index[FC_VALUE_W1] = output_coil[FC_VALUE_W1];
                                  output_coil_index[FC_VALUE_W1] = bitSet(output_coil_index[FC_VALUE_W1], GET_PRODUCT);
                                  GLCD.printTxt(2,0,">LAY SAN PHAM  ");
                                  break;

          case BUTTON_INS:        // Chèn thêm output coil để thực hiện nhiều thao tác
                                  if ((output_coil[FC_VALUE_W2] != output_coil_index[FC_VALUE_W2]) || (output_coil[FC_VALUE_W1] != output_coil_index[FC_VALUE_W1])){
                                      count_act++;
                                      output_coil[FC_VALUE_W1] = output_coil_index[FC_VALUE_W1]; 
                                      output_coil[FC_VALUE_W2] = output_coil_index[FC_VALUE_W2];
                                      GLCD.text_display_clear(); 
                                  }break;

          case BUTTON_ENTER:      // Xuất output coil để mô phỏng thực tế những thao tác đã chọn
                                  Write_packet_data_to_output_slave_board(output_coil_index, INPUT_OUTPUT_SIZE/2); break;

          case BUTTON_PAGE_DOWN:  // Lưu giá trị output coil vào eeprom board slave
                                  if ((output_coil[FC_VALUE_W2] != output_coil_index[FC_VALUE_W2]) || (output_coil[FC_VALUE_W1] != output_coil_index[FC_VALUE_W1])){
                                      output_coil[FC_VALUE_W1] = output_coil_index[FC_VALUE_W1]; 
                                      output_coil[FC_VALUE_W2] = output_coil_index[FC_VALUE_W2];  
                                      write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(output_coil, INPUT_OUTPUT_SIZE/2, index_address);
                                      if (write_eeprom_status != WRITE_DONE) {
                                        choose_teach_mode = false; 
                                        Display_eeprom_error(prog_name, index_address);
                                        break;
                                      }
                                      // Reset bit đóng khuôn sau khi đã lưu vào eeprom
                                      if (bitRead(output_coil[FC_VALUE_W1],MOLD_CLOSE)) {output_coil[FC_VALUE_W1] = bitClear(output_coil[FC_VALUE_W1],MOLD_CLOSE);}
                                      // Reset bit lấy sản phẩm sau khi đã lưu vào eeprom
                                      if (bitRead(output_coil[FC_VALUE_W1],GET_PRODUCT)) {output_coil[FC_VALUE_W1] = bitClear(output_coil[FC_VALUE_W1],GET_PRODUCT);}
                                      // Reset bit tới vị trí gắp sau khi đã lưu vào eeprom
                                      //if (bitRead(output_coil[FC_VALUE_W2],ARM_SET_REVERS)) {output_coil[FC_VALUE_W2] = bitClear(output_coil[FC_VALUE_W2],ARM_SET_REVERS);}
                                      // Reset bit tới vị trí thả sau khi đã lưu vào eeprom
                                      //if (bitRead(output_coil[FC_VALUE_W2],ARM_SET_FORWARD)) {output_coil[FC_VALUE_W2] = bitClear(output_coil[FC_VALUE_W2],ARM_SET_FORWARD);}
                                      
                                      index_address += INPUT_OUTPUT_SIZE;
                                      pick_position = false; delay_step = true;
                                      GLCD.text_display_clear(); 
                                  }break;

          case TEACH_MODE:        // Lưu mã hoàn thành chương trình TEACH_MODE
                                  pick_position = false; delay_step = false; main_position = false; choose_teach_mode = false;
                                  // Lưu mã kết thúc chương trình
                                  write_eeprom_status = Write_single_data_to_Eeprom_slave_board(REPEAT_STEP, index_address);
                                  if (write_eeprom_status != WRITE_DONE) {
                                      choose_teach_mode = false;
                                      Display_eeprom_error(prog_name, index_address);
                                      break;
                                  }
                                  // Lưu mã sẵn sàng chạy auto/modify
                                  write_eeprom_status = Write_single_data_to_Eeprom_slave_board(PROGRAM_READY, Program_start_addr + 2);
                                  if (write_eeprom_status != WRITE_DONE) {
                                      Display_eeprom_error(prog_name, Program_start_addr + 2);
                                      choose_teach_mode = false;
                                      break;
                                  }                          
                                  GLCD.text_display_clear(); 
                                  GLCD.printTxt(1,0,"TEACH MODE DONE"); 
                                  GLCD.printTxt(3,0, "Press OP.0/OP.1");
                                  break;                         

          case STOP_01:           // Thoát khỏi chương trình TEACH_MODE và không lưu data
          case STOP_02:           pick_position = false; delay_step = false; main_position = false; choose_teach_mode = false; 
                                  GLCD.text_display_clear();  Show_Infor(); break;
        }
        if (choose_teach_mode == false) { break; } // Thoát khỏi vòng lặp while choose_teach_mode
      } // end while pick_position
      if (choose_teach_mode == false) { break; } // Thoát khỏi vòng lặp while choose_teach_mode
    } // end while main_position
    if (choose_teach_mode == false) { break; } // Thoát khỏi vòng lặp while choose_teach_mode
  } // end while choose_teach_mode
} // end if teach_mode
//============================================================================================ 
// Chế độ mô phỏng tay máy chạy step by step chương trình đã teached 
// Modify chương trình: chế độ dùng cảm biến, thời gian delay
//============================================================================================ 
  if (COMMAND == REMOTR_MODE) {
    if (choose_program == true){

      uint16_t index_address;
      uint16_t get_function;
      uint8_t  packet_len;
      uint16_t output_coil[INPUT_OUTPUT_SIZE/2]; 
      
      bool _function = false;  bool modify_program = true;
      bool modify_delay = false; bool modify_vaccum_sensor = false; bool modify_pick_sensor = false;

      output_coil[FC_CODE] = OUTPUT_STEP; output_coil[FC_VALUE_W1] = 0; output_coil[FC_VALUE_W2] = 0;
      GLCD.text_display_clear(); 
      GLCD.printTxt(0,1,"OP0 "); GLCD.printTxt(0,3,"PROG"); GLCD.print_number(0,5,prog_name); 
      GLCD.printTxt(1,0, "PgDn:GotoNextPg");
      GLCD.printTxt(2,0, "Modify:EditDelay"); 
      GLCD.printTxt(3,0, "Stop:ExitRemotr");
    
      index_address = Program_start_addr + 4; //Nhảy 4 bytes tới mode_program
     
      while(modify_program){
          COMMAND = KEYPAD.Switch_press_scan();
          
          switch(COMMAND) {
            case BUTTON_PAGE_DOWN: 
                  modify_delay = false; modify_vaccum_sensor = false; modify_pick_sensor = false;
                  get_function = Read_single_data_from_Eeprom_slave_board(index_address); 
                  Serial.println(get_function,HEX);
                  for (int i = 0; i < Function_number_max; i++) {
                    if (get_function == u16Function[i]) {_function = true; break;}
                  }
                  if (_function == false) {GLCD.printTxt(3,2,"Error..."); return;}
                  break;
            case MODIFY_MODE:
//Modify delay value------------------------------------------------------------------------
                if (modify_delay) { // Nếu modify ở step delay
                    uint16_t delay_value[STEP_REPEAT_DELAY_SIZE/2]; char delayArray[9]; int8_t count_value;
                    delay_value[FC_CODE] = DELAY_STEP; 
                    index_address -= STEP_REPEAT_DELAY_SIZE; // Quay ngược lại địa chỉ bắt đầu của delay step
                    GLCD.printTxt(2,0,"Modify delay ...");
                    for (int i = 0; i < 9; i++) {delayArray[i] = ' ';} 
                    delayArray[8] = '\0'; GLCD.printTxt(1,3,delayArray); GLCD.printTxt(1,3,"0s"); count_value = 0;
                
                    while(modify_delay) {
                        COMMAND = KEYPAD.Switch_press_scan();
                        switch (COMMAND) {
                          case _1_:    delayArray[count_value++] = '1'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _2_:    delayArray[count_value++] = '2'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _3_:    delayArray[count_value++] = '3'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _4_:    delayArray[count_value++] = '4'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _5_:    delayArray[count_value++] = '5'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _6_:    delayArray[count_value++] = '6'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _7_:    delayArray[count_value++] = '7'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _8_:    delayArray[count_value++] = '8'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _9_:    delayArray[count_value++] = '9'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _0_:    delayArray[count_value++] = '0'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;
                          case _point_:delayArray[count_value++] = '.'; delayArray[count_value] = 's'; delayArray[count_value+1] = '\0';GLCD.printTxt(1,3,delayArray); if (count_value == 7) {count_value = 6;} break;

                          case BUTTON_DEL:  for (int i = 0; i < 9; i++) {delayArray[i] = ' ';} 
                                            delayArray[8] = '\0'; GLCD.printTxt(1,3,delayArray);
                                            GLCD.printTxt(1,3,"0s"); count_value = 0;
                                            break; 
                          case BUTTON_PAGE_DOWN:  delay_value[FC_VALUE_W1] = convert_str_to_delay_number(delayArray);
                                                  uint8_t write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(delay_value, STEP_REPEAT_DELAY_SIZE/2, index_address);
                                                  if (write_eeprom_status != WRITE_DONE) {
                                                    modify_program = false; modify_delay = false;
                                                    Display_eeprom_error(prog_name, index_address);
                                                    break; //Thoat khoi switch
                                                  } delay(100);
                                                  index_address += STEP_REPEAT_DELAY_SIZE;
                                                  get_function = Read_single_data_from_Eeprom_slave_board(index_address); 
                                                  Serial.println(get_function,HEX);
                                                  for (int i = 0; i < Function_number_max; i++) {
                                                    if (get_function == u16Function[i]) {_function = true; break;}
                                                  }
                                                  if (_function == false) {GLCD.printTxt(3,2,"Error.........."); return;}
                                                  modify_delay = false; break;                        
                        }
                    } // while modify_delay
                  } // if modify_delay
//Modify vaccum sensor------------------------------------------------------------------------
                 if (modify_vaccum_sensor){
                    GLCD.printTxt(1,0,"Modify vaccum..."); GLCD.printTxt(2,0,">Use sensor");
                    uint16_t choose_vaccum_sensor[VACCUM_SENSOR_SIZE/2];
                    choose_vaccum_sensor[FC_CODE] = VACCUM_SENSOR; choose_vaccum_sensor[FC_VALUE_W1] = 0;
                    index_address -= VACCUM_SENSOR_SIZE;
                    while (modify_vaccum_sensor){
                      COMMAND = KEYPAD.Switch_press_scan();
                      switch (COMMAND) {
                        case ARM_VACUUM:    
                                      if (!choose_vaccum_sensor[FC_VALUE_W1]){GLCD.printTxt(2,0,">No sensor "); choose_vaccum_sensor[FC_VALUE_W1] = 1;}
                                      else {GLCD.printTxt(2,0,">Use sensor"); choose_vaccum_sensor[FC_VALUE_W1] = 0;}
                                      break;
                        case BUTTON_PAGE_DOWN: 
                                              uint8_t write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(choose_vaccum_sensor, VACCUM_SENSOR_SIZE/2, index_address);
                                              if (write_eeprom_status != WRITE_DONE) {
                                                modify_program = false; modify_vaccum_sensor = false;
                                                Display_eeprom_error(prog_name, index_address);
                                                break;
                                              } delay(100);
                                              index_address += VACCUM_SENSOR_SIZE;
                                              get_function = Read_single_data_from_Eeprom_slave_board(index_address); 
                                              Serial.println(get_function,HEX);
                                              for (int i = 0; i < Function_number_max; i++) {
                                                if (get_function == u16Function[i]) {_function = true; break;}
                                              }
                                              if (_function == false) {GLCD.printTxt(3,2,"Error.........."); return;}
                                              modify_vaccum_sensor = false; break;
                                        }
                    } // while modify_vaccum_sensor
                 } // if modify_vaccum_sensor
//Modify pick sensor------------------------------------------------------------------------
                 if (modify_pick_sensor){
                   GLCD.printTxt(1,0,"Modify grip ..."); GLCD.printTxt(2,0,">Use sensor"); 
                   uint16_t choose_pick_sensor[VACCUM_SENSOR_SIZE/2];
                   choose_pick_sensor[FC_CODE] = PICK_SENSOR; choose_pick_sensor[FC_VALUE_W1] = 0;
                   index_address -= VACCUM_SENSOR_SIZE;
                   while (modify_pick_sensor){
                      COMMAND = KEYPAD.Switch_press_scan();
                      switch (COMMAND) {
                        case ARM_VACUUM:    
                                      if (!choose_pick_sensor[FC_VALUE_W1]){GLCD.printTxt(2,0,">No sensor "); choose_pick_sensor[FC_VALUE_W1] = 1;}
                                      else {GLCD.printTxt(2,0,">Use sensor"); choose_pick_sensor[FC_VALUE_W1] = 0;}
                                      break;
                        case BUTTON_PAGE_DOWN: 
                                              uint8_t write_eeprom_status = Write_packet_data_to_Eeprom_slave_board(choose_pick_sensor, VACCUM_SENSOR_SIZE/2, index_address);
                                              if (write_eeprom_status != WRITE_DONE) {
                                                modify_program = false; modify_pick_sensor = false;
                                                Display_eeprom_error(prog_name, index_address);
                                                break;
                                              } delay(100);
                                              index_address += VACCUM_SENSOR_SIZE;
                                              get_function = Read_single_data_from_Eeprom_slave_board(index_address); 
                                              Serial.println(get_function,HEX);
                                              for (int i = 0; i < Function_number_max; i++) {
                                                if (get_function == u16Function[i]) {_function = true; break;}
                                              }
                                              if (_function == false) {GLCD.printTxt(3,2,"Error.........."); return;}
                                              modify_pick_sensor = false; break;
                                        }
                   } // while modify_pick_sensor
                 } // if modify_pick_sensor
                 break;
            case STOP_01: 
            case STOP_02:
                  modify_program = false; 
                  GLCD.text_display_clear(); 
                  Show_Infor();
                  _function = false;
                  break;
          }
//Thực hiện monitor------------------------------------------------------------------------
          while (_function) {
          Serial.println(get_function, HEX);
// Hiện thị mã chọn mode tay máy
            if (get_function == MODE_ARM){

                GLCD.text_display_clear(); 
                GLCD.printTxt(0,0,"N00:MODE"); GLCD.printTxt(0,4," PROG"); GLCD.print_number(0,6,prog_name);
                packet_len = MODE_ARM_SIZE/2; // 2 words
                uint16_t *mode_function = Read_packet_data_from_Eeprom_slave_board(packet_len, index_address);
                switch (mode_function[FC_VALUE_W1]){
                    case MAIN_ARM_MODE: GLCD.printTxt(1,0,">Mode tay hut"); break;
                    case PICK_ARM_MODE: GLCD.printTxt(1,0,">Mode tay gap"); break;
                    case TWO_ARM_MODE:  GLCD.printTxt(1,0,">Mode ket hop"); break;
                }
                GLCD.printTxt(3,0," Press PgDn ...");
                index_address += MODE_ARM_SIZE; _function = false;
                break;}
// Hiện thị mã chọn cảm biến chân không              
            if (get_function == VACCUM_SENSOR) {

                GLCD.text_display_clear(); 
                GLCD.printTxt(0,0,"N01:USE VACCUM"); GLCD.printTxt(1,0,"Vaccum sensor: ");
                packet_len = VACCUM_SENSOR_SIZE/2; // 2 words
                uint16_t *vaccum_function = Read_packet_data_from_Eeprom_slave_board(packet_len, index_address);
                switch (vaccum_function[FC_VALUE_W1]){
                    case 0:  GLCD.printTxt(2,0,">Use sensor"); break;
                    case 1:  GLCD.printTxt(2,0,">No sensor"); break;
                }
                GLCD.printTxt(3,0," Pr.PgDn/Modify");
                index_address += VACCUM_SENSOR_SIZE; _function = false; modify_vaccum_sensor = true;
                break;}
// Hiện thị mã chọn cảm biến tay kẹp 
            if (get_function == PICK_SENSOR) {

                GLCD.text_display_clear(); 
                GLCD.printTxt(0,0,"N02:USE GRIP"); GLCD.printTxt(1,0,"Grip sensor: ");
                packet_len = VACCUM_SENSOR_SIZE/2; // 2 words
                uint16_t *pick_function = Read_packet_data_from_Eeprom_slave_board(packet_len, index_address);
                switch (pick_function[FC_VALUE_W1]){
                    case 0:  GLCD.printTxt(2,0,">Use sensor"); break;
                    case 1:  GLCD.printTxt(2,0,">No sensor"); break;
                }

                GLCD.printTxt(3,0," Pr.PgDn/Modify");
                index_address += VACCUM_SENSOR_SIZE; _function = false; modify_pick_sensor = true;
                break;}
// Vị trí chờ lấy sản phẩm 
            if (get_function == WAITING_POSSITON){

                GLCD.text_display_clear(); 
                GLCD.printTxt(0,0,"N03:WAITING POS");
                packet_len = INPUT_OUTPUT_SIZE/2; // 3 words
                uint16_t *waiting_function = Read_packet_data_from_Eeprom_slave_board(packet_len, index_address);
                if(bitRead(waiting_function[FC_VALUE_W2],ARM_SET_ROTATE_UP)){GLCD.printTxt(1,0,">Tay xoay len  ");}
                if(bitRead(waiting_function[FC_VALUE_W2],ARM_SET_ROTATE_DOWN)){GLCD.printTxt(1,0,">Tay xoay xuong");} 
                if(bitRead(waiting_function[FC_VALUE_W2],ARM_SUB_LEFT_RIGHT)){GLCD.printTxt(2,0,">Tay di toi ");} 
                else GLCD.printTxt(2,0,">Tay di lui ");
                //Xuất giá trị output
                output_coil[FC_VALUE_W1] = waiting_function[FC_VALUE_W1]; 
                output_coil[FC_VALUE_W2] = waiting_function[FC_VALUE_W2];
                Write_packet_data_to_output_slave_board(output_coil, INPUT_OUTPUT_SIZE/2); // xuất output_coil cho tay máy chạy
                GLCD.printTxt(3,0," Press PgDn ...");
                index_address += INPUT_OUTPUT_SIZE; _function = false;
                break;}
// Thời gian delay 
            if (get_function == DELAY_STEP){

                GLCD.text_display_clear(); 
                GLCD.printTxt(0,0,"N04:DELAY POS"); GLCD.printTxt(1,0,"Delay: "); 
                packet_len = STEP_REPEAT_DELAY_SIZE/2; //2 words
                uint16_t *delay_function = Read_packet_data_from_Eeprom_slave_board(packet_len, index_address);
                char delayvalue[9];
                char *delayArray = convert_delay_number_to_str(delay_function[FC_VALUE_W1]);
                // hiển thị giá trị delayvalue lên GLCD
                for (int i = 0; i < 9; i++) {delayvalue[i] = delayArray[i]; if (delayArray[i] == '\0') { break;}} 
                                            GLCD.printTxt(1,3,delayvalue);
                GLCD.printTxt(3,0," Pr.PgDn/Modify");
                index_address += STEP_REPEAT_DELAY_SIZE; _function = false; modify_delay = true; // Đang ở function delay step
                break;}
// Output coil 
            if (get_function == OUTPUT_STEP){

                GLCD.text_display_clear(); 
                GLCD.printTxt(0,0,"N05:DEFINE POS"); GLCD.printTxt(1,0,"Action: ...");
                packet_len = INPUT_OUTPUT_SIZE/2; // 3 words
                uint16_t *output_function = Read_packet_data_from_Eeprom_slave_board(packet_len, index_address);
                // Kiểm tra giá trị output_function có bị lỗi không
                if ((output_function[FC_VALUE_W1] == 0xFFFF) || (output_function[FC_VALUE_W2] == 0xFFFF)) 
                { GLCD.printTxt(3,0,"Eeprom error..."); break; }
                //Giám sát giá trị đọc được từ eeprom 
                Serial.println(output_function[FC_VALUE_W1],HEX);
                Serial.println(output_function[FC_VALUE_W2],HEX);
                Check_status_of_output_coil_word(output_function[FC_VALUE_W2]);
                GLCD.printTxt(3,0," Press PgDn ...");
                //Xuất giá trị output
                output_coil[FC_VALUE_W1] = output_function[FC_VALUE_W1]; 
                output_coil[FC_VALUE_W2] = output_function[FC_VALUE_W2];
                Write_packet_data_to_output_slave_board(output_coil, INPUT_OUTPUT_SIZE/2); // xuất output_coil cho tay máy chạy
                index_address += INPUT_OUTPUT_SIZE; _function = false;
                break;}
// mã kết thúc 1 chu kỳ chương trình            
            if (get_function == REPEAT_STEP){

                GLCD.text_display_clear(); 
                GLCD.printTxt(1,1,"END PROGRAM"); 
                GLCD.printTxt(3,0, "Press Stop..."); _function = false;
                break;}

          } // end while _function
      } // end while _program  
      choose_program = false;
    } 
    else delay(1);
  } // end modify_program
//============================================================================================ 
// Ghi PROGRAM_NOT_READY vào eeprom board slave để cho phép vào được teach mode
//============================================================================================ 
  if (COMMAND == BUTTON_DEL){
    if (choose_program == true){

      uint16_t index_address; uint8_t data_word_length = 10;
      uint16_t data_delete[data_word_length];
      uint8_t write_eeprom_status_01, write_eeprom_status_02;
      for (int i = 0; i < data_word_length; i++) {data_delete[i] = 0xFFFF;}
      index_address = Program_start_addr + 2;

      // Ghi mã PROGRAM_NOT_READY vào địa chỉ index_address epprom board slave
      write_eeprom_status_01 = Write_single_data_to_Eeprom_slave_board (PROGRAM_NOT_READY, index_address);
      // Ghi 10 word FFFF vào cách thanh ghi tiếp theo
      write_eeprom_status_02 = Write_packet_data_to_Eeprom_slave_board(data_delete, data_word_length, index_address + 2); 
      if ((write_eeprom_status_01 != WRITE_DONE)) {
          Display_eeprom_error(prog_name, index_address);
      }
      else {
      GLCD.text_display_clear(); 
      GLCD.printTxt(1,1,"DELETED DONE"); 
      GLCD.printTxt(3,0, "Press OP.0/OP.1");
      choose_program = false;  
      }
    }
    else delay(1);
  }

*/
//============================================================================================ 
// Chế độ monitor: đọc tín hiệu input và output của robot
//============================================================================================ 
  if (commandKeyPad == MONITOR){
    GLCD.text_display_clear(); 
    GLCD.printTxt(0,0,"GIAM SAT INPUT");
    uint8_t pageInput = 1; uint8_t prepageInput = 0;
    uint16_t inputSignal; uint16_t preInputSignal = 0xFFFF; 
    int8_t commandMonitor = NO_KEY_ACTIVE;
    bool monitorMode = true;
    char *strbitInput[20] = {}; // Lưu giá trị của inputSignal theo chuỗi ON/OFF
    //TIMER4_INTERRUPTS_ON; //read_input_from_PLC(); 
    while (monitorMode) {
      inputSignal = read_input_from_PLC(); //readInputSignals; 
      // đọc giá trị inputSignal từ PLC nếu có tín hiệu thay đổi thì hiển thị lên màn hình GLCD
      // đọc giá trị inputSignals từ PLC nếu có tín hiệu thay đổi thì hiển thị lên màn hình GLCD
      for (int i = 0; i < 8; i++) { 
        strbitInput[i] = bitRead(lowByte(inputSignal),i) == ON ? "ON " : "OFF";
        strbitInput[i+8] = bitRead(highByte(inputSignal),i) == ON ? "ON " : "OFF";
      }
      if (pageInput == 1){
        GLCD.printTxt(1,0,"VT HOME:   "); //0
        GLCD.printTxt(2,0,"VT GAP:    "); //1
        GLCD.printTxt(3,0,"PEN LEN:   "); //2
        for (int i = 0; i < 3; i++){ GLCD.printTxt(i+1, 6, strbitInput[i]); } 
      }
      else if (pageInput == 2){
        GLCD.printTxt(1,0,"PEN LUI:   "); //3
        GLCD.printTxt(2,0,"CHAN KHONG:"); //4
        GLCD.printTxt(3,0,"XOAY XUONG:"); //5
        for (int i = 0; i < 3; i++){ GLCD.printTxt(i+1, 6, strbitInput[i+3]); }  
      }
      else if (pageInput == 3){
        GLCD.printTxt(1,0,"XOAY LEN:  "); //6
        GLCD.printTxt(2,0,"PEN XUONG: "); //7
        GLCD.printTxt(3,0,"PEN KEP:   "); //8
        for (int i = 0; i < 3; i++){ GLCD.printTxt(i+1, 6, strbitInput[i+6]); }  
      }
      else if (pageInput == 4){
        GLCD.printTxt(1,0,"PEN TOI:   "); //9
        GLCD.printTxt(2,0,"CB SP:     "); //10
        GLCD.printTxt(3,0,"ROBOT MODE:"); //11
        for (int i = 0; i < 3; i++){ GLCD.printTxt(i+1, 6, strbitInput[i+9]); }  
      }
      else if (pageInput == 5){
        GLCD.printTxt(1,0,"DONG KHUON:"); //12
        GLCD.printTxt(2,0,"MO KHUON:  "); //13
        GLCD.printTxt(3,0,"*****:     "); //14
        for (int i = 0; i < 3; i++){ GLCD.printTxt(i+1, 6, strbitInput[i+12]); }  
      }
      else if (pageInput == 6){
        GLCD.printTxt(1,0,"*****:     "); GLCD.printTxt(1, 6, strbitInput[15]); //15
        GLCD.printTxt(2,0,"X30:       "); GLCD.printTxt(2, 6, "OFF"); 
        GLCD.printTxt(3,0,"X31:       "); GLCD.printTxt(3, 6, "OFF"); 
      }
      else delay(1);
      
      commandMonitor = getSingleKeypad(RELEASED);
      switch(commandMonitor){
        case MONITOR: monitorMode = false; Show_Infor(); break;
        case RIGHT: pageInput++; if (pageInput > 6) {pageInput = 6;} break;
        case LEFT:  pageInput--; if (pageInput < 1) {pageInput = 1;} break;
      }
      delay(50);
    }
  }
//============================================================================================
// Vào chế độ Manual, điều khiển các Output của máy
//============================================================================================
  if (commandKeyPad == MANUAL) {
      bool COMMAND_MANUAL_MODE = false;
      static uint16_t inputSignals;
      static char *strbitInputSignals[16] = {}; // Lưu giá trị của inputSignals theo chuỗi ON/OFF
      int8_t commandCount  = -1;
      NODE.writeSingleCoil(MANUAL_ADDR,ON);
      // Đọc giá trị manual M168 kiểm tra đã vào chế độ manual chưa.
      while (true) {
          static uint8_t checkManualCnt = 0;
          uint8_t state_manual = Read_SingleCoil_PLC(STATE_MANUAL_ADDR, 1);
          // Đọc tín hiệu state_manual 10 lần nếu không có tín hiệu thì thoát vòng lặp
          // Không vào chế độ manual nữa, tránh bị treo bộ điều khiển
          checkManualCnt++; if (checkManualCnt == 10) {
            GLCD.text_display_clear();
            GLCD.printTxt(1,0," MANUAL FAILED");
            GLCD.printTxt(2,0,"KiemTra TinHieu");
            checkManualCnt = 0;  break;
            }
          // Nếu có tín hiệu state_manual thì vào manual mode
          if (bitRead(state_manual, 0) == ON) { 
            checkManualCnt = 0;
            GLCD.text_display_clear(); 
            GLCD.printTxt(0,1,"MANUAL_MODE");
            GLCD.printTxt(1,0,"Action:"); 
            GLCD.printTxt(3,1, "Press PgDn");
            COMMAND_MANUAL_MODE = true;
            stateRobot = stateManual;
            //TIMER4_INTERRUPTS_ON;
            break; 
          }
          delay(100);
      }
      NODE.writeSingleCoil(MANUAL_ADDR,OFF);
      int8_t COMMAND_MANUAL = NO_KEY_ACTIVE;
      while (COMMAND_MANUAL_MODE) {
        COMMAND_MANUAL = getSingleKeypad(RELEASED);
        switch (COMMAND_MANUAL){
          case XOAY_LEN: 
            commandCount = banXoayLen;
            GLCD.printTxt(2,0,">Tay xoay len  "); break;
          
          case XOAY_XUONG: 
            commandCount = banXoayXuong;                                  
            GLCD.printTxt(2,0,">Tay xoay xuong"); break;

          case TAY_TOI:
            commandCount = penToi;                                     
            GLCD.printTxt(2,0,">Tay di toi    "); break;

          case TAY_LUI:
            commandCount = penLui;                                     
            GLCD.printTxt(2,0,">Tay di lui    "); break;

          case TAY_LEN:
            commandCount = penLen;                                            
            GLCD.printTxt(2,0,">Tay di len    "); break;

          case TAY_XUONG:
            commandCount = penXuong;                                      
            GLCD.printTxt(2,0,">Tay di xuong  "); break;

          case VT_THA:
          // Đưa tay máy về vị trí 0
            commandCount = toiVtTha;                                      
            GLCD.printTxt(2,0,">Toi vi tri tha"); break;

          case VT_GAP:
          // Đưa tay máy về vị trí gắp sản phẩm
            commandCount = toiVtGap;                                       
            GLCD.printTxt(2,0,">Toi vi tri gap"); break;

          case KEP_SP:
            commandCount = kepSanPham;                                          
            GLCD.printTxt(2,0,">Kep san pham  "); break;

          case HUT_SP:
            commandCount = hutSanPham;
            GLCD.printTxt(2,0,">Hut san pham  "); break;

          case THA_SP:
            commandCount = thaSanPham;                                    
            GLCD.printTxt(2,0,">Tha san pham  "); break;

          case PGDN:      
            Send_manual_command_to_PLC(commandCount);
            manualAction = commandCount;
            break; 

          case STOP_1:
          case STOP_2:
            commandCount = -1; manualAction = -1;
            Write_SingleCoil_PLC(TOI_VT_THA_ADDR, OFF); 
            Write_SingleCoil_PLC(TOI_VT_GAP_ADDR, OFF);
            GLCD.text_display_clear(); 
            GLCD.printTxt(0,1,"MANUAL_MODE");
            GLCD.printTxt(1,0,"Action:"); 
            GLCD.printTxt(3,1, "Press PgDn");
            break;

          case MANUAL:  // Exit the manual mode
            commandCount = -1; manualAction = -1;
            //TIMER4_INTERRUPTS_OFF;
            stateRobot = idle;
            uint8_t resultRequest = NODE.writeSingleCoil(RESET_MANUAL_ADDR,ON);
            if (resultRequest == 0) { 
              COMMAND_MANUAL_MODE = false;
              GLCD.text_display_clear(); Show_Infor(); 
              break;
            }
        } // end switch
      } // end while COMMAND_MANUAL_MODE
  }
//================================================================================================================
// Gửi command tới board slaver đưa tay máy về vị trí HOME (vị trí thả sản phẩm)
//================================================================================================================
  if (commandKeyPad == HOME) {
    uint8_t result = 1;
    uint16_t *currentPosPacket;  uint32_t currentPos; uint32_t preCurrentPos = 0;
    uint32_t currentPosindex[2];
    char *strCurrentPos; char strBuffer[16]; 
    bool enterHomeMode = false;
    Write_SingleCoil_PLC(HOME_ADDR, ON);
    while (true) {
      static uint8_t checkFlagHomeCnt = 0;
      uint8_t flagHome = Read_SingleCoil_PLC(GOING_HOME_ADDR, 1);
      if (bitRead(flagHome,0) == ON) {enterHomeMode = true; Write_SingleCoil_PLC(HOME_ADDR, OFF); checkFlagHomeCnt = 0; break; }
      checkFlagHomeCnt++; 
      if (checkFlagHomeCnt == 15) { 
        checkFlagHomeCnt = 0;
        Write_SingleCoil_PLC(HOME_ADDR, OFF); 
        GLCD.text_display_clear();
        GLCD.printTxt(1,0,"GO HOME FAILED");
        GLCD.printTxt(2,0,"KiemTra TinHieu");
        break; 
      }
      delay(100);
    } 
    if (enterHomeMode) { 
      GLCD.text_display_clear();
      GLCD.printTxt(0,0,"GOING TO HOME");
      GLCD.printTxt(1,1,"Position: ");
      while (true) {
        static uint8_t waitingGoHomeCnt = 0; // biến đếm chờ về tới home
        currentPosPacket = Read_Resgister_PLC(CURRENT_POSTIONS_ADDR, 2);  // nên để trong ngắt timer
        currentPosindex[0] = currentPosPacket[0]; currentPosindex[1] = currentPosPacket[1]; 
        currentPos = currentPosindex[1] << 16 | currentPosindex[0]; 
        if (currentPos != preCurrentPos) { 
          preCurrentPos =  currentPos; 
          GLCD.printTxt(2,0,"               ");
        }
        strCurrentPos = ultoa(currentPos, strBuffer, 10);
        GLCD.printTxt(2,1,strCurrentPos);

        uint8_t value_home = Read_SingleCoil_PLC(GOING_HOME_ADDR, 1);
        if (bitRead(value_home,0) == OFF) { 
          GLCD.printTxt(0,0,"GO HOME DONE !!"); 
          waitingGoHomeCnt = 0;
          break;
        }
        // Đếm 200 lần = 20s, sau 20s mà vẫn chưa về tới HOME thì thoat khỏi vòng lặp để tránh bị treo
        waitingGoHomeCnt++; if (waitingGoHomeCnt == 200) {waitingGoHomeCnt = 0; GLCD.printTxt(0,0,"GO HOME FAILED"); break;}
        delay(100);
      }
    }
  }
//============================================================================================ 
// Dừng tay máy tại chỗ
//============================================================================================ 
  if (commandKeyPad == STOP_1 || commandKeyPad == STOP_2) {
    uint8_t resultRequest = 1;
    char *strStopRobot[] = {"STOP ROBOT.    ", "STOP ROBOT..   ", "STOP ROBOT...  ", "STOP ROBOT.... ","STOP ROBOT....."};
    uint8_t strStopRobotCnt = 0;
    resultRequest = NODE.writeSingleCoil(RESET_ADDR,ON);
    if (resultRequest == 0) { 
      NODE.writeSingleCoil(RESET_ADDR,OFF);
      GLCD.text_display_clear(); 
      GLCD.printTxt(0,1,"STOP ROBOT");
      while(true){
        static uint8_t checkStopCnt = 0;
        // chờ cờ reset done set on
        GLCD.printTxt(2,0,strStopRobot[strStopRobotCnt]);
        uint8_t value_reset = Read_SingleCoil_PLC(STATE_RESET_DONE_ADDR, 1);
        // Giới hạn thời gian kích stop trong vòng lặp tối đa 30 lần tránh treo bộ điều khiển
        checkStopCnt ++; if (checkStopCnt == 30) {
          checkStopCnt = 0; NODE.writeSingleCoil(STATE_RESET_DONE_ADDR, OFF); 
          GLCD.printTxt(2,0,"..STOP FAILED..");
          break; 
          }
        if (bitRead(value_reset,0) == ON) { 
          GLCD.printTxt(2,0,"STOP ROBOT DONE");
          checkStopCnt = 0;
          stateRobot = idle;
          // xóa cờ reset done
          NODE.writeSingleCoil(STATE_RESET_DONE_ADDR, OFF);
          break; 
        }
        strStopRobotCnt++; if (strStopRobotCnt > 4 ) { strStopRobotCnt = 0; }
        delay(100);
      }
    }
  }
//============================================================================================
// Bật chế độ chạy auto
// Có 2 trường hợp: 
// 1: Bật auto ở vị trí gắp
// 2: Bật auto ở khác vị trí gắp
//============================================================================================
  if (commandKeyPad == AUTO_1 || commandKeyPad == AUTO_2){
    GLCD.text_display_clear();
    GLCD.printTxt(0,1,"PLC PROGRAM"); 
    GLCD.printTxt(1,0," Auto PLC Run ");
    while(true) {
      Write_SingleCoil_PLC(AUTO_COIL_ADDR, ON); 
      uint8_t autoState = Read_SingleCoil_PLC(STATE_AUTO_ADDR, 1);
      static uint8_t checkAutoCnt = 0;
      if (bitRead(autoState, 0) == ON) { 
        Write_SingleCoil_PLC(AUTO_COIL_ADDR, OFF); 
        GLCD.printTxt(2,0," AUTO RUNNING  ");
        stateRobot = stateAutoRun;
        checkAutoCnt = 0;
        break; 
      }
      // Kiểm tra trạng thái feedback auto 100 lần (10s)
      // 10s là thời gian tối đa để chờ tay máy vào vị trí gắp và chạy auto
      // Nếu M171 không có phản hồi thì thoát khỏi vòng lặp while, tránh bị treo chương trình.
      checkAutoCnt ++;
      if (checkAutoCnt == 100) { 
        Write_SingleCoil_PLC(AUTO_COIL_ADDR, OFF);  
        GLCD.printTxt(2,0,"AUTO RUN FAILED");
        GLCD.printTxt(3,0,"KiemTra TinHieu");
        checkAutoCnt = 0;
        break;
      }
      delay(100); 
    }
  }
//============================================================================================ 
// Che do remotr: stream trạng thái chạy tự động của PLC
// 1. Chu kỳ ép của máy ép.
// 2. Chu kỳ lấy sản phẩm của tay máy.
// 3. Số lượng sản phẩm đã lấy.
//============================================================================================
  if (commandKeyPad == REMOTR){
    GLCD.text_display_clear();
    GLCD.printTxt(0,0,"Quatity:"); 
    GLCD.printTxt(1,0,"Robot:");
    GLCD.printTxt(2,0,"MayEp:");
    GLCD.printTxt(3,0,"dd/mm/yy");
    bool remotrMode = true;
    uint16_t *reMotrPacket; uint16_t preProductQty = 6000;
    char *strProductQty; char *strRBcycleTime; char *strJMcycleTime; char strBuffer[16];
    while(remotrMode){
      // đọc số lượng sản phẩm robot đã lấy được
      reMotrPacket = Read_Resgister_PLC(PRODUCT_QUATITY_ADDR, 3); // read: D15,D16,D17
      if (reMotrPacket[0] != preProductQty) { 
        preProductQty =  reMotrPacket[0]; 
        strProductQty = ltoa(reMotrPacket[0], strBuffer, 10);
        GLCD.printTxt(0,5,"    " );
        GLCD.printTxt(0,5,strProductQty);
      }
      // đếm chu kỳ robot lấy sản phẩm ra khỏi khuôn: tính từ lúc tay máy xuống lấy sản phẩm -> kích đóng khuôn ép.
      strRBcycleTime = convert_3float_number_to_str(reMotrPacket[1]);
      GLCD.printTxt(1,4,strRBcycleTime);
      // đếm chu kỳ máy ép nhựa
      strJMcycleTime = convert_4float_number_to_str(reMotrPacket[2]);
      GLCD.printTxt(2,4,strJMcycleTime);
      // Thoát khỏi remotrMode
      int8_t commandRemotr = getSingleKeypad(RELEASED);
      switch(commandRemotr){
        case REMOTR: remotrMode = false; Show_Infor(); break;
      }
      delay(50);
    }
  }
  delay(100);
}
// END LOOP
//================================================================================================================
// Hiện thị epprom error ở chương trình program_name
void Display_eeprom_error(uint16_t program_name, uint16_t addr){
  Serial.print("Eeprom bị lỗi bắt đầu ở địa chỉ: "); Serial.println(addr,HEX);
  GLCD.text_display_clear();
  GLCD.printTxt(0,0,"OP0 "); GLCD.printTxt(0,2,"PROG"); GLCD.print_number(0,4,program_name);
  GLCD.printTxt(2,1,"EEPROM ERROR");
}
//================================================================================================================
// Chuyển đổi chuỗi thành số delay
//================================================================================================================
uint16_t convert_str_to_delay_number(char* stringArr)
{
  int myflag = 0; float s = 0; float v = 0;
  int i = 1; int sign = 1;
  float result = 0;

  for(int m = 0; m < strlen(stringArr) + 1; m++)
    {
      char ch = stringArr[m];
      switch(ch)
      {
       case '0'...'9':
                if (myflag == 0)            // tach phan nguyen
                  {
                    v = v*10 + ch - '0';    // đổi kiểu char thành kiểu số
                  }
                if (myflag == 1)            // tach phan thap phan
                  {
                    s = ch - '0';
                    v = v + s*(pow(10,-i));
                    i += 1;
                  }
                break;
       case '\0': 
                result = v*sign;            // Toa do Y
                break;
       case '.':
                myflag = 1;
                break;
       case '-':
                sign = -1;
                break;          
      }
    }
    result = result*10; 
    if (result > 90) {result = 90;} // Giới hạn giá trị delay tối đa là 9 giây
    return int(result);
 }
//================================================================================================================
// Chuyển đổi số thực thành chuỗi delay: 1.250 -> "1.250"
// Xử lý số tối đa là 2 số nguyên 99 -> 9.9
//================================================================================================================
char *convert_delay_number_to_str(uint16_t value){
    static char result[5];
    float delay_value = value/10; // lấy giá trị số thực delay_value cần hiển thị
    result[0] = value/10%10 + '0';
    result[1] = '.';
    result[2] = value%10 + '0';
    result[3] = 's';
    result[4] = '\0';

    return result;
}
//================================================================================================================
// Chuyển đổi số thực thành chuỗi delay 1 số thập phân: 1.2 -> "1.2"
// Xử lý tối đa 3 số nguyên 999 -> 99.9
//================================================================================================================
char *convert_3float_number_to_str(uint16_t value){
    static char result[6];
    float delay_value = value/10; // lấy giá trị số thực delay_value cần hiển thị

    result[0] = value/100%10 + '0';
    result[1] = value/10%10 + '0';
    result[2] = '.';
    result[3] = value%10 + '0';
    result[4] = 's';
    result[5] = '\0';

    return result;
}
//================================================================================================================
// Chuyển đổi số thực thành chuỗi delay 1 số thập phân: 1.2 -> "1.2"
// Xử lý tối đa 4 số nguyên 9999 -> 999.9
//================================================================================================================
char *convert_4float_number_to_str(uint16_t value){
    static char result[7];
    float delay_value = value/10; // lấy giá trị số thực delay_value cần hiển thị

    result[0] = value/1000%10 + '0';
    result[1] = value/100%10 + '0';
    result[2] = value/10%10 + '0';
    result[3] = '.';
    result[4] = value%10 + '0';
    result[5] = 's';
    result[6] = '\0';

    return result;
}
//================================================================================================================
// Kiểm tra trạng thái các bit trong thanh ghi output coil
// In ra GLCD mã HEX output_coil[FC_VALUE_W2]
//================================================================================================================
void Check_status_of_output_coil_word(uint16_t data){
    uint8_t HighData = highByte(data); uint8_t code[4];
    uint8_t LowData = lowByte(data); float num_index = 0;
    uint8_t Number_STT[4];

    float num0,num1,num2,num3;
    num0 = num1 = num2 = num3 = 0;

    for (int i = 4; i < 8; i++){
      float bit = bitRead(HighData,i); float number = pow(2,i-4); 
      num_index =  bit*number; num0 += num_index; // code = 0 ... F
    } int code0 = round(num0); code[0] = Number_STT[code0];
      
    for (int j = 0; j < 4; j++) {
      float bit = bitRead(HighData,j); float number = pow(2,j); 
      num_index =  bit*number; num1 += num_index; // code = 0 ... F   
    } int code1 = round(num1); code[1] = Number_STT[code1];

    for (int k = 4; k < 8; k++){
      float bit = bitRead(LowData,k); float number = pow(2,k-4); 
      num_index =  bit*number; num2 += num_index; // code = 0 ... F
    } int code2 = round(num2); code[2] = Number_STT[code2];

    for (int f = 0; f < 4; f++) {
      float bit = bitRead(LowData,f); float number = pow(2,f); 
      num_index =  bit*number; num3 += num_index; // code = 0 ... F   
    } int code3 = round(num3); code[3] = Number_STT[code3]; 

    GLCD.print_u16code(1,4,code);
}