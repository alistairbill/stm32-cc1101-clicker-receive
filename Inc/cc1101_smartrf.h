#ifndef SMARTRF_CC1100_H
#define SMARTRF_CC1100_H

// Address Config = No address check
// Base Frequency = 433.919830
// CRC Autoflush = false
// CRC Enable = false
// Carrier Frequency = 433.919830
// Channel Number = 0
// Channel Spacing = 199.951172
// Data Format = Asynchronous serial mode
// Data Rate = 1.19948
// Deviation = 5.157471
// Device Address = 0
// Manchester Enable = false
// Modulation Format = ASK/OOK
// PA Ramping = false
// Packet Length = 255
// Packet Length Mode = Infinite packet length mode
// Preamble Count = 4
// RX Filter BW = 58.035714
// Sync Word Qualifier Mode = No preamble/sync
// TX Power = 0
// Whitening = false
// PA table
const uint8_t PA_TABLE[] = { 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// RF settings for CC1101 (which seem to work)
// Taken from https://github.com/maksms/wifi-iot-cda/blob/master/rapid/esp8266/hardware_rf_cc1101/code
// 433MHz OOK with output on GDO0
const uint8_t RF_SETTINGS[] = {
  0x7F,  // IOCFG2              GDO2 Output Pin Configuration
  0x2E,  // IOCFG1              GDO1 Output Pin Configuration
  0x0D,  // IOCFG0              GDO0 Output Pin Configuration
  0x67,  // FIFOTHR             RX FIFO and TX FIFO Thresholds
  0x57,  // SYNC1               Sync Word, High Byte
  0x43,  // SYNC0               Sync Word, Low Byte
  0xFF,  // PKTLEN              Packet Length
  0x04,  // PKTCTRL1            Packet Automation Control
  0x32,  // PKTCTRL0            Packet Automation Control
  0x00,  // ADDR                Device Address
  0x00,  // CHANNR              Channel Number
  0x06,  // FSCTRL1             Frequency Synthesizer Control
  0x00,  // FSCTRL0             Frequency Synthesizer Control
  0x10,  // FREQ2               Frequency Control Word, High Byte
  0xB0,  // FREQ1               Frequency Control Word, Middle Byte
  0x71,  // FREQ0               Frequency Control Word, Low Byte
  0xA7,  // MDMCFG4             Modem Configuration
  0x32,  // MDMCFG3             Modem Configuration
  0x30,  // MDMCFG2             Modem Configuration
  0x22,  // MDMCFG1             Modem Configuration
  0xF8,  // MDMCFG0             Modem Configuration
  0x50,  // DEVIATN             Modem Deviation Setting
  0x07,  // MCSM2               Main Radio Control State Machine Configuration
  0x30,  // MCSM1               Main Radio Control State Machine Configuration
  0x18,  // MCSM0               Main Radio Control State Machine Configuration
  0x16,  // FOCCFG              Frequency Offset Compensation Configuration
  0x6C,  // BSCFG               Bit Synchronization Configuration
  0x04,  // AGCCTRL2            AGC Control
  0x00,  // AGCCTRL1            AGC Control
  0x92,  // AGCCTRL0            AGC Control
  0x02,  // WOREVT1             High Byte Event0 Timeout
  0x26,  // WOREVT0             Low Byte Event0 Timeout
  0x09,  // WORCTRL             Wake On Radio Control
  0xB6,  // FREND1              Front End RX Configuration
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
/*
  0x00,  // PARTNUM             Chip ID
  0x04,  // VERSION             Chip ID
  0x00,  // FREQEST             Frequency Offset Estimate from Demodulator
  0x00,  // LQI                 Demodulator Estimate for Link Quality
  0x00,  // RSSI                Received Signal Strength Indication
  0x00,  // MARCSTATE           Main Radio Control State Machine State
  0x00,  // WORTIME1            High Byte of WOR Time
  0x00,  // WORTIME0            Low Byte of WOR Time
  0x00,  // PKTSTATUS           Current GDOx Status and Packet Status
  0x00,  // VCO_VC_DAC          Current Setting from PLL Calibration Module
  0x00,  // TXBYTES             Underflow and Number of Bytes
  0x00,  // RXBYTES             Overflow and Number of Bytes
  0x00,  // RCCTRL1_STATUS      Last RC Oscillator Calibration Result
  0x00,  // RCCTRL0_STATUS      Last RC Oscillator Calibration Result
};
*/
#endif
