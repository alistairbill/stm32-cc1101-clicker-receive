#include "stm32f4xx_hal.h"
#include "main.h"
#include "cc1101.h"
#include "cc1101_smartrf.h"

typedef enum {
  SRES     = 0x30,         // Reset chip
  SFSTXON  = 0x31,         // Enable/calibrate freq synthesizer
  SXOFF    = 0x32,         // Turn off crystal oscillator.
  SCAL     = 0x33,         // Calibrate freq synthesizer & disable
  SRX      = 0x34,         // Enable RX.
  STX      = 0x35,         // Enable TX.
  SIDLE    = 0x36,         // Exit RX / TX
  SAFC     = 0x37,         // AFC adjustment of freq synthesizer
  SWOR     = 0x38,         // Start automatic RX polling sequence
  SPWD     = 0x39,         // Enter pwr down mode when CSn goes hi
  SFRX     = 0x3A,         // Flush the RX FIFO buffer.
  SFTX     = 0x3B,         // Flush the TX FIFO buffer.
  SWORRST  = 0x3C,         // Reset real time clock.
  SNOP     = 0x3D         // No operation.
} CC1101_COMMAND_STROBE;

typedef enum {
  // config
  IOCFG2   = 0x00,         // GDO2 output pin configuration
  IOCFG1   = 0x01,         // GDO1 output pin configuration
  IOCFG0   = 0x02,         // GDO0 output pin configuration
  FIFOTHR  = 0x03,         // RX FIFO and TX FIFO thresholds
  SYNC1    = 0x04,         // Sync word, high byte
  SYNC0    = 0x05,         // Sync word, low byte
  PKTLEN   = 0x06,         // Packet length
  PKTCTRL1 = 0x07,         // Packet automation control
  PKTCTRL0 = 0x08,         // Packet automation control
  ADDR     = 0x09,         // Device address
  CHANNR   = 0x0A,        // Channel number
  FSCTRL1  = 0x0B,         // Frequency synthesizer control
  FSCTRL0  = 0x0C,         // Frequency synthesizer control
  FREQ2    = 0x0D,         // Frequency control word, high byte
  FREQ1    = 0x0E,         // Frequency control word, middle byte
  FREQ0    = 0x0F,         // Frequency control word, low byte
  MDMCFG4  = 0x10,         // Modem configuration
  MDMCFG3  = 0x11,         // Modem configuration
  MDMCFG2  = 0x12,         // Modem configuration
  MDMCFG1  = 0x13,         // Modem configuration
  MDMCFG0  = 0x14,         // Modem configuration
  DEVIATN  = 0x15,         // Modem deviation setting
  MCSM2    = 0x16,         // Main Radio Cntrl State Machine config
  MCSM1    = 0x17,        // Main Radio Cntrl State Machine config
  MCSM0    = 0x18,         // Main Radio Cntrl State Machine config
  FOCCFG   = 0x19,         // Frequency Offset Compensation config
  BSCFG    = 0x1A,         // Bit Synchronization configuration
  AGCCTRL2 = 0x1B,         // AGC control
  AGCCTRL1 = 0x1C,         // AGC control
  AGCCTRL0 = 0x1D,         // AGC control
  WOREVT1  = 0x1E,         // High byte Event 0 timeout
  WOREVT0  = 0x1F,         // Low byte Event 0 timeout
  WORCTRL  = 0x20,         // Wake On Radio control
  FREND1   = 0x21,         // Front end RX configuration
  FREND0   = 0x22,         // Front end TX configuration
  FSCAL3   = 0x23,         // Frequency synthesizer calibration
  FSCAL2   = 0x24,         // Frequency synthesizer calibration
  FSCAL1   = 0x25,         // Frequency synthesizer calibration
  FSCAL0   = 0x26,         // Frequency synthesizer calibration
  RCCTRL1  = 0x27,         // RC oscillator configuration
  RCCTRL0  = 0x28,         // RC oscillator configuration
  FSTEST   = 0x29,         // Frequency synthesizer cal control
  PTEST    = 0x2A,         // Production test
  AGCTEST  = 0x2B,         // AGC test
  TEST2    = 0x2C,         // Various test settings
  TEST1    = 0x2D,         // Various test settings
  TEST0    = 0x2E,         // Various test settings
  // status
  PARTNUM        = 0xF0,   // Part number
  VERSION        = 0xF1,   // Current version number
  FREQEST        = 0xF2,   // Frequency offset estimate
  LQI            = 0xF3,   // Demodulator estimate for link quality
  RSSI           = 0xF4,   // Received signal strength indication
  MARCSTATE      = 0xF5,   // Control state machine state
  WORTIME1       = 0xF6,   // High byte of WOR timer
  WORTIME0       = 0xF7,   // Low byte of WOR timer
  PKTSTATUS      = 0xF8,   // Current GDOx status and packet status
  VCO_VC_DAC     = 0xF9,   // Current setting from PLL cal module
  TXBYTES        = 0xFA,   // Underflow and # of bytes in TXFIFO
  RXBYTES        = 0xFB,   // Overflow and # of bytes in RXFIFO
  RCCTRL1_STATUS = 0xFC,   //Last RC Oscillator Calibration Result
  RCCTRL0_STATUS = 0xFD   //Last RC Oscillator Calibration Result
} CC1101_REGISTER;

typedef enum {
  SLEEP             = 0,   // SLEEP
  IDLE              = 1,   // IDLE
  XOFF              = 2,   // XOFF
  VCOON_MC          = 3,   // MANCAL
  REGON_MC          = 4,   // MANCAL
  MANCAL            = 5,   // MANCAL
  VCOON             = 6,   // FS_WAKEUP
  REGON             = 7,   // FS_WAKEUP
  STARTCAL          = 8,   // CALIBRATE
  BWBOOST           = 9,   // SETTLING
  FS_LOCK           = 10,  // SETTLING
  IFADCON           = 11,  // SETTLING
  ENDCAL            = 12,  // CALIBRATE
  RX                = 13,  // RX
  RX_END            = 14,  // RX
  RX_RST            = 15,  // RX
  TXRX_SWITCH       = 16,  // TXRX_SETTLING
  RXFIFO_OVERFLOW   = 17,  // RXFIFO_OVERFLOW
  FSTXON            = 18,  // FSTXON
  TX                = 19,  // TX
  TX_END            = 20,  // TX
  RXTX_SWITCH       = 21,  // RXTX_SETTLING
  TXFIFO_UNDERFLOW  = 22,  // TXFIFO_UNDERFLOW
} CC1101_MARCSTATE;

extern SPI_HandleTypeDef hspi1;
static int initialized = 0;

static void write_read(const uint8_t *tx, uint8_t *rx, uint16_t length)
{
	// casts are required to avoid a warning about discarding the const qualifier
	if (!rx) {
		HAL_SPI_Transmit(&hspi1, (uint8_t*)tx, length, 100);
	} else {
		HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)tx, rx, length, 100);
	}
}

static void chip_select(int state)
{
	HAL_GPIO_WritePin(CC1101_NSS_GPIO_Port, CC1101_NSS_Pin, state ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void wait_chip_ready()
{
  uint32_t spinlock_protection = 0;
	while(HAL_GPIO_ReadPin(CC1101_GDO2_GPIO_Port, CC1101_GDO2_Pin) == GPIO_PIN_SET
    && ++spinlock_protection < 10000);
  if (spinlock_protection >= 10000) {
    Error_Handler();
  }
}

static uint8_t strobe(CC1101_COMMAND_STROBE byte)
{
	chip_select(1);
	wait_chip_ready();
	uint8_t rx = 0;
	write_read(&byte, &rx, 1);
	chip_select(0);
	return rx;
}

static uint8_t read_register(CC1101_REGISTER addr)
{
	chip_select(1);
	wait_chip_ready();
	const uint8_t tx[2] = { addr | 0x80, 0xFF };
	uint8_t rx[2] = { 0 };
	write_read(tx, rx, sizeof(tx));
	chip_select(0);
	return rx[1];
}

static void read_burst(CC1101_REGISTER addr, uint8_t *buffer, size_t length)
{
	chip_select(1);
	wait_chip_ready();
	uint8_t tx = addr | 0xC0;
	write_read(&tx, NULL, sizeof(tx));
	tx = 0xFF;
	write_read(&tx, buffer, length);
	chip_select(0);
}

static void write_register(CC1101_REGISTER, uint8_t) __attribute__ ((unused));
static void write_register(CC1101_REGISTER addr, uint8_t value)
{
	chip_select(1);
	wait_chip_ready();
	uint8_t tx[2] = { addr, value };
	write_read(tx, NULL, sizeof(tx));
	chip_select(0);
}

static void write_burst(CC1101_REGISTER addr, const uint8_t *buffer, size_t length)
{
	chip_select(1);
	wait_chip_ready();
	uint8_t tx = addr | 0x40;
	write_read(&tx, NULL, sizeof(tx));
	write_read(buffer, NULL, length);
	chip_select(0);
}

static void write_rf_settings(const uint8_t *settings, size_t length)
{
	write_burst(0, settings, length);
}

static void write_pa_table(const uint8_t *pa_table)
{
	write_burst(0x7E, pa_table, 8);
}

static void read_pa_table(uint8_t *pa_table)
{
	// 0x7E to read single
	read_burst(0x7E, pa_table, 8);
}

static void read_rf_settings(uint8_t *settings, size_t length)
{
	read_burst(0, settings, length);
}

static void reset()
{
	chip_select(1);
	chip_select(0);
	strobe(SRES);
}

static void wait_for_marcstate(CC1101_MARCSTATE expected_state)
{
	uint8_t state = 0;
	do {
		state = read_register(MARCSTATE);
	} while (state != expected_state);
}

static void flush_rx_fifo()
{
	strobe(SFRX);
	wait_for_marcstate(IDLE);
}

static void sidle()
{
	strobe(SIDLE);
	wait_for_marcstate(IDLE);
}

static void start_rx()
{
	sidle();
	flush_rx_fifo();
	strobe(SRX);
	wait_for_marcstate(RX);
}

int CC1101_Init()
{
	if (initialized) return -1;

	reset();
	write_rf_settings(RF_SETTINGS, sizeof(RF_SETTINGS));
	write_pa_table(PA_TABLE);

	printf("CC1101: PARTNUM = 0x%02X\n", read_register(PARTNUM));
	printf("CC1101: VERSION = 0x%02X\n", read_register(VERSION));

	uint8_t rf_settings[sizeof(RF_SETTINGS)] = { 0 };
	read_rf_settings(rf_settings, sizeof(rf_settings));
	for (size_t i = 0; i < sizeof(rf_settings); i++) {
		printf("CC1101: RF_SETTINGS @ 0x%02X = 0x%02X\n", i, rf_settings[i]);
	}

	uint8_t pa_table[8] = { 0 };
	read_pa_table(pa_table);
	for (size_t i = 0; i < sizeof(pa_table); i++) {
		printf("CC1101: PATABLE @ 0x%02X = 0x%02X\n", i, pa_table[i]);
	}

	start_rx();

	initialized = 1;
	return 0;
}
