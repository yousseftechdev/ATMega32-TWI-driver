# Chnages:
- Finished bus scanner example
- Fixed interrupt enable reset bug (TWIE was being reset by every eevent)
- Fixed flags being optimized away by compiler at the start of program
- Fixed broken bus scanner example
- Added README
- Added UART driver as a dependency of the example code
- Added prebuilt hex file for a quick test
- Changed `TWI_u8GetTransactionStatus` to `TWI_boolGetTransactionStatus` and it returns `bool` now