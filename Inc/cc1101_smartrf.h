#ifndef SMARTRF_CC1100_H
#define SMARTRF_CC1100_H

const uint8_t PA_TABLE[] = { 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t RF_SETTINGS[] = {
  0x0D,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x0D,  // IOCFG0              GDO0 Output Pin Configuration
  0x47,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0xD3,  // SYNC1               Sync Word, High Byte
  0x91,  // SYNC0               Sync Word, Low Byte
  0xFF,  // PKTLEN              Packet Length
  0x04,  // PKTCTRL1            Packet Automation Control
  0x32,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x0A,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x21,  // FREQ2               Frequency Control Word, High Byte
  0x62,  // FREQ1               Frequency Control Word, Middle Byte
  0x76,  // FREQ0               Frequency Control Word, Low Byte
  0xF9,  // MDMCFG4             Modem Configuration
  0x3B,  // MDMCFG3             Modem Configuration
  0x30,  // MDMCFG2             Modem Configuration
  0x00,  // MDMCFG1             Modem Configuration
  0x60,  // MDMCFG0             Modem Configuration
  0x15,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x30,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x03,  // AGCCTRL2            AGC Control
  0x40,  // AGCCTRL1            AGC Control
  0x91,  // AGCCTRL0            AGC Control
  0x87,  // WOREVT1             High Byte Event0 Timeout
  0x6B,  // WOREVT0             Low Byte Event0 Timeout
  0xFB,  // WORCTRL             Wake On Radio Control
  0x56,  // FREND1              Front End RX Configuration
  0x11,  // FREND0              Front End TX Configuration
  0xE9,  // FSCAL3              Frequency Synthesizer Calibration
  0x2A,  // FSCAL2              Frequency Synthesizer Calibration
  0x00,  // FSCAL1              Frequency Synthesizer Calibration
  0x1F,  // FSCAL0              Frequency Synthesizer Calibration
  0x41,  // RCCTRL1             RC Oscillator Configuration
  0x00,  // RCCTRL0             RC Oscillator Configuration
  0x59,  // FSTEST              Frequency Synthesizer Calibration Control
  0x7F,  // PTEST               Production Test
  0x3F,  // AGCTEST             AGC Test
  0x81,  // TEST2               Various Test Settings
  0x35,  // TEST1               Various Test Settings
  0x09,  // TEST0               Various Test Settings
};

#endif
